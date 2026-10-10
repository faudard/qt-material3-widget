"""Small QtMaterial3 1.17.5 PySide6 form (Qt 6, no callback-based factories)."""

from PySide6.QtCore import Qt
from PySide6.QtWidgets import QApplication, QVBoxLayout, QWidget

from QtMaterial3 import SnackbarDuration, Widgets


def main():
    app = QApplication([])
    window = QWidget()
    layout = QVBoxLayout(window)

    badge = Widgets.QtMaterialBadge(window)
    badge.setCount(12)
    layout.addWidget(badge)

    divider = Widgets.QtMaterialDivider(window)
    divider.setOrientation(Qt.Horizontal)
    layout.addWidget(divider)

    pagination = Widgets.QtMaterialPagination(window)
    pagination.setTotalCount(150)
    pagination.setPageSize(25)
    layout.addWidget(pagination)

    snackbar = Widgets.QtMaterialSnackbar(window)
    snackbar.setText("Page ready")
    snackbar.setDuration(SnackbarDuration.Indefinite)
    snackbar.setActionText("Dismiss")
    snackbar.actionTriggered.connect(lambda: snackbar.dismiss())

    dialog = Widgets.QtMaterialDialog(window)
    dialog.setTitleText("PySide6 Catalogue 2.0")
    dialog.setSupportingText("Dialog, Snackbar, Badge, Pagination and Divider")
    dialog.rejected.connect(lambda: print("Dialog rejected"))

    window.setWindowTitle("QtMaterial3 / Python Catalogue 2.0")
    window.resize(560, 260)
    window.show()
    snackbar.showSnackbar()
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
