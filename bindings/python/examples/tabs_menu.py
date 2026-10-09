"""QtMaterial3 Tabs and Menu powered by PySide6 and the native C++ library."""

from PySide6.QtWidgets import QApplication, QHBoxLayout, QLabel, QVBoxLayout, QWidget
from QtMaterial3 import Widgets, TabsVariant


def main():
    app = QApplication([])
    window = QWidget()
    outer = QVBoxLayout(window)

    tabs = Widgets.QtMaterialTabs(window)
    tabs.setVariant(TabsVariant.Secondary)
    for title in ("Home", "Settings", "About"):
        page = QWidget()
        page_layout = QVBoxLayout(page)
        page_layout.addWidget(QLabel(f"{title} page"))
        tabs.addTab(page, title)
    tabs.setTabId(1, "settings")
    tabs.setBadge(1, "2")
    outer.addWidget(tabs)

    bottom = QHBoxLayout()
    menu = Widgets.QtMaterialMenu(window)
    menu.setExpressive(True)
    menu.addItem("Home")
    menu.addItem("Settings")
    menu.addItem("About")
    menu.activated.connect(tabs.setCurrentIndex)
    bottom.addWidget(menu)
    outer.addLayout(bottom)

    window.resize(680, 440)
    window.setWindowTitle("QtMaterial3: PySide6 navigation")
    window.show()
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
