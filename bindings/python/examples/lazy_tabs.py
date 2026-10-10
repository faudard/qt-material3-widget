"""Create Material 3 tab pages only when the user first selects them.

Run from an installed QtMaterial3 wheel with PySide6 6.6.3.
"""
from PySide6.QtWidgets import QApplication, QLabel, QVBoxLayout, QWidget

from QtMaterial3 import LazyTabs, Widgets


def make_page(title: str) -> QWidget:
    page = QWidget()
    layout = QVBoxLayout(page)
    layout.addWidget(QLabel(title))
    return page


def main() -> int:
    app = QApplication([])
    tabs = Widgets.QtMaterialTabs()
    lazy = LazyTabs(tabs)

    lazy.addLazyTab("Overview", lambda: make_page("Overview loaded immediately"),
                    route="/overview")
    lazy.addLazyTab("Settings", lambda: make_page("Settings loaded on first visit"),
                    route="/settings")
    lazy.addLazyTab("History", lambda: make_page("History loaded on first visit"),
                    route="/history")

    # Load exceptions are reported instead of propagating through Qt's event loop.
    lazy.loadFailed.connect(lambda index, error: print("Page", index, error))
    lazy.pageLoaded.connect(lambda index, _: print("Loaded page", index))
    tabs.resize(720, 480)
    tabs.setWindowTitle("QtMaterial3 Python Lazy Tabs")
    tabs.show()
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
