"""1.17.7 Python lazy page factory integration/ownership regression tests."""

import gc
import os
import unittest
import weakref

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtWidgets import QApplication, QLabel, QVBoxLayout, QWidget
from shiboken6 import Shiboken

from QtMaterial3 import LazyTabs, Widgets


class LazyTabsContracts(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.app = QApplication.instance() or QApplication([])

    def test_first_page_eager_other_pages_deferred_and_created_once(self):
        tabs = Widgets.QtMaterialTabs()
        lazy = LazyTabs(tabs)
        calls = []
        a = lazy.addLazyTab("Overview", lambda: (calls.append("a"), QLabel("A"))[1])
        b = lazy.addLazyTab("Settings", lambda: (calls.append("b"), QLabel("B"))[1],
                            route="/settings")
        self.assertEqual((a, b), (0, 1))
        self.assertEqual(calls, ["a"])
        self.assertTrue(lazy.isLoaded(a))
        self.assertFalse(lazy.isLoaded(b))
        self.assertIsInstance(tabs.widget(0).layout(), QVBoxLayout)

        loaded = []
        lazy.pageLoaded.connect(lambda index, widget: loaded.append((index, widget.text())))
        tabs.setCurrentIndex(1)
        self.assertEqual(calls, ["a", "b"])
        self.assertEqual(loaded, [(1, "B")])
        self.assertTrue(lazy.isLoaded(1))
        self.assertEqual(tabs.route(1).path(), "/settings")
        tabs.setCurrentIndex(0)
        tabs.setCurrentIndex(1)
        self.assertEqual(calls, ["a", "b"])
        Shiboken.delete(tabs)
        self.assertFalse(Shiboken.isValid(lazy))

    def test_register_existing_empty_tab_and_manual_ensure(self):
        tabs = Widgets.QtMaterialTabs()
        tabs.addTab(QWidget(), "Active")
        tabs.addTab(QWidget(), "Deferred")
        lazy = LazyTabs(tabs)
        calls = []
        lazy.registerTab(1, lambda: (calls.append(1), QLabel("Deferred"))[1])
        self.assertEqual(calls, [])
        self.assertIsNone(lazy.lastError(1))
        child = lazy.ensureLoaded(1)
        self.assertIsInstance(child, QLabel)
        self.assertEqual(child.text(), "Deferred")
        self.assertIs(child.parentWidget(), tabs.widget(1))
        self.assertIs(child, lazy.ensureLoaded(1))
        self.assertEqual(calls, [1])
        with self.assertRaises(ValueError):
            lazy.registerTab(1, lambda: QLabel("Duplicate"))
        self.assertTrue(lazy.unregisterTab(1))
        self.assertFalse(lazy.unregisterTab(1))
        self.assertTrue(Shiboken.isValid(child))
        Shiboken.delete(tabs)
        self.assertFalse(Shiboken.isValid(child))

    def test_errors_are_reported_and_retryable_not_raised_from_qt_signals(self):
        tabs = Widgets.QtMaterialTabs()
        lazy = LazyTabs(tabs)
        lazy.addLazyTab("Normal", lambda: QWidget())
        attempts = []
        failures = []
        lazy.loadFailed.connect(lambda i, err: failures.append((i, err)))
        def sometimes_bad():
            attempts.append(True)
            if len(attempts) == 1:
                raise ValueError("temporary failure")
            return QLabel("Recovered")
        lazy.addLazyTab("Deferred", sometimes_bad)
        tabs.setCurrentIndex(1)
        self.assertEqual(len(attempts), 1)
        self.assertFalse(lazy.isLoaded(1))
        self.assertIn("temporary failure", lazy.lastError(1))
        self.assertEqual(len(failures), 1)
        self.assertIn("ValueError", failures[0][1])
        widget = lazy.ensureLoaded(1)
        self.assertEqual(widget.text(), "Recovered")
        self.assertTrue(lazy.isLoaded(1))
        self.assertIsNone(lazy.lastError(1))
        Shiboken.delete(tabs)

    def test_reordering_tracks_page_identity_not_stale_index(self):
        tabs = Widgets.QtMaterialTabs()
        lazy = LazyTabs(tabs)
        calls = []
        lazy.addLazyTab("First", lambda: QLabel("First"))
        lazy.addLazyTab("Second", lambda: (calls.append("second"), QLabel("Second"))[1])
        lazy.addLazyTab("Third", lambda: (calls.append("third"), QLabel("Third"))[1])
        third_page = tabs.widget(2)

        tabs.tabBar().moveTab(2, 1)
        self.assertIs(tabs.widget(1), third_page)
        tabs.setCurrentIndex(1)
        self.assertEqual(calls, ["third"])
        self.assertEqual(lazy.ensureLoaded(1).text(), "Third")
        tabs.setCurrentIndex(2)
        self.assertEqual(calls, ["third", "second"])
        self.assertEqual(lazy.ensureLoaded(2).text(), "Second")
        Shiboken.delete(tabs)

    def test_reentrant_factory_is_called_once(self):
        tabs = Widgets.QtMaterialTabs()
        lazy = LazyTabs(tabs)
        lazy.addLazyTab("A", lambda: QWidget())
        calls = []
        def factory():
            calls.append(True)
            self.assertIsNone(lazy.ensureLoaded(1))
            return QWidget()
        lazy.addLazyTab("B", factory)
        tabs.setCurrentIndex(1)
        self.assertEqual(len(calls), 1)
        self.assertTrue(lazy.isLoaded(1))
        Shiboken.delete(tabs)

    def test_invalid_result_and_foreign_parent_are_rejected(self):
        tabs = Widgets.QtMaterialTabs()
        lazy = LazyTabs(tabs)
        lazy.addLazyTab("First", lambda: QWidget())
        lazy.addLazyTab("Wrong type", lambda: "not a QWidget")
        tabs.setCurrentIndex(1)
        self.assertIn("live QWidget", lazy.lastError(1))

        foreign_parent = QWidget()
        lazy.addLazyTab("Foreign", lambda: QWidget(foreign_parent))
        tabs.setCurrentIndex(2)
        self.assertIn("another parent", lazy.lastError(2))
        self.assertTrue(Shiboken.isValid(foreign_parent))
        Shiboken.delete(tabs)
        Shiboken.delete(foreign_parent)

    def test_remove_releases_uninvoked_callback_without_deleting_page(self):
        class Capture:
            pass

        tabs = Widgets.QtMaterialTabs()
        lazy = LazyTabs(tabs)
        lazy.addLazyTab("First", lambda: QWidget())
        holder = Capture()
        ref = weakref.ref(holder)
        factory = lambda h=holder: (h, QWidget())[1]
        lazy.addLazyTab("Never loaded", factory)
        del factory, holder
        gc.collect()
        self.assertIsNotNone(ref())
        page = lazy.removeLazyTab(1)
        gc.collect()
        self.assertIsNone(ref())
        self.assertEqual(tabs.count(), 1)
        self.assertTrue(Shiboken.isValid(page))
        Shiboken.delete(page)
        Shiboken.delete(tabs)

    def test_native_parent_deletion_invalidates_lazy_content_and_helper(self):
        for _ in range(10):
            tabs = Widgets.QtMaterialTabs()
            lazy = LazyTabs(tabs)
            lazy.addLazyTab("A", lambda: QWidget())
            lazy.addLazyTab("B", lambda: QWidget())
            tabs.setCurrentIndex(1)
            first, second = lazy.ensureLoaded(0), lazy.ensureLoaded(1)
            self.assertTrue(Shiboken.isValid(first))
            self.assertTrue(Shiboken.isValid(second))
            Shiboken.delete(tabs)
            for obj in (lazy, first, second):
                self.assertFalse(Shiboken.isValid(obj))
            del tabs, lazy, first, second
            gc.collect()

    def test_invalid_inputs_fail_fast(self):
        with self.assertRaises(TypeError):
            LazyTabs(QWidget())
        tabs = Widgets.QtMaterialTabs()
        lazy = LazyTabs(tabs)
        with self.assertRaises(TypeError):
            lazy.addLazyTab("Wrong", None)
        with self.assertRaises(IndexError):
            lazy.registerTab(99, lambda: QWidget())
        tabs.addTab(QWidget(), "Empty")
        with self.assertRaises(TypeError):
            lazy.registerTab(0, None)
        Shiboken.delete(tabs)


if __name__ == "__main__":
    unittest.main(verbosity=2)
