"""Material 3 Python navigation session: history, prefetch and restore.

The example uses plain-data workers and GUI-thread QWidget renderers.
"""
import json

from PySide6.QtWidgets import QApplication, QLabel
from QtMaterial3 import AsyncLazyTabs, NavigationSession, Widgets


def main() -> int:
    app = QApplication([])
    tabs = Widgets.QtMaterialTabs()
    pages = AsyncLazyTabs(tabs, max_workers=2, cancel_on_leave=False)

    for path, title in [
        ("/home", "Home"),
        ("/reports", "Reports"),
        ("/settings", "Settings"),
    ]:
        pages.addAsyncTab(
            title,
            lambda _cancel, text=title: {"title": text},
            lambda data: QLabel("Loaded page: " + data["title"]),
            route=path,
        )

    session = NavigationSession(pages, prefetch_radius=1, max_pending=2)
    session.setPageState("/settings", {"tab": "profile", "scroll": 0})
    session.navigate("/reports")
    session.navigate("/settings")
    session.back()  # /reports

    snapshot = json.dumps(session.saveState(), ensure_ascii=False)
    print("Serializable session:", snapshot)

    # A later app launch can reconstruct the tabs before restoring the state.
    session.restoreState(snapshot)

    session.routeChanged.connect(lambda route: print("Navigated:", route))
    tabs.setWindowTitle("QtMaterial3 Python Navigation 3.0")
    tabs.resize(720, 480)
    tabs.show()
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
