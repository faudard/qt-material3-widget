"""1.28 Python SDK 2.0: native desktop bindings and ownership contracts."""
from __future__ import annotations

import gc
import os
import unittest

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtCore import QByteArray
from PySide6.QtGui import QStandardItem, QStandardItemModel
from PySide6.QtWidgets import QApplication, QTableView, QTreeView, QWidget
from shiboken6 import Shiboken

from QtMaterial3 import (NavigationSuiteType, ThemeContext,
                         WindowWidthSizeClass, Widgets,
                         restore_workspace_state, save_workspace_state)


class DesktopSdk2Contracts(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.app = QApplication.instance() or QApplication([])

    def test_native_widget_types_and_qt_parent_lifetime(self):
        classes = (
            ("QtMaterialTable", QTableView),
            ("QtMaterialTreeView", QTreeView),
            ("QtMaterialBreadcrumb", QWidget),
            ("QtMaterialNavigationSuite", QWidget),
        )
        for name, qt_type in classes:
            with self.subTest(name=name):
                parent = QWidget()
                widget = getattr(Widgets, name)(parent)
                self.assertIsInstance(widget, qt_type)
                self.assertTrue(Shiboken.isValid(widget))
                Shiboken.delete(parent)
                self.assertFalse(Shiboken.isValid(widget))

    def test_table_external_model_proxy_compatibility_and_state(self):
        owner = QWidget()
        model = QStandardItemModel(2, 2, owner)
        model.setHorizontalHeaderLabels(["Task", "Owner"])
        model.setItem(0, 0, QStandardItem("Release"))
        model.setItem(1, 0, QStandardItem("Review"))
        view = Widgets.QtMaterialTable()
        view.setModel(model)
        self.assertIs(view.model(), model)
        self.assertIs(model.parent(), owner)
        view.setDense(True)
        view.setMultiSelectionEnabled(True)
        view.setColumnReorderingEnabled(True)
        self.assertTrue(view.dense())
        snapshot = save_workspace_state(view)
        self.assertTrue(snapshot)
        view.setDense(False)
        self.assertTrue(restore_workspace_state(view, snapshot))
        self.assertTrue(view.dense())
        self.assertFalse(restore_workspace_state(view, b"invalid-workspace"))
        self.assertTrue(view.dense())
        with self.assertRaises(TypeError):
            restore_workspace_state(view, "wrong type")  # type: ignore[arg-type]
        Shiboken.delete(view)
        self.assertTrue(Shiboken.isValid(model))
        Shiboken.delete(owner)

    def test_tree_external_model_and_state_roundtrip(self):
        owner = QWidget()
        model = QStandardItemModel(owner)
        root = QStandardItem("Documents")
        root.appendRow(QStandardItem("Reports"))
        model.appendRow(root)
        view = Widgets.QtMaterialTreeView()
        view.setModel(model)
        view.expand(model.index(0, 0))
        view.setDense(True)
        view.setInlineEditingEnabled(True)
        snapshot = view.saveWorkspaceState()
        self.assertIsInstance(snapshot, QByteArray)
        view.setDense(False)
        self.assertTrue(view.restoreWorkspaceState(snapshot))
        self.assertTrue(view.dense())
        self.assertTrue(view.isExpanded(model.index(0, 0)))
        self.assertFalse(view.restoreWorkspaceState(QByteArray(b"wrong")))
        Shiboken.delete(view)
        self.assertTrue(Shiboken.isValid(model))
        Shiboken.delete(owner)

    def test_navigation_and_breadcrumb_workflows(self):
        suite = Widgets.QtMaterialNavigationSuite()
        suite.addDestination("Overview")
        suite.addDestination("Settings")
        suite.setCurrentIndex(1)
        suite.setWindowWidthSizeClass(WindowWidthSizeClass.Compact)
        self.assertEqual(suite.count(), 2)
        self.assertEqual(suite.currentIndex(), 1)
        self.assertEqual(suite.navigationType(), NavigationSuiteType.NavigationBar)
        snapshot = save_workspace_state(suite)
        suite.setCurrentIndex(0)
        self.assertTrue(restore_workspace_state(suite, snapshot))
        self.assertEqual(suite.currentIndex(), 1)
        self.assertFalse(restore_workspace_state(suite, b"wrong"))
        Shiboken.delete(suite)

        breadcrumb = Widgets.QtMaterialBreadcrumb()
        breadcrumb.setItems(["Workspace", "Settings"])
        breadcrumb.setCurrentIndex(1)
        self.assertEqual(breadcrumb.items(), ["Workspace", "Settings"])
        self.assertEqual(breadcrumb.currentIndex(), 1)
        Shiboken.delete(breadcrumb)

    def test_borrowed_context_and_repeated_model_view_lifetime(self):
        context = ThemeContext()
        for _ in range(15):
            parent = QWidget()
            model = QStandardItemModel(parent)
            model.appendRow(QStandardItem("Row"))
            table = Widgets.QtMaterialTable()
            table.setThemeContext(context)
            table.setModel(model)
            self.assertIs(table.themeContext(), context)
            Shiboken.delete(table)
            self.assertTrue(Shiboken.isValid(model))
            Shiboken.delete(parent)
            gc.collect()
        self.assertTrue(Shiboken.isValid(context))
        Shiboken.delete(context)


if __name__ == "__main__":
    unittest.main(verbosity=2)
