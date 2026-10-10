"""1.17.5 native imports, properties, signals, borrowed pointers and QObject lifetimes."""

import gc
import os
import unittest

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtCore import Qt
from PySide6.QtWidgets import QApplication, QPushButton, QWidget
from shiboken6 import Shiboken

import QtMaterial3
from QtMaterial3 import Widgets


CATALOGUE = (
    "QtMaterialDialog",
    "QtMaterialSnackbar",
    "QtMaterialBadge",
    "QtMaterialPagination",
    "QtMaterialDivider",
)


class PythonCatalogue2Contracts(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.app = QApplication.instance() or QApplication([])

    def test_imports_and_qobject_inheritance(self):
        for name in CATALOGUE:
            with self.subTest(widget=name):
                widget_type = getattr(Widgets, name)
                self.assertIs(widget_type, getattr(QtMaterial3.QtMaterial, name))
                widget = widget_type()
                self.assertIsInstance(widget, QWidget)
                self.assertTrue(Shiboken.isValid(widget))
                Shiboken.delete(widget)
                self.assertFalse(Shiboken.isValid(widget))

    def test_dialog_properties_native_signals_and_borrowed_pointers(self):
        parent = QWidget()
        dialog = Widgets.QtMaterialDialog(parent)
        target = QPushButton("Accept", parent)
        dialog.setTitleText("Confirm")
        dialog.setSupportingText("Keep changes?")
        dialog.setDismissOnEscape(False)
        dialog.setRestoreFocusOnClose(False)
        self.assertEqual(dialog.titleText(), "Confirm")
        self.assertEqual(dialog.supportingText(), "Keep changes?")
        self.assertFalse(dialog.dismissOnEscape())
        self.assertFalse(dialog.restoreFocusOnClose())
        self.assertIn("Confirm", dialog.accessibilitySummary())

        dialog.setInitialFocusWidget(target)
        dialog.setDefaultButton(target)
        self.assertIs(dialog.initialFocusWidget(), target)
        self.assertIs(dialog.defaultButton(), target)

        opened, closed, rejected = [], [], []
        dialog.opened.connect(lambda: opened.append(True))
        dialog.closed.connect(lambda: closed.append(True))
        dialog.rejected.connect(lambda: rejected.append(True))
        dialog.open()
        dialog.reject()
        self.assertEqual((len(opened), len(closed), len(rejected)), (1, 1, 1))

        Shiboken.delete(target)
        self.assertIsNone(dialog.initialFocusWidget())
        self.assertIsNone(dialog.defaultButton())
        self.assertTrue(Shiboken.isValid(dialog))
        Shiboken.delete(parent)
        self.assertFalse(Shiboken.isValid(dialog))

    def test_snackbar_properties_cpp_action_signal_and_enums(self):
        snackbar = Widgets.QtMaterialSnackbar()
        snackbar.setText("Saved")
        snackbar.setActionText("Undo")
        snackbar.setDuration(QtMaterial3.SnackbarDuration.Indefinite)
        snackbar.setShowDismissButton(True)
        snackbar.setPauseAutoHideOnInteraction(False)
        self.assertEqual(snackbar.text(), "Saved")
        self.assertEqual(snackbar.actionText(), "Undo")
        self.assertEqual(snackbar.duration(), QtMaterial3.SnackbarDuration.Indefinite)
        self.assertTrue(snackbar.showDismissButton())
        self.assertFalse(snackbar.property("pauseAutoHideOnInteraction"))
        self.assertIn("Saved", snackbar.accessibilitySummary())

        actions = []
        snackbar.actionTriggered.connect(lambda: actions.append(True))
        action_button = next(
            button for button in snackbar.findChildren(QPushButton)
            if button.text() == "Undo"
        )
        action_button.click()
        self.assertEqual(actions, [True])
        self.assertTrue(Shiboken.isValid(snackbar))
        Shiboken.delete(snackbar)

    def test_badge_properties_signals_and_normalization(self):
        badge = Widgets.QtMaterialBadge()
        counts, maxima, dots = [], [], []
        badge.countChanged.connect(counts.append)
        badge.maximumChanged.connect(maxima.append)
        badge.dotChanged.connect(dots.append)
        self.assertTrue(badge.setProperty("count", 101))
        badge.setMaximum(99)
        badge.setDot(True)
        self.assertEqual(counts, [101])
        self.assertEqual(maxima, [99])
        self.assertEqual(dots, [True])
        self.assertEqual(badge.property("count"), 101)
        self.assertEqual(badge.displayText(), "")
        badge.setDot(False)
        self.assertEqual(badge.displayText(), "99+")
        badge.setCount(-1)
        self.assertEqual(badge.count(), 0)
        Shiboken.delete(badge)

    def test_pagination_properties_signals_and_borrowed_context(self):
        pagination = Widgets.QtMaterialPagination()
        totals, sizes, pages = [], [], []
        pagination.totalCountChanged.connect(totals.append)
        pagination.pageSizeChanged.connect(sizes.append)
        pagination.pageChanged.connect(pages.append)
        self.assertEqual(pagination.pageSize(), 25)
        self.assertTrue(pagination.setProperty("totalCount", 100))
        # Idempotent setters do not emit: 25 is the native default.
        pagination.setPageSize(25)
        self.assertEqual(sizes, [])
        pagination.setPageSize(10)
        pagination.setPage(3)
        self.assertEqual(totals, [100])
        self.assertEqual(sizes, [10])
        self.assertEqual(pages, [3])
        self.assertEqual(pagination.pageCount(), 10)
        self.assertEqual(pagination.property("page"), 3)
        self.assertEqual(pagination.rangeText(), "21–30 / 100")
        self.assertIn(10, pagination.pageSizeOptions())

        context = QtMaterial3.ThemeContext()
        pagination.setThemeContext(context)
        self.assertIs(pagination.themeContext(), context)
        Shiboken.delete(pagination)
        self.assertTrue(Shiboken.isValid(context))
        Shiboken.delete(context)

    def test_divider_properties_and_native_signals(self):
        divider = Widgets.QtMaterialDivider()
        orientations, thicknesses, labels = [], [], []
        divider.orientationChanged.connect(orientations.append)
        divider.thicknessChanged.connect(thicknesses.append)
        divider.accessibilityLabelChanged.connect(labels.append)
        divider.setOrientation(Qt.Vertical)
        self.assertTrue(divider.setProperty("thickness", 3))
        divider.setAccessibilityLabel("Section")
        self.assertEqual(orientations, [Qt.Vertical])
        self.assertEqual(thicknesses, [3])
        self.assertEqual(labels, ["Section"])
        self.assertEqual(divider.property("thickness"), 3)
        self.assertEqual(divider.orientation(), Qt.Vertical)
        Shiboken.delete(divider)

    def test_qt_parent_deletion_invalidates_every_wrapper(self):
        for name in CATALOGUE:
            with self.subTest(widget=name):
                parent = QWidget()
                child = getattr(Widgets, name)(parent)
                self.assertIs(child.parent(), parent)
                self.assertTrue(Shiboken.isValid(child))
                Shiboken.delete(parent)
                self.assertFalse(Shiboken.isValid(child))
                del child, parent
                gc.collect()

    def test_unparented_wrapper_destruction_repeated(self):
        for name in CATALOGUE:
            with self.subTest(widget=name):
                for _ in range(5):
                    widget = getattr(Widgets, name)()
                    self.assertTrue(Shiboken.isValid(widget))
                    Shiboken.delete(widget)
                    self.assertFalse(Shiboken.isValid(widget))
                    del widget
                    gc.collect()


if __name__ == "__main__":
    unittest.main(verbosity=2)
