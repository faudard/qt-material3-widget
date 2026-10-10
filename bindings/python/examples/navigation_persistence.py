"""Persist editable Qt form state and bound speculative data prefetch."""

from PySide6.QtWidgets import (
    QApplication, QCheckBox, QLineEdit, QVBoxLayout, QWidget,
)
from QtMaterial3 import AsyncLazyTabs, NavigationSession, Widgets


def form_page(_payload):
    page = QWidget()
    layout = QVBoxLayout(page)
    username = QLineEdit(page)
    username.setObjectName("username")
    username.setPlaceholderText("Your name")
    layout.addWidget(username)

    secret = QLineEdit(page)
    secret.setObjectName("password")
    secret.setEchoMode(QLineEdit.Password)
    layout.addWidget(secret)

    enabled = QCheckBox("Enable notifications", page)
    enabled.setObjectName("notifications")
    layout.addWidget(enabled)
    return page


def main():
    app = QApplication([])
    tabs = Widgets.QtMaterialTabs()
    pages = AsyncLazyTabs(tabs, cancel_on_leave=False)

    pages.addAsyncTab("Home", lambda _: "home", lambda _: QWidget(), route="/home")
    pages.addAsyncTab("Account", lambda _: None, form_page, route="/account")
    pages.addAsyncTab("Reports", lambda _: "reports", lambda _: QWidget(), route="/reports")

    session = NavigationSession(
        pages, prefetch_radius=1, max_pending=2, max_cached_pages=1,
    )
    session.registerForm("/account")
    session.cacheEvicted.connect(lambda index: print("Evicted prefetched data", index))
    session.stateProviderFailed.connect(lambda route, error: print(route, error))

    # In production, persist session.saveState() to your chosen storage. After
    # rebuilding tabs on startup, call session.restoreState(saved_json).
    tabs.setWindowTitle("QtMaterial3 Navigation Persistence 2.0")
    tabs.resize(720, 460)
    tabs.show()
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
