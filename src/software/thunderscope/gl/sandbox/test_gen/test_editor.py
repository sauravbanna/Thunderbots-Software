import pathlib

from jinja2 import Environment, FileSystemLoader

import libcst as cst


class TestEditor:
    """Edits or creates test files by rendering Jinja templates."""

    _TEMPLATES_DIR = pathlib.Path(__file__).parent / "templates"

    # Template for generating a whole new test file
    _TEST_FILE_TEMPLATE = str(_TEMPLATES_DIR / "test_file_template.py.jinja")

    # Template for generating a single test case within an existing test
    _TEST_CASE_TEMPLATE = str(_TEMPLATES_DIR / "test_case_template.py.jinja")

    def __init__(
        self,
        test_file_path: str,
        test_name: str,
        is_new_case: bool,
        **kwargs,
    ):
        """Initialise the TestEditor.

        :param test_file_path: Path to the test file to create or modify.
        :param test_name: Name of the test function.
        :param is_new_case: True if this is a brand-new test case/file,
                            False if modifying an existing test.
        :param kwargs: Additional keyword arguments forwarded to the
                       template renderer.
        """
        self._test_file_path = pathlib.Path(test_file_path)
        self._test_name = test_name
        self._is_new_case = is_new_case
        self._kwargs = kwargs

        # Pick the template based on whether the file already exists
        if self._test_file_path.exists():
            self._template_path = self._TEST_CASE_TEMPLATE
        else:
            self._template_path = self._TEST_FILE_TEMPLATE

    def execute(self) -> None:
        """Render the selected template and write the result to file."""
        rendered = self._render_template(
            self._template_path,
            test_name=self._test_name,
            **self._kwargs,
        )
        self._test_file_path.write_text(rendered)

    @staticmethod
    def get_test_method_names(file_path: str) -> list[str]:
        """
        Parses a Python file and returns the names of all top-level functions
        whose name starts with ``test_``.

        :param file_path: Path to the Python file to scan.
        :return: List of function name strings matching the ``test_*`` pattern.
        :raises FileNotFoundError: If the file does not exist.
        """
        path = pathlib.Path(file_path)
        if not path.exists():
            raise FileNotFoundError(f"The file {file_path} does not exist.")

        source_code = path.read_text()

        module = cst.parse_module(source_code)

        test_method_names = []

        for node in module.body:
            # if it is a top level function starting with "test_"
            if isinstance(node, cst.FunctionDef) and node.name.value.startswith(
                "test_"
            ):
                test_method_names.append(node.name)

        return test_method_names

    def _render_template(self, template_path: str, **kwargs) -> str:
        """
        Reads a jinja template file from disk, renders it with the provided
        keyword arguments, and returns the completed string.

        :param template_path: Path to the .jinja or .txt template file.
        :param kwargs: Key-value pairs matching the variables inside the template.
        :return: Rendered string.
        """
        path = pathlib.Path(template_path).resolve()
        if not path.exists():
            raise FileNotFoundError(f"Template file not found at: {path}")

        template_dir = path.parent
        template_file_name = path.name

        env = Environment(
            loader=FileSystemLoader(template_dir),
            trim_blocks=True,
            lstrip_blocks=True,
        )

        template = env.get_template(template_file_name)
        return template.render(**kwargs)

    class ParametrizeListAppender(cst.CSTTransformer):
        """CST transformer that appends a new test case to an existing
        ``@pytest.mark.parametrize`` decorator on a named test function.

        If the target function has no ``@pytest.mark.parametrize`` decorator, or
        the decorator doesn't have a list of test cases, the node is left unchanged.
        """

        def __init__(self, test_name: str, test_case_to_add: cst.Tuple):
            """
            :param test_name: The name of the test function to modify.
            :param test_case_to_add: A CST Tuple node to append to the
                parametrize decorator's argument list.
            """
            self.test_name = test_name
            self.test_case_to_add = test_case_to_add

        def leave_FunctionDef(
            self, original_node: cst.FunctionDef, updated_node: cst.FunctionDef
        ) -> cst.FunctionDef:
            # skip if not the test we want to add the case to
            if updated_node.name.value != self.test_name:
                return updated_node

            new_decorators = [decorator for decorator in updated_node.decorators]
            for idx, decorator in enumerate(updated_node.decorators):
                # look for the pytest parametrize decorator
                if not (
                    isinstance(decorator.expression, cst.Call)
                    and cst.matchers.matches(
                        decorator.expression.func,
                        cst.matchers.Attribute(
                            attr=cst.matchers.Name("parametrize")
                        ),
                    )
                ):
                    continue

                args = decorator.expression.args
                # parametrize needs 2 args
                if len(args) < 2:
                    continue

                test_cases_arg = args[1].value

                if not isinstance(test_cases_arg, cst.List):
                    continue

                new_test_cases = list(test_cases_arg.elements) + [
                    cst.Element(value=self.test_case_to_add)
                ]
                updated_test_cases = test_cases_arg.with_changes(
                    elements=new_test_cases
                )

                updated_args = list(args)
                updated_args[1] = args[1].with_changes(value=updated_test_cases)

                decorator = decorator.with_changes(
                    expression=decorator.expression.with_changes(args=updated_args)
                )

                new_decorators[idx] = decorator

            return updated_node.with_changes(decorators=new_decorators)

    def _append_case_to_test(
        self,
        test_file_path: str,
        target_test_function_name: str,
        template_path: str,
        template_args: dict,
    ):
        """
        Renders a Jinja template test case and injects it into the
        ``@pytest.mark.parametrize`` block of a specific top-level test function.

        :param test_file_path: Path to the Python test file to modify.
        :param target_test_function_name: Name of the top-level test function whose
            parametrize block should receive the new case.
        :param template_path: Path to the Jinja template file.
        :param template_args: Keyword arguments to pass to the template renderer.
        :raises FileNotFoundError: If the test file does not exist.
        """
        file_path = pathlib.Path(test_file_path)
        if not file_path.exists():
            raise FileNotFoundError(
                f"Python test file not found at {test_file_path}"
            )

        # render the template with the args
        rendered_case_str = self._render_template(template_path, **template_args)

        # turn the string into an expression node
        new_case_node = cst.parse_expression(rendered_case_str.strip())

        # turn the existing test file into a tree
        source_code = file_path.read_text()
        module = cst.parse_module(source_code)

        # insert expression node into tree
        transformer = self.ParametrizeListAppender(
            target_test_function_name, new_case_node
        )
        modified_module = module.visit(transformer)

        file_path.write_text(modified_module.code)
