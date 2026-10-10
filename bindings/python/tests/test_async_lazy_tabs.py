"""Async Lazy Tabs: worker isolation, cancellation, routing and ownership."""
import gc
import os
import threading
import time
import unittest
import weakref

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtCore import QThread
from PySide6.QtWidgets import QApplication, QLabel, QWidget
from shiboken6 import Shiboken

from QtMaterial3 import AsyncLazyTabs, Widgets


class AsyncLazyTabsContracts(unittest.TestCase):
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
        self.fail("Timed out waiting for asynchronous page result")

    def test_worker_is_background_renderer_is_gui_and_only_once(self):
        tabs = Widgets.QtMaterialTabs()
        async_tabs = AsyncLazyTabs(tabs)
        main_ident = threading.get_ident()
        worker_ident = []
        render_ident = []
        ready = []
        def loader(cancel):
            worker_ident.append(threading.get_ident())
            self.assertFalse(cancel.is_set())
            return {"title": "Data ready"}
        def render(data):
            render_ident.append(threading.get_ident())
            self.assertEqual(QThread.currentThread(), self.app.thread())
            return QLabel(data["title"])
        async_tabs.pageReady.connect(lambda index, widget: ready.append((index, widget)))
        idx = async_tabs.addAsyncTab("Data", loader, render, route="/data")
        self.wait_for(lambda: async_tabs.isReady(idx))
        self.assertEqual(worker_ident and len(worker_ident), 1)
        self.assertNotEqual(worker_ident[0], main_ident)
        self.assertEqual(render_ident, [main_ident])
        self.assertEqual(ready[0][0], idx)
        self.assertEqual(ready[0][1].text(), "Data ready")
        self.assertEqual(tabs.route(idx).path(), "/data")
        async_tabs.requestPage(idx)
        self.assertEqual(len(worker_ident), 1)
        Shiboken.delete(tabs)
        self.assertFalse(Shiboken.isValid(async_tabs))

    def test_immediate_switch_cancels_initially_selected_page(self):
        tabs = Widgets.QtMaterialTabs()
        async_tabs = AsyncLazyTabs(tabs)
        started = threading.Event()
        released = threading.Event()
        cancelled = threading.Event()
        def first_loader(token):
            started.set()
            released.wait(timeout=3.0)
            if token.is_set():
                cancelled.set()
            return "too late"
        async_tabs.addAsyncTab("Initial", first_loader, lambda s: QLabel(s))
        self.wait_for(started.is_set)
        async_tabs.addAsyncTab("Next", lambda _: "ready", lambda s: QLabel(s))
        tabs.setCurrentIndex(1)
        self.assertFalse(async_tabs.isLoading(0))
        released.set()
        self.wait_for(cancelled.is_set)
        self.wait_for(lambda: async_tabs.isReady(1))
        self.assertFalse(async_tabs.isReady(0))
        Shiboken.delete(tabs)

    def test_change_tab_cancels_pending_and_ignores_stale_result(self):
        tabs = Widgets.QtMaterialTabs()
        async_tabs = AsyncLazyTabs(tabs, max_workers=2)
        slow_started = threading.Event()
        release = threading.Event()
        observed = []
        cancelled = []
        async_tabs.pageReady.connect(lambda i, _: observed.append(i))
        async_tabs.pageCancelled.connect(cancelled.append)
        async_tabs.addAsyncTab("Ready", lambda _: "A", lambda data: QLabel(data))
        self.wait_for(lambda: async_tabs.isReady(0))
        def slow(cancel):
            slow_started.set()
            release.wait(timeout=3.0)  # intentionally does not cooperate
            return "STALE"
        async_tabs.addAsyncTab("Slow", slow, lambda data: QLabel(data))
        tabs.setCurrentIndex(1)
        self.wait_for(slow_started.is_set)
        tabs.setCurrentIndex(0)
        self.assertIn(1, cancelled)
        self.assertFalse(async_tabs.isLoading(1))
        release.set()
        # Restart on next visit with fresh generation; stale result is ignored.
        tabs.setCurrentIndex(1)
        self.wait_for(lambda: async_tabs.isReady(1))
        self.assertEqual(async_tabs._entry(1).content.text(), "STALE")
        self.assertEqual(observed.count(1), 1)
        Shiboken.delete(tabs)

    def test_cached_result_does_not_render_on_inactive_tab(self):
        tabs = Widgets.QtMaterialTabs()
        async_tabs = AsyncLazyTabs(tabs, cancel_on_leave=False)
        allow = threading.Event()
        rendering = []
        async_tabs.addAsyncTab("Fast", lambda _: "A", lambda s: QLabel(s))
        self.wait_for(lambda: async_tabs.isReady(0))
        async_tabs.addAsyncTab("Other", lambda _: (allow.wait(3.0), "B")[1],
                               lambda s: (rendering.append(s), QLabel(s))[1])
        tabs.setCurrentIndex(1)
        tabs.setCurrentIndex(0)
        allow.set()
        self.wait_for(lambda: not async_tabs.isLoading(1))
        self.assertFalse(async_tabs.isReady(1))
        self.assertEqual(rendering, [])
        tabs.setCurrentIndex(1)
        self.wait_for(lambda: async_tabs.isReady(1))
        self.assertEqual(rendering, ["B"])
        Shiboken.delete(tabs)

    def test_loader_failure_retry_then_render_failure_retry_without_reload(self):
        tabs = Widgets.QtMaterialTabs()
        async_tabs = AsyncLazyTabs(tabs)
        attempts = []
        paints = []
        errors = []
        async_tabs.loadFailed.connect(lambda i, err: errors.append((i, err)))
        def loader(_):
            attempts.append(True)
            if len(attempts) == 1:
                raise ValueError("data load failed")
            return "loaded"
        def renderer(data):
            paints.append(True)
            if len(paints) == 1:
                raise ValueError("widget creation failed")
            return QLabel(data)
        async_tabs.addAsyncTab("Fallible", loader, renderer)
        self.wait_for(lambda: bool(errors))
        self.assertIn("data load failed", async_tabs.lastError(0))
        async_tabs.requestPage(0)
        self.wait_for(lambda: len(errors) == 2)
        self.assertIn("widget creation failed", async_tabs.lastError(0))
        async_tabs.requestPage(0)
        self.assertTrue(async_tabs.isReady(0))
        self.assertEqual((len(attempts), len(paints)), (2, 2))
        self.assertIsNone(async_tabs.lastError(0))
        Shiboken.delete(tabs)

    def test_unregister_releases_callback_and_removed_widget_survives(self):
        class Capture:
            pass

        tabs = Widgets.QtMaterialTabs()
        async_tabs = AsyncLazyTabs(tabs)
        async_tabs.addAsyncTab("A", lambda _: 1, lambda n: QLabel(str(n)))
        self.wait_for(lambda: async_tabs.isReady(0))
        payload = Capture()
        ref = weakref.ref(payload)
        loader = lambda event, held=payload: held
        async_tabs.addAsyncTab("Never opened", loader, lambda _: QLabel("B"))
        del loader, payload
        gc.collect()
        self.assertIsNotNone(ref())
        page = async_tabs.removeAsyncTab(1)
        gc.collect()
        self.assertIsNone(ref())
        self.assertTrue(Shiboken.isValid(page))
        self.assertEqual(tabs.count(), 1)
        Shiboken.delete(page)
        Shiboken.delete(tabs)

    def test_reordered_pages_follow_placeholder_identity(self):
        tabs = Widgets.QtMaterialTabs()
        async_tabs = AsyncLazyTabs(tabs)
        async_tabs.addAsyncTab("A", lambda _: "A", lambda d: QLabel(d))
        self.wait_for(lambda: async_tabs.isReady(0))
        async_tabs.addAsyncTab("B", lambda _: "B", lambda d: QLabel(d))
        async_tabs.addAsyncTab("C", lambda _: "C", lambda d: QLabel(d))
        page_c = tabs.widget(2)
        tabs.tabBar().moveTab(2, 1)
        self.assertIs(tabs.widget(1), page_c)
        tabs.setCurrentIndex(1)
        self.wait_for(lambda: async_tabs.isReady(1))
        self.assertEqual(async_tabs._entry(1).content.text(), "C")
        tabs.setCurrentIndex(2)
        self.wait_for(lambda: async_tabs.isReady(2))
        self.assertEqual(async_tabs._entry(2).content.text(), "B")
        Shiboken.delete(tabs)

    def test_parent_destroyed_during_inflight_load_releases_controller(self):
        tabs = Widgets.QtMaterialTabs()
        async_tabs = AsyncLazyTabs(tabs)
        started = threading.Event()
        finish = threading.Event()
        def loader(cancel):
            started.set()
            finish.wait(timeout=3.0)
            return "discard"
        async_tabs.addAsyncTab("Pending", loader, lambda x: QLabel(x))
        self.wait_for(started.is_set)
        Shiboken.delete(tabs)
        self.assertFalse(Shiboken.isValid(async_tabs))
        finish.set()
        # The background task must not dereference a destroyed Qt object.
        for _ in range(15):
            self.app.processEvents()
            time.sleep(0.005)
        gc.collect()

    def test_explicit_cancel_prevents_old_result_from_becoming_page(self):
        tabs = Widgets.QtMaterialTabs()
        async_tabs = AsyncLazyTabs(tabs)
        started = threading.Event()
        release = threading.Event()
        def loader(_):
            started.set()
            release.wait(timeout=3.0)
            return "delayed"
        async_tabs.addAsyncTab("Cancel", loader, lambda x: QLabel(x))
        self.wait_for(started.is_set)
        self.assertTrue(async_tabs.cancelPage(0))
        self.assertFalse(async_tabs.isLoading(0))
        release.set()
        self.wait_for(lambda: not async_tabs.isLoading(0))
        self.assertFalse(async_tabs.isReady(0))
        async_tabs.requestPage(0)
        self.wait_for(lambda: async_tabs.isReady(0))
        Shiboken.delete(tabs)

    def test_close_is_nonblocking_and_rejects_new_operations(self):
        tabs = Widgets.QtMaterialTabs()
        async_tabs = AsyncLazyTabs(tabs)
        async_tabs.addAsyncTab("A", lambda _: "A", lambda d: QLabel(d))
        async_tabs.close()
        with self.assertRaises(RuntimeError):
            async_tabs.addAsyncTab("B", lambda _: "B", lambda d: QLabel(d))
        Shiboken.delete(tabs)

    def test_bad_arguments(self):
        with self.assertRaises(TypeError):
            AsyncLazyTabs(QWidget())
        tabs = Widgets.QtMaterialTabs()
        with self.assertRaises(ValueError):
            AsyncLazyTabs(tabs, max_workers=0)
        async_tabs = AsyncLazyTabs(tabs)
        with self.assertRaises(TypeError):
            async_tabs.addAsyncTab("Invalid", None, lambda _: QWidget())
        with self.assertRaises(IndexError):
            async_tabs.registerAsyncTab(8, lambda _: {}, lambda _: QWidget())
        Shiboken.delete(tabs)


if __name__ == "__main__":
    unittest.main(verbosity=2)
