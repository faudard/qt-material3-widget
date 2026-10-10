"""1.17.9 — route-based history, JSON state and bounded prefetch contracts."""
from __future__ import annotations

import gc
import json
import os
import threading
import time
import unittest

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtWidgets import QApplication, QLabel, QWidget
from shiboken6 import Shiboken

from QtMaterial3 import AsyncLazyTabs, NavigationSession, Widgets


class NavigationSessionContracts(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.app = QApplication.instance() or QApplication([])

    def wait_for(self, predicate, timeout=8.0):
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            self.app.processEvents()
            if predicate():
                return
            time.sleep(0.005)
        self.fail("Timed out waiting for navigation session task")

    @staticmethod
    def make_pages(routes=("/home", "/settings", "/reports", "/help"),
                   *, cancel_on_leave=False, loader=None):
        tabs = Widgets.QtMaterialTabs()
        async_pages = AsyncLazyTabs(
            tabs, max_workers=2, cancel_on_leave=cancel_on_leave
        )
        for route in routes:
            async_pages.addAsyncTab(
                route, loader or (lambda _token, value=route: value),
                lambda data: QLabel(str(data)),
                route=route,
            )
        return tabs, async_pages

    def test_route_history_and_back_forward_branching(self):
        tabs, pages = self.make_pages()
        session = NavigationSession(pages, prefetch_radius=0)
        routes = []
        session.routeChanged.connect(routes.append)
        self.assertEqual(session.history(), ["/home"])
        self.assertFalse(session.canGoBack())
        self.assertTrue(session.navigate("/settings"))
        self.assertTrue(session.navigate("/reports"))
        self.assertEqual(session.history(), ["/home", "/settings", "/reports"])
        self.assertTrue(session.canGoBack())
        self.assertFalse(session.canGoForward())
        self.assertTrue(session.back())
        self.assertEqual(tabs.currentIndex(), 1)
        self.assertTrue(session.back())
        self.assertEqual(session.currentRoute(), "/home")
        self.assertFalse(session.back())
        self.assertTrue(session.forward())
        self.assertEqual(session.currentRoute(), "/settings")
        self.assertTrue(session.canGoForward())
        self.assertTrue(session.navigate("/help"))
        self.assertEqual(session.history(), ["/home", "/settings", "/help"])
        self.assertFalse(session.canGoForward())
        self.assertFalse(session.forward())
        self.assertFalse(session.navigate("/not-registered"))
        self.assertEqual(routes[-1], "/help")
        Shiboken.delete(tabs)

    def test_tab_reordering_uses_routes_not_indices(self):
        tabs, pages = self.make_pages()
        session = NavigationSession(pages, prefetch_radius=0)
        session.navigate("/reports")
        report_widget = tabs.widget(2)
        tabs.tabBar().moveTab(2, 1)
        self.assertIs(tabs.widget(1), report_widget)
        self.assertEqual(session.currentRoute(), "/reports")
        self.assertTrue(session.navigate("/settings"))
        self.assertEqual(session.currentRoute(), "/settings")
        self.assertTrue(session.back())
        self.assertEqual(session.currentRoute(), "/reports")
        self.assertEqual(tabs.currentIndex(), 1)
        Shiboken.delete(tabs)

    def test_snapshot_roundtrip_and_plain_json_state_is_detached(self):
        tabs, pages = self.make_pages()
        session = NavigationSession(pages, prefetch_radius=0)
        session.navigate("/settings")
        form = {"draft": {"value": 12}, "filters": ["a", "b"]}
        session.setPageState("/settings", form)
        form["draft"]["value"] = -1
        self.assertEqual(session.pageState("/settings")["draft"]["value"], 12)
        saved = session.saveState()
        saved["pages"]["/settings"]["draft"]["value"] = 99
        self.assertEqual(session.pageState("/settings")["draft"]["value"], 12)

        other_tabs, other_pages = self.make_pages()
        other_session = NavigationSession(other_pages, prefetch_radius=0)
        other_session.restoreState(json.dumps(session.saveState()))
        self.assertEqual(other_session.currentRoute(), "/settings")
        self.assertEqual(other_session.history(), ["/home", "/settings"])
        self.assertEqual(other_session.pageState("/settings")["filters"], ["a", "b"])
        self.assertTrue(other_session.back())
        self.assertEqual(other_session.currentRoute(), "/home")
        Shiboken.delete(tabs)
        Shiboken.delete(other_tabs)

    def test_restore_filters_removed_routes_and_back_skips_missing(self):
        tabs, pages = self.make_pages()
        session = NavigationSession(pages, prefetch_radius=0)
        session.navigate("/settings")
        session.navigate("/reports")
        session.navigate("/help")
        snapshot = session.saveState()

        removed = pages.removeAsyncTab(tabs.indexOfRoute("/reports"))
        Shiboken.delete(removed)
        self.assertTrue(session.back())
        self.assertEqual(session.currentRoute(), "/settings")

        new_tabs, new_pages = self.make_pages(("/home", "/settings", "/help"))
        new_session = NavigationSession(new_pages, prefetch_radius=0)
        self.assertTrue(new_session.restoreState(snapshot))
        self.assertEqual(new_session.history(), ["/home", "/settings", "/help"])
        self.assertEqual(new_session.currentRoute(), "/help")
        self.assertTrue(new_session.back())
        self.assertEqual(new_session.currentRoute(), "/settings")
        Shiboken.delete(tabs)
        Shiboken.delete(new_tabs)

    def test_prefetch_is_bounded_and_inactive_widgets_are_not_rendered(self):
        started = []
        allow = threading.Event()
        def loader(_token, route=None):
            started.append(threading.get_ident())
            allow.wait(timeout=3.0)
            return "READY"
        tabs, pages = self.make_pages(loader=loader)
        session = NavigationSession(pages, prefetch_radius=2, max_pending=2)
        launched = session.prefetchNeighbors()
        self.assertEqual(launched, [1])
        self.assertTrue(pages.isLoading(0))
        self.assertTrue(pages.isLoading(1))
        self.assertFalse(pages.isLoading(2))
        allow.set()
        self.wait_for(lambda: not pages.isLoading(0) and not pages.isLoading(1))
        self.assertFalse(pages.isReady(1))  # prefetched data only, no QWidget
        tabs.setCurrentIndex(1)
        self.wait_for(lambda: pages.isReady(1))
        self.assertEqual(pages._entry(1).content.text(), "READY")
        self.assertTrue(all(ident != threading.get_ident() for ident in started))
        Shiboken.delete(tabs)

    def test_navigation_updates_while_rapidly_switching(self):
        tabs, pages = self.make_pages(cancel_on_leave=True)
        session = NavigationSession(pages, prefetch_radius=1, max_pending=2)
        for _ in range(20):
            session.navigate("/settings")
            session.navigate("/reports")
            session.navigate("/home")
        self.assertEqual(session.currentRoute(), "/home")
        self.assertLessEqual(len(session.history()), 100)
        state = session.saveState()
        self.assertEqual(state["history"][state["cursor"]], "/home")
        Shiboken.delete(tabs)
        gc.collect()

    def test_bad_snapshots_do_not_partially_modify_existing_session(self):
        tabs, pages = self.make_pages()
        session = NavigationSession(pages, prefetch_radius=0)
        session.navigate("/settings")
        before = session.saveState()
        bad = [
            {"version": 99, "history": [], "cursor": -1, "currentRoute": "", "pages": {}},
            {"version": 1, "history": ["/home"], "cursor": 99,
             "currentRoute": "/home", "pages": {}},
            {"version": 1, "history": ["/home"], "cursor": 0,
             "currentRoute": "/settings", "pages": {}},
            {"version": 1, "history": "wrong", "cursor": -1,
             "currentRoute": "", "pages": {}},
        ]
        for item in bad:
            with self.subTest(item=item), self.assertRaises(ValueError):
                session.restoreState(item)
            self.assertEqual(session.saveState(), before)
        with self.assertRaises((TypeError, ValueError)):
            session.setPageState("/settings", QWidget())
        with self.assertRaises(ValueError):
            session.setPageState("/deleted", {})
        Shiboken.delete(tabs)

    def test_close_guards_operations_and_parent_deletion_invalidates_helper(self):
        tabs, pages = self.make_pages()
        session = NavigationSession(pages)
        session.close()
        with self.assertRaises(RuntimeError):
            session.navigate("/settings")
        Shiboken.delete(tabs)
        self.assertFalse(Shiboken.isValid(session))
        self.assertFalse(Shiboken.isValid(pages))

    def test_invalid_ctor_limits_and_ambiguous_routes(self):
        tabs, pages = self.make_pages()
        for name, value in (("prefetch_radius", -1), ("max_pending", 0),
                            ("max_history", 0), ("max_pending", 9)):
            with self.subTest(name=name), self.assertRaises(ValueError):
                NavigationSession(pages, **{name: value})
        session = NavigationSession(pages, prefetch_radius=0)
        self.assertFalse(session.navigate("/does-not-exist"))
        self.assertRaises(TypeError, session.navigate, None)
        tabs.setRoute(1, "/home")  # duplicates are intentionally ambiguous
        self.assertFalse(session.navigate("/home"))
        Shiboken.delete(tabs)


if __name__ == "__main__":
    unittest.main(verbosity=2)
