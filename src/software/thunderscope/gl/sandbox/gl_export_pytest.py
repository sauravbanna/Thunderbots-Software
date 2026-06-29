import os
from pyqtgraph.Qt.QtWidgets import *
from software.thunderscope.gl.sandbox.test_gen.test_editor import TestEditor


class GLExportPytest(QWidget):
    """A widget for managing pytest test export.

    Displays the current test file and current test names,
    and provides buttons for adding test cases, tests, and creating test files.
    """

    ADD_NEW_TEST_TEXT = "Add New Test"

    def __init__(self):
        super().__init__()
        self.setLayout(QVBoxLayout())

        self._has_test_file = False
        self._has_test_name = False
        self._is_new_file = False

        # Current Test File row
        test_file_row = QHBoxLayout()
        test_file_row.addWidget(QLabel("Current Test File:"))

        self.current_test_file_label = QLabel(self.ADD_NEW_TEST_TEXT)
        self.current_test_file_label.setStyleSheet("color: #aaa;")
        test_file_row.addWidget(self.current_test_file_label)

        self._current_test_file_path = None

        self.browse_button = QPushButton("...")
        self.browse_button.setFixedWidth(30)
        self.browse_button.clicked.connect(self._set_test_file)
        test_file_row.addWidget(self.browse_button)

        test_file_row.addStretch()
        self.layout().addLayout(test_file_row)

        # Current Test row
        test_row = QHBoxLayout()
        test_row.addWidget(QLabel("Current Test:"))

        self.current_test_combo = QComboBox()
        self.current_test_combo.currentIndexChanged.connect(
            self._on_test_combo_changed
        )
        self._initialize_test_name_dropdown()
        test_row.addWidget(self.current_test_combo)
        test_row.addStretch()
        self.layout().addLayout(test_row)

        # New test name row
        new_test_row = QHBoxLayout()
        new_test_row.addStretch()
        self.new_test_name_edit = QLineEdit()
        self.new_test_name_edit.setPlaceholderText("Enter new test name...")
        self.new_test_name_edit.setEnabled(False)
        self.new_test_name_edit.textChanged.connect(self._on_new_test_name_changed)
        new_test_row.addWidget(self.new_test_name_edit)
        self.layout().addLayout(new_test_row)

        # test modification buttons
        self.create_test_file_button = QPushButton("Copy Test Case")
        self.layout().addWidget(self.create_test_file_button)

        self.add_test_case_button = QPushButton("Add Test Case")
        self.add_test_case_button.setEnabled(False)
        self.add_test_case_button.clicked.connect(self._on_add_test_case)
        self.layout().addWidget(self.add_test_case_button)

    def _on_test_combo_changed(self, index: int) -> None:
        """Enable the new test name field only when 'Add New Test' is selected.

        :param index: the newly selected index
        """
        is_add_new = self.current_test_combo.currentText() == self.ADD_NEW_TEST_TEXT
        self.new_test_name_edit.setEnabled(is_add_new)

        self._has_test_name = not is_add_new
        self._update_add_test_case_state()

    def _on_new_test_name_changed(self, text: str) -> None:
        """Update state when the new test name text changes.

        :param text: the current text in the field
        """
        self._has_test_name = bool(text.strip())
        self._update_add_test_case_state()

    def _on_add_test_case(self) -> None:
        """Handle the Add Test Case button click."""
        test_name = self._get_active_test_name()
        if not test_name or not self._current_test_file_path:
            return

        editor = TestEditor(
            test_file_path=self._current_test_file_path,
            test_name=test_name,
            is_new_case=self._is_new_file,
        )
        editor.execute()

    def _get_active_test_name(self) -> str | None:
        """Get the currently active test name, either from the dropdown
        selection or the new test name text field.

        :return: the test name, or None if neither is available
        """
        if self.current_test_combo.currentText() != self.ADD_NEW_TEST_TEXT:
            return self.current_test_combo.currentText()
        text = self.new_test_name_edit.text().strip()
        return text if text else None

    def _update_add_test_case_state(self) -> None:
        """Enable or disable 'Add Test Case' based on whether both
        a test file and a test name are available.
        """
        self.add_test_case_button.setEnabled(
            self._has_test_file and self._has_test_name
        )

    def _set_test_file(self) -> None:
        """Open a file dialog to select or create a test file and update the label."""
        file_path = self._get_file_from_dialog()
        if not file_path:
            return

        self._current_test_file_path = file_path
        self._has_test_file = True
        self._is_new_file = not os.path.exists(file_path)
        self.current_test_file_label.setText(os.path.basename(file_path))
        self.current_test_file_label.setStyleSheet("color: #fff;")
        if os.path.exists(file_path):
            self._populate_test_dropdown(file_path)
        else:
            self._initialize_test_name_dropdown()
        self._update_add_test_case_state()

    def _initialize_test_name_dropdown(self) -> None:
        """Reset the test name dropdown to its default state."""
        self.current_test_combo.clear()
        self.current_test_combo.addItem(self.ADD_NEW_TEST_TEXT)
        self.current_test_combo.setStyleSheet("color: #aaa;")
        self.current_test_combo.setCurrentIndex(0)

    def _populate_test_dropdown(self, file_path: str) -> None:
        """Fetch test method names from the given file and populate the dropdown.

        :param file_path: path to the test file
        """
        self.current_test_combo.clear()
        self.current_test_combo.addItem(self.ADD_NEW_TEST_TEXT)
        try:
            test_names = TestEditor.get_test_method_names(file_path)
            if test_names:
                self.current_test_combo.addItems(test_names)
                self.current_test_combo.setStyleSheet("color: #fff;")
            else:
                self.current_test_combo.setStyleSheet("color: #aaa;")
        except FileNotFoundError:
            self.current_test_combo.setStyleSheet("color: #aaa;")

        self.current_test_combo.setCurrentIndex(0)

    def _get_file_from_dialog(self) -> str | None:
        """Open a QFileDialog for picking or creating a test file.

        :return: the selected file path, or None if cancelled
        """
        file_path, _ = QFileDialog.getSaveFileName(
            self,
            "Select or Create Test File",
            "",
            "Python Files (*.py);;All Files (*)",
        )
        return file_path if file_path else None
