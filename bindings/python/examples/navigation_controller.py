"""Demonstrate a borrowed native stack controller synchronized with Material tabs."""

from PySide6.QtWidgets import QApplication, QLabel, QStackedWidget, QVBoxLayout, QWidget
from QtMaterial3 import QtMaterialStackedWidgetController, Widgets


def main():
    app = QApplication([])
    window = QWidget()
    layout = QVBoxLayout(window)
    tabs = Widgets.QtMaterialTabs(window)
    stack = QStackedWidget(window)

    for title in ("Overview", "Settings", "History"):
        tabs.addTab(QWidget(), title)
        page = QWidget()
        page_layout = QVBoxLayout(page)
        page_layout.addWidget(QLabel("Destination: " + title))
        stack.addWidget(page)

    # The controller does not own stack: both are owned by window.
    # Tabs holds a QPointer to the externally parented controller.
    controller = QtMaterialStackedWidgetController(stack, window)
    tabs.bindToController(controller)

    layout.addWidget(tabs)
    layout.addWidget(stack)
    window.setWindowTitle("QtMaterial3 Navigation Controller — PySide6")
    window.resize(640, 420)
    window.show()
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
