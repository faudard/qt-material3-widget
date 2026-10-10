"""Async Material tabs: load data in a thread, construct widgets on the GUI thread."""

from PySide6.QtWidgets import QApplication, QLabel, QVBoxLayout, QWidget
from QtMaterial3 import AsyncLazyTabs, Widgets


def load_report(cancel):
    # Real applications can read local files, query services, or compute data.
    # Never create QWidgets or touch Qt objects inside this worker callback.
    for _ in range(20):
        if cancel.wait(0.01):
            return None
    return {"heading": "Report", "text": "Loaded without blocking the GUI"}


def render_report(payload):
    # This callback runs on the Qt GUI thread.
    page = QWidget()
    layout = QVBoxLayout(page)
    layout.addWidget(QLabel(payload["heading"]))
    layout.addWidget(QLabel(payload["text"]))
    return page


def main():
    app = QApplication([])
    tabs = Widgets.QtMaterialTabs()
    async_tabs = AsyncLazyTabs(tabs, max_workers=2, cancel_on_leave=True)
    async_tabs.addAsyncTab("Overview", lambda _: {"heading": "Overview", "text": "Ready"},
                           render_report, route="/overview")
    async_tabs.addAsyncTab("Reports", load_report, render_report, route="/reports")
    async_tabs.pageReady.connect(lambda index, _: print("Page ready:", index))
    async_tabs.loadFailed.connect(lambda index, error: print("Load failed:", index, error))
    async_tabs.pageCancelled.connect(lambda index: print("Cancelled:", index))
    tabs.resize(720, 460)
    tabs.setWindowTitle("QtMaterial3 Async Lazy Tabs")
    tabs.show()
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
