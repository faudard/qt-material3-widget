import gc
import os
import unittest

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtGui import QColor
from PySide6.QtWidgets import QApplication, QWidget
from shiboken6 import Shiboken

import QtMaterial3
from QtMaterial3 import Widgets


class QtMaterial3BindingsTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.app = QApplication.instance() or QApplication([])

    def test_theme_builder_expressive_round_trip(self):
        options = QtMaterial3.ThemeOptions()
        options.sourceColor = QColor("#6750A4")
        options.variant = QtMaterial3.ThemeVariant.Expressive
        options.motionScheme = QtMaterial3.MotionScheme.Expressive
        theme = QtMaterial3.ThemeBuilder().build(options)
        self.assertEqual(theme.motionScheme(), QtMaterial3.MotionScheme.Expressive)
        self.assertEqual(theme.options().variant, QtMaterial3.ThemeVariant.Expressive)

    def test_theme_context_cpp_signal_reaches_python(self):
        context = QtMaterial3.ThemeContext()
        revisions = []
        context.revisionChanged.connect(revisions.append)
        options = QtMaterial3.ThemeOptions()
        options.sourceColor = QColor("#00639B")
        changed = QtMaterial3.ThemeBuilder().build(options)
        self.assertTrue(context.setTheme(changed))
        self.assertEqual(context.revision(), 1)
        self.assertEqual(revisions, [1])

    def test_qproperty_and_signal_surface(self):
        card = Widgets.QtMaterialCard()
        titles = []
        card.titleTextChanged.connect(titles.append)
        self.assertTrue(card.setProperty("titleText", "Bound from Python"))
        self.assertEqual(card.property("titleText"), "Bound from Python")
        self.assertEqual(titles, ["Bound from Python"])

    def test_adaptive_and_expressive_enums(self):
        size_class = QtMaterial3.WindowSizeClass.fromLogicalSize(520, 700)
        self.assertEqual(size_class.width, QtMaterial3.WindowWidthSizeClass.Compact)
        self.assertEqual(size_class.height, QtMaterial3.WindowHeightSizeClass.Medium)
        self.assertEqual(QtMaterial3.MotionScheme.Expressive.name, "Expressive")

    def test_qobject_parent_ownership_invalidates_python_wrapper(self):
        parent = QWidget()
        child = Widgets.QtMaterialFilledButton(parent)
        self.assertIs(child.parent(), parent)
        self.assertTrue(Shiboken.isValid(child))
        Shiboken.delete(parent)
        self.assertFalse(Shiboken.isValid(child))
        del parent
        gc.collect()
        self.assertFalse(Shiboken.isValid(child))
        del child
        gc.collect()


if __name__ == "__main__":
    unittest.main(verbosity=2)
