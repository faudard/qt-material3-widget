"""Stress Qt/Python ownership boundaries at the installed-wheel ABI.

These contracts deliberately use real C++ QObject destruction and the
Shiboken validity bit, not Python mock objects.
"""
from __future__ import annotations

import gc
import os
import unittest

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtWidgets import QApplication, QWidget
from shiboken6 import Shiboken

import QtMaterial3
from QtMaterial3 import Widgets


class ReliabilityContracts(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.app = QApplication.instance() or QApplication([])

    def test_parent_deletion_invalidates_wrappers_over_repeated_cycles(self):
        for iteration in range(25):
            with self.subTest(iteration=iteration):
                parent = QWidget()
                button = Widgets.QtMaterialFilledButton(parent)
                chip = Widgets.QtMaterialChip(parent)
                tabs = Widgets.QtMaterialTabs(parent)
                page = QWidget()
                tabs.addTab(page, "Owned")
                self.assertTrue(all(Shiboken.isValid(obj)
                                    for obj in (button, chip, tabs, page)))
                Shiboken.delete(parent)
                for obj in (button, chip, tabs, page):
                    self.assertFalse(Shiboken.isValid(obj))
                with self.assertRaises(RuntimeError):
                    button.objectName()
                del parent, button, chip, tabs, page
                gc.collect()

    def test_unparented_widget_survives_old_parent(self):
        parent = QWidget()
        child = Widgets.QtMaterialFilledButton(parent)
        child.setParent(None)
        self.assertIsNone(child.parent())
        Shiboken.delete(parent)
        self.assertTrue(Shiboken.isValid(child))
        child.setText("independent")
        self.assertEqual(child.text(), "independent")
        Shiboken.delete(child)
        self.assertFalse(Shiboken.isValid(child))
        gc.collect()

    def test_borrowed_tooltip_target_never_becomes_tooltip_owned(self):
        parent = QWidget()
        target = QWidget(parent)
        tooltip = Widgets.QtMaterialTooltip()
        tooltip.setTargetWidget(target)
        self.assertIs(tooltip.targetWidget(), target)
        Shiboken.delete(parent)
        self.assertFalse(Shiboken.isValid(target))
        self.assertIsNone(tooltip.targetWidget())
        self.assertTrue(Shiboken.isValid(tooltip))
        Shiboken.delete(tooltip)
        gc.collect()

    def test_tabs_borrowed_navigation_model_follows_external_parent(self):
        model_parent = QWidget()
        model = QtMaterial3.QtMaterialNavigationModel(model_parent)
        tabs = Widgets.QtMaterialTabs()
        tabs.setNavigationModel(model)
        self.assertIs(tabs.navigationModel(), model)
        Shiboken.delete(model_parent)
        self.assertFalse(Shiboken.isValid(model))
        self.assertIsNone(tabs.navigationModel())
        self.assertTrue(Shiboken.isValid(tabs))
        Shiboken.delete(tabs)
        gc.collect()

    def test_detaching_model_does_not_destroy_it(self):
        tabs = Widgets.QtMaterialTabs()
        model = QtMaterial3.QtMaterialNavigationModel()
        for _ in range(15):
            tabs.setNavigationModel(model)
            self.assertIs(tabs.navigationModel(), model)
            tabs.setNavigationModel(None)
            self.assertIsNone(tabs.navigationModel())
            self.assertTrue(Shiboken.isValid(model))
        Shiboken.delete(tabs)
        self.assertTrue(Shiboken.isValid(model))
        Shiboken.delete(model)
        gc.collect()


if __name__ == "__main__":
    unittest.main(verbosity=2)
