"""1.28 sample: native navigation + Table/Tree models + dialog + live theme.

Run from the installed wheel with Qt 6 / PySide6 6.6.3. The Qt-owned model
objects remain application-owned; QtMaterial never reconstructs the data.
"""
from PySide6.QtCore import Qt
from PySide6.QtGui import QColor, QStandardItem, QStandardItemModel
from PySide6.QtWidgets import (
    QApplication, QHBoxLayout, QLabel, QPushButton, QStackedWidget,
    QVBoxLayout, QWidget,
)

from QtMaterial3 import ThemeBuilder, ThemeContext, ThemeOptions, Widgets


def main() -> int:
    app = QApplication([])
    window = QWidget()
    window.setWindowTitle("QtMaterial3 / Python Desktop SDK 2.0")
    layout = QVBoxLayout(window)

    crumb = Widgets.QtMaterialBreadcrumb(window)
    crumb.setItems(["Workspace", "Overview"])
    layout.addWidget(crumb)

    body = QHBoxLayout()
    layout.addLayout(body)
    navigation = Widgets.QtMaterialNavigationSuite(window)
    navigation.addDestination("Overview")
    navigation.addDestination("Settings")
    body.addWidget(navigation)

    stack = QStackedWidget(window)
    body.addWidget(stack, 1)
    overview = QWidget()
    overview_layout = QVBoxLayout(overview)
    table = Widgets.QtMaterialTable(overview)
    table.setDense(True)
    task_model = QStandardItemModel(overview)
    task_model.setHorizontalHeaderLabels(["Task", "Owner"])
    for title, owner in [("Qualification", "QA"), ("Wheel packaging", "SDK")]:
        task_model.appendRow([QStandardItem(title), QStandardItem(owner)])
    table.setModel(task_model)
    overview_layout.addWidget(table)

    tree = Widgets.QtMaterialTreeView(overview)
    project_model = QStandardItemModel(overview)
    project_model.setHorizontalHeaderLabels(["Projects"])
    root = QStandardItem("SDK 2.0")
    root.appendRow(QStandardItem("Desktop example"))
    project_model.appendRow(root)
    tree.setModel(project_model)
    tree.expandAll()
    overview_layout.addWidget(tree)
    stack.addWidget(overview)

    settings = QWidget()
    settings_layout = QVBoxLayout(settings)
    settings_layout.addWidget(QLabel("Theme source color"))
    theme_context = ThemeContext(window)
    table.setThemeContext(theme_context)
    options = ThemeOptions()
    colors = [QColor("#6750A4"), QColor("#006A60")]
    index = [0]

    def switch_theme() -> None:
        index[0] = 1 - index[0]
        options.sourceColor = colors[index[0]]
        theme_context.setTheme(ThemeBuilder().build(options))

    theme_button = QPushButton("Switch theme")
    theme_button.clicked.connect(switch_theme)
    settings_layout.addWidget(theme_button)

    dialog = Widgets.QtMaterialDialog(window)
    dialog.setTitleText("SDK 2.0")
    dialog.setSupportingText("The dialog is the native C++ Material widget.")
    open_dialog = QPushButton("Show dialog")
    open_dialog.clicked.connect(dialog.open)
    settings_layout.addWidget(open_dialog)
    settings_layout.addStretch(1)
    stack.addWidget(settings)

    def navigate(index: int) -> None:
        stack.setCurrentIndex(index)
        crumb.setItems(["Workspace", "Overview" if index == 0 else "Settings"])

    navigation.currentIndexChanged.connect(navigate)
    window.resize(1000, 680)
    window.show()
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
