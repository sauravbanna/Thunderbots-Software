from software.thunderscope.common.common_widgets import StyledButton


class GLExportPytest(StyledButton):
    """A button that exports a pytest test case"""

    def __init__(self):
        super().__init__()
        self.setText("Create new Test")
