"""Async data-loading tabs with a strictly GUI-thread QWidget render phase.

Workers execute only user-provided data loaders. They never read/write a
QWidget, emit Qt signals, or call methods on the QObject manager: results are
posted to a thread-safe queue and consumed by a GUI-thread QTimer.
"""
from __future__ import annotations

from concurrent.futures import Future, ThreadPoolExecutor
from dataclasses import dataclass, field
from queue import Empty, SimpleQueue
from threading import Event
import traceback
from typing import Any, Callable
import weakref

from PySide6.QtCore import QObject, QThread, QTimer, Signal
from PySide6.QtWidgets import QVBoxLayout, QWidget
from shiboken6 import Shiboken

from .Widgets import QtMaterialTabs


_MISSING = object()


@dataclass
class _AsyncPage:
    placeholder: QWidget
    loader: Callable[[Event], Any]
    renderer: Callable[[Any], QWidget]
    generation: int = 0
    cancellation: Event | None = None
    future: Future | None = None
    payload: Any = field(default=_MISSING)
    content: QWidget | None = None
    error: str | None = None


def _execute(key: int, generation: int, loader: Callable[[Event], Any],
             cancel: Event, results: SimpleQueue) -> None:
    """Worker-side path: no QObject access, even in the error path."""
    try:
        if not cancel.is_set():
            result = loader(cancel)
            if not cancel.is_set():
                results.put((key, generation, result, None))
    except Exception:
        if not cancel.is_set():
            results.put((key, generation, _MISSING, traceback.format_exc()))


class AsyncLazyTabs(QObject):
    """Asynchronous *data* loading for native QtMaterialTabs.

    Loader signature: loader(cancel: threading.Event) -> plain Python data.
    Renderer signature: renderer(data) -> QWidget, called ONLY on the GUI thread.

    A pending task is cooperatively cancelled on navigation away by default;
    its late results are invalidated by a per-registration generation number.
    Callbacks are not dispatched directly by the worker thread.
    """

    pageReady = Signal(int, QWidget)
    dataReady = Signal(int)
    loadFailed = Signal(int, str)
    pageCancelled = Signal(int)

    def __init__(self, tabs: QtMaterialTabs, *, max_workers: int = 2,
                 cancel_on_leave: bool = True, poll_interval_ms: int = 15):
        if not isinstance(tabs, QtMaterialTabs) or not Shiboken.isValid(tabs):
            raise TypeError("AsyncLazyTabs requires a live QtMaterialTabs")
        if not 1 <= max_workers <= 8:
            raise ValueError("max_workers must be between 1 and 8")
        if poll_interval_ms < 1:
            raise ValueError("poll_interval_ms must be positive")
        super().__init__(tabs)
        self._tabs = tabs
        self._pages: dict[int, _AsyncPage] = {}
        self._results: SimpleQueue = SimpleQueue()
        self._pool = ThreadPoolExecutor(
            max_workers=max_workers, thread_name_prefix="qtm3-async-page"
        )
        self._closed = False
        self._cancel_on_leave = bool(cancel_on_leave)
        current = tabs.currentWidget()
        self._current_key: int | None = id(current) if current is not None else None
        self._timer = QTimer(self)
        self._timer.setInterval(poll_interval_ms)
        self._timer.timeout.connect(self._drain)
        tabs.currentChanged.connect(self._on_current_changed)
        # A non-QObject weakref closure also works while the native parent is
        # being destroyed; cleanup deliberately does not invoke Qt methods.
        ref = weakref.ref(self)
        tabs.destroyed.connect(
            lambda *_: ref() is not None and ref()._shutdown()
        )

    def _require_gui(self) -> None:
        if self._closed or not Shiboken.isValid(self._tabs):
            raise RuntimeError("AsyncLazyTabs has been closed or destroyed")
        if QThread.currentThread() != self.thread():
            raise RuntimeError("AsyncLazyTabs methods must run on the Qt GUI thread")

    def _page(self, index: int) -> QWidget:
        self._require_gui()
        if index < 0 or index >= self._tabs.count():
            raise IndexError(f"Tab index out of range: {index}")
        return self._tabs.widget(index)

    def _entry(self, index: int) -> _AsyncPage | None:
        page = self._page(index)
        entry = self._pages.get(id(page))
        return entry if entry and entry.placeholder is page else None

    @staticmethod
    def _cancel(entry: _AsyncPage) -> bool:
        if entry.future is None:
            return False
        entry.generation += 1
        if entry.cancellation is not None:
            entry.cancellation.set()
        entry.future.cancel()
        entry.future = None
        entry.cancellation = None
        return True

    def _discard(self, key: int) -> None:
        entry = self._pages.pop(key, None)
        if entry is not None:
            self._cancel(entry)
        if self._current_key == key:
            self._current_key = None

    def _prune(self) -> None:
        if self._closed or not Shiboken.isValid(self._tabs):
            return
        for key, entry in tuple(self._pages.items()):
            if (not Shiboken.isValid(entry.placeholder)
                    or self._tabs.indexOf(entry.placeholder) < 0):
                self._discard(key)

    def addAsyncTab(self, label: str, loader: Callable[[Event], Any],
                    renderer: Callable[[Any], QWidget],
                    *, route: str | None = None) -> int:
        self._require_gui()
        if not callable(loader) or not callable(renderer):
            raise TypeError("loader and renderer must be callable")
        placeholder = QWidget()
        index = self._tabs.addTab(placeholder, label)
        if index < 0:
            raise RuntimeError("QTabWidget rejected the async tab")
        if route is not None:
            self._tabs.setRoute(index, route)
        self.registerAsyncTab(index, loader, renderer)
        return index

    def registerAsyncTab(self, index: int, loader: Callable[[Event], Any],
                         renderer: Callable[[Any], QWidget]) -> None:
        if not callable(loader) or not callable(renderer):
            raise TypeError("loader and renderer must be callable")
        page = self._page(index)
        if page.layout() is not None:
            raise ValueError("Async placeholder must have no existing layout")
        self._prune()
        key = id(page)
        if key in self._pages:
            raise ValueError("This tab already has an async loader")
        self._pages[key] = _AsyncPage(page, loader, renderer)
        ref = weakref.ref(self)
        page.destroyed.connect(
            lambda *_args, token=key: ref() is not None and ref()._discard(token)
        )
        if index == self._tabs.currentIndex():
            self._current_key = key
            self.requestPage(index)

    def cancelPage(self, index: int) -> bool:
        entry = self._entry(index)
        if entry is None:
            return False
        if not self._cancel(entry):
            return False
        self.pageCancelled.emit(index)
        return True

    def unregisterAsyncTab(self, index: int) -> bool:
        key = id(self._page(index))
        if key not in self._pages:
            return False
        self._discard(key)
        return True

    def removeAsyncTab(self, index: int) -> QWidget:
        page = self._page(index)
        self._discard(id(page))
        self._tabs.removeTab(index)
        return page

    def isLoading(self, index: int) -> bool:
        entry = self._entry(index)
        return bool(entry and entry.future is not None)

    def isReady(self, index: int) -> bool:
        entry = self._entry(index)
        return bool(entry and entry.content is not None
                    and Shiboken.isValid(entry.content))

    def hasCachedData(self, index: int) -> bool:
        """Check whether preloaded data awaits GUI-thread widget rendering."""
        entry = self._entry(index)
        return bool(entry is not None and entry.payload is not _MISSING
                    and entry.content is None)

    def evictCachedData(self, index: int) -> bool:
        """Discard inactive, finished plain data; never destroy a QWidget."""
        entry = self._entry(index)
        if (entry is None or entry.payload is _MISSING
                or entry.future is not None or entry.content is not None
                or self._tabs.currentIndex() == index):
            return False
        entry.payload = _MISSING
        return True

    def lastError(self, index: int) -> str | None:
        entry = self._entry(index)
        return entry.error if entry else None

    def requestPage(self, index: int) -> None:
        """Start/retry a loader; finished data renders when its tab is active."""
        entry = self._entry(index)
        if entry is None or entry.future is not None or self.isReady(index):
            return
        if entry.payload is not _MISSING:
            if self._tabs.currentIndex() == index:
                self._render(entry)
            return
        entry.error = None
        entry.generation += 1
        generation = entry.generation
        event = Event()
        entry.cancellation = event
        self._timer.start()
        try:
            entry.future = self._pool.submit(
                _execute, id(entry.placeholder), generation, entry.loader,
                event, self._results
            )
        except Exception:
            entry.cancellation = None
            entry.future = None
            entry.error = traceback.format_exc()
            self.loadFailed.emit(index, entry.error)

    def _render(self, entry: _AsyncPage) -> None:
        if self._closed or not Shiboken.isValid(self._tabs):
            return
        if not Shiboken.isValid(entry.placeholder):
            return
        index = self._tabs.indexOf(entry.placeholder)
        if index < 0 or self._tabs.currentIndex() != index:
            return
        if entry.payload is _MISSING or self.isReady(index):
            return
        try:
            if isinstance(entry.payload, QObject):
                raise TypeError("Worker loader must return plain data, never QObject")
            content = entry.renderer(entry.payload)
            if not isinstance(content, QWidget) or not Shiboken.isValid(content):
                raise TypeError("GUI renderer must return a live QWidget")
            if content is entry.placeholder:
                raise ValueError("Renderer cannot return the placeholder")
            parent = content.parentWidget()
            if parent is not None and parent is not entry.placeholder:
                raise ValueError("Renderer returned a widget from another owner")
            if (not Shiboken.isValid(self._tabs)
                    or not Shiboken.isValid(entry.placeholder)
                    or self._pages.get(id(entry.placeholder)) is not entry
                    or self._tabs.indexOf(entry.placeholder) < 0):
                return
            layout = QVBoxLayout(entry.placeholder)
            layout.setContentsMargins(0, 0, 0, 0)
            layout.setSpacing(0)
            layout.addWidget(content)
            entry.content = content
            entry.payload = _MISSING
            entry.error = None
            self.pageReady.emit(self._tabs.indexOf(entry.placeholder), content)
        except Exception:
            entry.error = traceback.format_exc()
            if Shiboken.isValid(self):
                self.loadFailed.emit(index, entry.error)

    def _drain(self) -> None:
        if self._closed or not Shiboken.isValid(self._tabs):
            return
        self._prune()
        while True:
            try:
                key, generation, payload, error = self._results.get_nowait()
            except Empty:
                break
            entry = self._pages.get(key)
            if (entry is None or entry.generation != generation
                    or entry.future is None):
                continue
            entry.future = None
            entry.cancellation = None
            if not Shiboken.isValid(entry.placeholder):
                self._discard(key)
                continue
            index = self._tabs.indexOf(entry.placeholder)
            if index < 0:
                self._discard(key)
                continue
            if error is not None:
                entry.error = error
                self.loadFailed.emit(index, error)
            else:
                entry.payload = payload
                self.dataReady.emit(index)
                # The receiver may have evicted cached data or removed the
                # tab. Never render a stale entry after signal callbacks.
                if (self._pages.get(key) is entry
                        and Shiboken.isValid(entry.placeholder)
                        and entry.payload is not _MISSING):
                    self._render(entry)
        if not any(entry.future is not None for entry in self._pages.values()):
            self._timer.stop()

    def _on_current_changed(self, index: int) -> None:
        if self._closed or not Shiboken.isValid(self._tabs):
            return
        self._prune()
        key = id(self._tabs.widget(index)) if index >= 0 else None
        if self._cancel_on_leave and self._current_key != key:
            old = self._pages.get(self._current_key)
            if old is not None and self._cancel(old):
                former_index = self._tabs.indexOf(old.placeholder)
                if former_index >= 0:
                    self.pageCancelled.emit(former_index)
        self._current_key = key
        if index >= 0:
            self.requestPage(index)

    def _shutdown(self) -> None:
        """Pure-Python cleanup; safe even from QObject.destroyed callbacks."""
        if self._closed:
            return
        self._closed = True
        for entry in self._pages.values():
            self._cancel(entry)
        self._pages.clear()
        self._pool.shutdown(wait=False, cancel_futures=True)

    def close(self) -> None:
        """Explicit nonblocking shutdown: cancel tasks, release callbacks."""
        self._require_gui()
        self._timer.stop()
        self._shutdown()
