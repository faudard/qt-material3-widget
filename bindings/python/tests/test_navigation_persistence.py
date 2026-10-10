"""1.17.10: real QWidget state persistence, safe fields and bounded data LRU."""

import gc
import os
import threading
import time
import unittest
import weakref

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtWidgets import (
    QApplication, QCheckBox, QComboBox, QDoubleSpinBox, QLineEdit,
    QPlainTextEdit, QSlider, QSpinBox, QVBoxLayout, QWidget,
)
from shiboken6 import Shiboken

from QtMaterial3 import (
    AsyncLazyTabs, NavigationSession, Widgets, snapshotForm, restoreForm,
)


def build_form(_data=None):
    root = QWidget()
    layout = QVBoxLayout(root)

    def add(widget, name):
        widget.setObjectName(name)
        layout.addWidget(widget)
        return widget

    add(QLineEdit(), "username")
    add(QLineEdit(), "passwordField").setEchoMode(QLineEdit.Password)
    add(QLineEdit(), "api_token")
    add(QCheckBox("Enabled"), "enabled")
    combo = add(QComboBox(), "theme")
    combo.addItems(["Light", "Dark"])
    add(QSpinBox(), "count").setRange(0, 500)
    add(QDoubleSpinBox(), "price").setRange(0.0, 500.0)
    add(QSlider(), "level").setRange(0, 100)
    add(QPlainTextEdit(), "notes")
    add(QLineEdit(), "excluded").setProperty("navigationPersist", False)
    return root


def field(root, name):
    return root.findChild(QWidget, name)


class NavigationPersistenceContracts(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.app = QApplication.instance() or QApplication([])

    def wait_for(self, predicate, timeout=8):
        until = time.monotonic() + timeout
        while time.monotonic() < until:
            self.app.processEvents()
            if predicate():
                return
            time.sleep(0.005)
        self.fail("Timed out waiting for async form navigation")

    def make_pages(self):
        tabs = Widgets.QtMaterialTabs()
        pages = AsyncLazyTabs(tabs, max_workers=2, cancel_on_leave=False)
        pages.addAsyncTab("Home", lambda _: {"page": "home"},
                          lambda _: QWidget(), route="/home")
        pages.addAsyncTab("Form", lambda _: {"page": "form"},
                          build_form, route="/form")
        return tabs, pages

    def test_form_snapshot_secure_and_restoration(self):
        root = build_form()
        field(root, "username").setText("Alice")
        field(root, "passwordField").setText("never save me")
        field(root, "api_token").setText("secret-key")
        field(root, "enabled").setChecked(True)
        field(root, "theme").setCurrentIndex(1)
        field(root, "count").setValue(42)
        field(root, "price").setValue(12.5)
        field(root, "level").setValue(72)
        field(root, "notes").setPlainText("draft")
        field(root, "excluded").setText("excluded value")
        snapshot = snapshotForm(root)
        self.assertEqual(snapshot["version"], 1)
        self.assertEqual(snapshot["fields"]["username"]["value"], "Alice")
        self.assertNotIn("passwordField", snapshot["fields"])
        self.assertNotIn("api_token", snapshot["fields"])
        self.assertNotIn("excluded", snapshot["fields"])

        new_root = build_form()
        changed = restoreForm(new_root, snapshot)
        self.assertGreaterEqual(changed, 7)
        self.assertEqual(field(new_root, "username").text(), "Alice")
        self.assertTrue(field(new_root, "enabled").isChecked())
        self.assertEqual(field(new_root, "theme").currentIndex(), 1)
        self.assertEqual(field(new_root, "count").value(), 42)
        self.assertAlmostEqual(field(new_root, "price").value(), 12.5)
        self.assertEqual(field(new_root, "level").value(), 72)
        self.assertEqual(field(new_root, "notes").toPlainText(), "draft")
        self.assertEqual(field(new_root, "passwordField").text(), "")
        self.assertEqual(field(new_root, "excluded").text(), "")
        Shiboken.delete(root)
        Shiboken.delete(new_root)

    def test_invalid_form_snapshot_and_duplicate_names(self):
        root = build_form()
        with self.assertRaises(ValueError):
            restoreForm(root, {"version": 10, "fields": {}})
        with self.assertRaises(TypeError):
            snapshotForm(None)
        duplicate = QLineEdit(root)
        duplicate.setObjectName("username")
        self.assertNotIn("username", snapshotForm(root)["fields"])
        Shiboken.delete(root)

    def test_navigation_auto_capture_and_restore_on_lazy_page_loaded(self):
        tabs, pages = self.make_pages()
        session = NavigationSession(pages, prefetch_radius=0)
        session.registerForm("/form")
        self.assertTrue(session.navigate("/form"))
        self.wait_for(lambda: pages.isReady(1))
        original = pages._entry(1).content
        field(original, "username").setText("Remember me")
        field(original, "enabled").setChecked(True)
        session.navigate("/home")
        self.assertEqual(session.pageState("/form")["fields"]["username"]["value"],
                         "Remember me")
        saved = session.saveState()

        other_tabs, other_pages = self.make_pages()
        other_session = NavigationSession(other_pages, prefetch_radius=0)
        other_session.registerForm("/form")
        self.assertTrue(other_session.restoreState(saved))
        self.assertEqual(other_session.currentRoute(), "/home")
        other_session.navigate("/form")
        self.wait_for(lambda: other_pages.isReady(1))
        restored = other_pages._entry(1).content
        self.assertEqual(field(restored, "username").text(), "Remember me")
        self.assertTrue(field(restored, "enabled").isChecked())
        Shiboken.delete(tabs)
        Shiboken.delete(other_tabs)
        self.assertFalse(Shiboken.isValid(session))

    def test_unregistration_releases_callback_captures(self):
        class Capture:
            pass
        tabs, pages = self.make_pages()
        session = NavigationSession(pages, prefetch_radius=0)
        holder = Capture()
        ref = weakref.ref(holder)
        capture = lambda widget, h=holder: {}
        apply = lambda widget, data: None
        session.registerStateProvider("/form", capture, apply)
        del capture, apply, holder
        gc.collect()
        self.assertIsNotNone(ref())
        self.assertTrue(session.unregisterStateProvider("/form"))
        gc.collect()
        self.assertIsNone(ref())
        Shiboken.delete(tabs)

    def test_provider_errors_are_signaled_without_breaking_navigation(self):
        tabs, pages = self.make_pages()
        session = NavigationSession(pages, prefetch_radius=0)
        errors = []
        session.stateProviderFailed.connect(lambda path, text: errors.append((path, text)))
        session.registerStateProvider(
            "/form",
            lambda _: (_ for _ in ()).throw(ValueError("capture failed")),
            lambda widget, value: None,
        )
        session.navigate("/form")
        self.wait_for(lambda: pages.isReady(1))
        session.navigate("/home")
        self.assertTrue(errors)
        self.assertEqual(errors[0][0], "/form")
        self.assertIn("capture failed", errors[0][1])
        Shiboken.delete(tabs)

    def test_lru_evicts_plain_data_not_widgets(self):
        tabs = Widgets.QtMaterialTabs()
        pages = AsyncLazyTabs(tabs, max_workers=2, cancel_on_leave=False)
        counts = {"/home": 0, "/a": 0, "/b": 0}
        for route in counts:
            def loader(_cancel, path=route):
                counts[path] += 1
                return path
            pages.addAsyncTab(route, loader, lambda data: QWidget(), route=route)
        session = NavigationSession(
            pages, prefetch_radius=0, max_pending=2, max_cached_pages=1
        )
        self.wait_for(lambda: pages.isReady(0))
        evicted = []
        session.cacheEvicted.connect(evicted.append)
        pages.requestPage(1)
        self.wait_for(lambda: pages.hasCachedData(1))
        pages.requestPage(2)
        self.wait_for(lambda: pages.hasCachedData(2))
        self.assertFalse(pages.hasCachedData(1))
        self.assertEqual(evicted, [1])
        self.assertTrue(pages.isReady(0))
        self.assertFalse(pages.evictCachedData(0))
        session.navigate("/a")
        self.wait_for(lambda: pages.isReady(1))
        self.assertEqual(counts["/a"], 2)
        self.assertEqual(counts["/home"], 1)
        Shiboken.delete(tabs)

    def test_zero_cache_evicts_all_inactive_prefetch(self):
        tabs = Widgets.QtMaterialTabs()
        pages = AsyncLazyTabs(tabs, cancel_on_leave=False)
        for route in ("/a", "/b"):
            pages.addAsyncTab(route, lambda _: 1, lambda _: QWidget(), route=route)
        session = NavigationSession(pages, prefetch_radius=0, max_cached_pages=0)
        pages.requestPage(1)
        self.wait_for(lambda: not pages.isLoading(1))
        self.assertFalse(pages.hasCachedData(1))
        self.assertFalse(pages.isReady(1))
        Shiboken.delete(tabs)

    def test_cache_limits_are_validated(self):
        tabs, pages = self.make_pages()
        with self.assertRaises(ValueError):
            NavigationSession(pages, max_cached_pages=-1)
        with self.assertRaises(ValueError):
            NavigationSession(pages, max_cached_pages=65)
        with self.assertRaises(ValueError):
            NavigationSession(pages, max_cached_pages=True)
        Shiboken.delete(tabs)


if __name__ == "__main__":
    unittest.main(verbosity=2)
