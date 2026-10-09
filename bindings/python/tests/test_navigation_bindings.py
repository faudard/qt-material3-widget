"""PySide6 contract tests for the staged native Tabs and Menu bindings."""

import gc
import os
import unittest

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtWidgets import QApplication, QWidget
from shiboken6 import Shiboken

import QtMaterial3
from QtMaterial3 import Widgets


class NavigationBindingsTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.app = QApplication.instance() or QApplication([])

    def test_tabs_preserve_qtabwidget_and_native_metadata(self):
        tabs = Widgets.QtMaterialTabs()
        first, second = QWidget(), QWidget()
        self.assertEqual(tabs.addTab(first, "Home"), 0)
        self.assertEqual(tabs.addTab(second, "Settings"), 1)
        self.assertEqual(tabs.count(), 2)
        self.assertIs(tabs.widget(0), first)

        indices = []
        tabs.currentChanged.connect(indices.append)
        tabs.setCurrentIndex(1)
        self.assertEqual(indices, [1])

        tabs.setTabId(1, "settings")
        self.assertEqual(tabs.tabId(1), "settings")
        self.assertEqual(tabs.indexOfTabId("settings"), 1)
        tabs.setBadge(1, "3")
        self.assertEqual(tabs.badge(1), "3")
        tabs.setWrapNavigation(False)
        self.assertFalse(tabs.wrapNavigation())

        tabs.setVariant(QtMaterial3.TabsVariant.Secondary)
        self.assertEqual(tabs.variant(), QtMaterial3.TabsVariant.Secondary)
        self.assertEqual(tabs.property("variant"), QtMaterial3.TabsVariant.Secondary)

        Shiboken.delete(tabs)
        self.assertFalse(Shiboken.isValid(first))
        self.assertFalse(Shiboken.isValid(second))

    def test_menu_items_signals_and_properties(self):
        menu = Widgets.QtMaterialMenu()
        self.assertTrue(menu.isEmpty())
        self.assertEqual(menu.addItem("Open"), 0)
        self.assertEqual(menu.addSeparator(), 1)
        self.assertEqual(menu.addItem("Save"), 2)
        self.assertEqual(menu.count(), 3)
        self.assertEqual(menu.itemText(2), "Save")
        self.assertTrue(menu.isSeparator(1))
        menu.setItemCheckable(2, True)
        menu.setItemChecked(2, True)
        self.assertTrue(menu.isItemChecked(2))

        values = []
        menu.expressiveChanged.connect(values.append)
        menu.setExpressive(True)
        self.assertEqual(values, [True])
        self.assertEqual(menu.property("expressive"), True)
        self.assertTrue(menu.expressive())
        menu.clear()
        self.assertTrue(menu.isEmpty())

    def test_navigation_widgets_follow_parent_lifetime(self):
        parent = QWidget()
        tabs = Widgets.QtMaterialTabs(parent)
        menu = Widgets.QtMaterialMenu(parent)
        self.assertIs(tabs.parent(), parent)
        self.assertIs(menu.parent(), parent)
        self.assertTrue(Shiboken.isValid(tabs))
        self.assertTrue(Shiboken.isValid(menu))
        Shiboken.delete(parent)
        self.assertFalse(Shiboken.isValid(tabs))
        self.assertFalse(Shiboken.isValid(menu))
        del tabs, menu, parent
        gc.collect()


if __name__ == "__main__":
    unittest.main(verbosity=2)
