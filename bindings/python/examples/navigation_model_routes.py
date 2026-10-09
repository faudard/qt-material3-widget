"""Use the native Material 3 route and navigation model from PySide6."""

from PySide6.QtWidgets import QApplication, QLabel, QVBoxLayout, QWidget
from QtMaterial3 import QtMaterialRoute, QtMaterialNavigationItem, QtMaterialNavigationModel, Widgets


def destination(identifier, route, title):
    item = QtMaterialNavigationItem()
    item.id = identifier
    item.route = QtMaterialRoute(route).path()
    item.label = title
    return item


def main():
    app = QApplication([])
    window = QWidget()
    layout = QVBoxLayout(window)
    tabs = Widgets.QtMaterialTabs(window)

    for identifier, path, title in [
        ("home", "/home", "Home"),
        ("settings", "/settings", "Settings"),
    ]:
        page = QWidget()
        page_layout = QVBoxLayout(page)
        page_layout.addWidget(QLabel(title))
        row = tabs.addTab(page, title)
        tabs.setTabId(row, identifier)
        tabs.setRoute(row, QtMaterialRoute(path))

    # The model is borrowed by tabs. Keep it alive with a Qt parent.
    model = QtMaterialNavigationModel(window)
    model.addItem(destination("home", "/home", "Home"))
    model.addItem(destination("settings", "/settings", "Settings"))
    tabs.setNavigationModel(model)
    model.setSelectedRoute("/settings")

    layout.addWidget(tabs)
    window.setWindowTitle("QtMaterial3 PySide6 Routes + Navigation Model")
    window.resize(640, 380)
    window.show()
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
