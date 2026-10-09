"""Python ↔ native QtMaterial3 route and model ownership contracts."""

import gc
import os
import unittest

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtCore import Qt
from PySide6.QtWidgets import QApplication, QWidget
from shiboken6 import Shiboken

import QtMaterial3
from QtMaterial3 import Widgets


class NavigationRouteModelContracts(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.app = QApplication.instance() or QApplication([])

    def test_route_is_a_normalized_value_and_round_trips_from_tabs(self):
        route = QtMaterial3.QtMaterialRoute("  //settings///account/  ")
        self.assertTrue(route.isValid())
        self.assertEqual(route.path(), "/settings/account")
        self.assertEqual(route.toString(), "/settings/account")
        self.assertEqual(
            QtMaterial3.QtMaterialRoute.normalizedPath("settings//account/"),
            "/settings/account",
        )

        tabs = Widgets.QtMaterialTabs()
        tabs.addTab(QWidget(), "Home")
        tabs.addTab(QWidget(), "Account")
        tabs.setRoute(0, QtMaterial3.QtMaterialRoute("/home"))
        tabs.setRoute(1, route)
        self.assertEqual(tabs.route(1).path(), "/settings/account")
        self.assertEqual(tabs.indexOfRoute(route), 1)
        self.assertTrue(tabs.navigateTo(route))
        self.assertEqual(tabs.currentIndex(), 1)
        self.assertFalse(tabs.navigateTo(QtMaterial3.QtMaterialRoute("/missing")))
        Shiboken.delete(tabs)

    def test_navigation_items_reach_qabstractlistmodel(self):
        model = QtMaterial3.QtMaterialNavigationModel()
        home = QtMaterial3.QtMaterialNavigationItem()
        home.id = "home"
        home.route = "/home"
        home.label = "Home"
        settings = QtMaterial3.QtMaterialNavigationItem()
        settings.id = "settings"
        settings.route = "/settings"
        settings.label = "Settings"

        model.addItem(home)
        model.addItem(settings)
        self.assertEqual(model.rowCount(), 2)
        self.assertEqual(model.data(model.index(1, 0), Qt.DisplayRole), "Settings")
        self.assertEqual(model.itemAt(0).id, "home")

        ids, routes = [], []
        model.selectedIdChanged.connect(ids.append)
        model.selectedRouteChanged.connect(routes.append)
        self.assertTrue(model.setSelectedId("settings"))
        self.assertEqual(model.selectedId(), "settings")
        self.assertEqual(model.selectedRoute(), "/settings")
        self.assertEqual(ids, ["settings"])
        self.assertEqual(routes, ["/settings"])
        self.assertFalse(model.setSelectedRoute("/unknown"))
        Shiboken.delete(model)

    def test_tabs_retain_borrowed_model_without_taking_ownership(self):
        tabs = Widgets.QtMaterialTabs()
        tabs.addTab(QWidget(), "Home")
        tabs.addTab(QWidget(), "Settings")
        tabs.setTabId(0, "home")
        tabs.setTabId(1, "settings")
        tabs.setRoute(0, "/home")
        tabs.setRoute(1, "/settings")

        model = QtMaterial3.QtMaterialNavigationModel()
        changed = []
        tabs.navigationModelChanged.connect(changed.append)
        tabs.setNavigationModel(model)
        self.assertIs(tabs.navigationModel(), model)
        self.assertEqual(changed, [model])
        self.assertEqual(model.rowCount(), 2)

        self.assertTrue(model.setSelectedRoute("/settings"))
        self.assertEqual(tabs.currentIndex(), 1)

        tabs.setNavigationModel(None)
        self.assertIsNone(tabs.navigationModel())
        self.assertTrue(Shiboken.isValid(model))
        Shiboken.delete(tabs)
        self.assertTrue(Shiboken.isValid(model))
        Shiboken.delete(model)
        del tabs, model
        gc.collect()

    def test_qpointer_nulls_out_after_model_is_deleted(self):
        tabs = Widgets.QtMaterialTabs()
        model = QtMaterial3.QtMaterialNavigationModel()
        tabs.setNavigationModel(model)
        Shiboken.delete(model)
        self.assertIsNone(tabs.navigationModel())
        self.assertTrue(Shiboken.isValid(tabs))
        Shiboken.delete(tabs)


if __name__ == "__main__":
    unittest.main(verbosity=2)
