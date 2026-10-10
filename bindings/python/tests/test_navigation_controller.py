"""1.17.6: native QStackedWidget controller ownership and routing contracts.

Keep this as an integration test of the compiled bindings, not a Python
reimplementation of the native controller.
"""
import gc
import os
import unittest

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtCore import QObject
from PySide6.QtWidgets import QApplication, QStackedWidget, QWidget
from shiboken6 import Shiboken

import QtMaterial3
from QtMaterial3 import Widgets


class NavigationControllerContracts(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.app = QApplication.instance() or QApplication([])

    @staticmethod
    def make_stack():
        stack = QStackedWidget()
        stack.addWidget(QWidget())
        stack.addWidget(QWidget())
        return stack

    @staticmethod
    def make_tabs():
        tabs = Widgets.QtMaterialTabs()
        tabs.addTab(QWidget(), "First")
        tabs.addTab(QWidget(), "Second")
        return tabs

    def test_native_abstract_base_and_borrowed_stack(self):
        stack = self.make_stack()
        controller = QtMaterial3.QtMaterialStackedWidgetController(stack)
        self.assertIsInstance(controller, QtMaterial3.QtMaterialNavigationController)
        self.assertIs(controller.stackedWidget(), stack)
        self.assertIsNone(controller.parent())
        indices = []
        controller.currentIndexChanged.connect(indices.append)
        controller.setCurrentIndex(1)
        self.assertEqual((controller.currentIndex(), stack.currentIndex()), (1, 1))
        self.assertEqual(indices, [1])
        Shiboken.delete(controller)
        self.assertFalse(Shiboken.isValid(controller))
        self.assertTrue(Shiboken.isValid(stack))
        Shiboken.delete(stack)

    def test_tabs_controller_bidirectional_sync_and_detach(self):
        stack = self.make_stack()
        tabs = self.make_tabs()
        controller = QtMaterial3.QtMaterialStackedWidgetController(stack)
        bindings = []
        tabs.controllerBindingChanged.connect(
            lambda bound_controller, bound: bindings.append((bound_controller, bound))
        )
        tabs.bindToController(controller)
        self.assertEqual(len(bindings), 1)
        self.assertIs(bindings[0][0], controller)
        self.assertTrue(bindings[0][1])

        controller.setCurrentIndex(1)
        self.assertEqual(tabs.currentIndex(), 1)
        tabs.setCurrentIndex(0)
        self.assertEqual(stack.currentIndex(), 0)

        tabs.unbindController(controller)
        self.assertEqual(len(bindings), 2)
        self.assertIs(bindings[1][0], controller)
        self.assertFalse(bindings[1][1])
        controller.setCurrentIndex(1)
        self.assertEqual(tabs.currentIndex(), 0)
        tabs.setCurrentIndex(1)
        self.assertEqual(controller.currentIndex(), 1)
        Shiboken.delete(tabs)
        self.assertTrue(Shiboken.isValid(controller))
        Shiboken.delete(controller)
        Shiboken.delete(stack)

    def test_stack_destruction_nulls_borrowed_pointer(self):
        stack = self.make_stack()
        controller = QtMaterial3.QtMaterialStackedWidgetController(stack)
        Shiboken.delete(stack)
        self.assertFalse(Shiboken.isValid(stack))
        self.assertTrue(Shiboken.isValid(controller))
        self.assertIsNone(controller.stackedWidget())
        self.assertEqual(controller.currentIndex(), -1)
        controller.setCurrentIndex(0)
        self.assertEqual(controller.currentIndex(), -1)
        Shiboken.delete(controller)

    def test_external_controller_parent_destroyed_while_bound(self):
        parent = QObject()
        stack = self.make_stack()
        controller = QtMaterial3.QtMaterialStackedWidgetController(stack, parent)
        tabs = self.make_tabs()
        tabs.bindToController(controller)
        self.assertIs(controller.parent(), parent)
        Shiboken.delete(parent)
        self.assertFalse(Shiboken.isValid(controller))
        self.assertTrue(Shiboken.isValid(stack))
        self.assertTrue(Shiboken.isValid(tabs))
        tabs.setCurrentIndex(1)
        tabs.unbindAll()
        Shiboken.delete(tabs)
        Shiboken.delete(stack)

    def test_repeated_bind_unbind_never_transfers_ownership(self):
        for _ in range(10):
            stack = self.make_stack()
            tabs = self.make_tabs()
            controller = QtMaterial3.QtMaterialStackedWidgetController(stack)
            for _ in range(3):
                tabs.bindToController(controller)
                tabs.setCurrentIndex(1)
                self.assertEqual(stack.currentIndex(), 1)
                tabs.unbindController(controller)
                tabs.setCurrentIndex(0)
                self.assertEqual(stack.currentIndex(), 1)
            Shiboken.delete(tabs)
            self.assertTrue(Shiboken.isValid(controller))
            self.assertTrue(Shiboken.isValid(stack))
            Shiboken.delete(controller)
            Shiboken.delete(stack)
            gc.collect()


if __name__ == "__main__":
    unittest.main(verbosity=2)
