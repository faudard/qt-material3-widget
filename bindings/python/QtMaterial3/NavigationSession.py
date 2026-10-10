"""Route-based navigation history, JSON session state, and bounded prefetch.

NavigationSession works with native QtMaterialTabs and AsyncLazyTabs. History
and persisted data are identified by stable QtMaterialRoute paths, never
mutable tab positions or live QWidget pointers.
"""
from __future__ import annotations

import json
import traceback
from typing import Any, Callable

from PySide6.QtCore import QObject, QThread, QTimer, Signal
from shiboken6 import Shiboken

from .AsyncLazyTabs import AsyncLazyTabs
from .FormState import snapshotForm, restoreForm
from . import QtMaterialRoute
from .Widgets import QtMaterialTabs


def _json_copy(value: Any) -> Any:
    """Validate plain serializable state and detach callers' mutable objects."""
    data = json.dumps(value, allow_nan=False, ensure_ascii=False, sort_keys=True)
    if len(data.encode("utf-8")) > 262144:
        raise ValueError("Navigation state exceeds the 256 KiB limit")
    return json.loads(data)


class NavigationSession(QObject):
    """Manage route history and prefetch without owning page widgets.

    Bind a session to an existing AsyncLazyTabs manager (and its native Tabs).
    Only the GUI thread may access it. Background loaders must obey the
    AsyncLazyTabs plain-data/cancellation contract.
    """

    routeChanged = Signal(str)
    historyChanged = Signal(bool, bool)
    prefetchStarted = Signal(int)
    cacheEvicted = Signal(int)
    stateProviderFailed = Signal(str, str)

    def __init__(self, pages: AsyncLazyTabs, *, prefetch_radius: int = 1,
                 max_pending: int = 2, max_history: int = 100,
                 max_cached_pages: int = 4):
        if not isinstance(pages, AsyncLazyTabs) or not Shiboken.isValid(pages):
            raise TypeError("NavigationSession requires live AsyncLazyTabs")
        if isinstance(prefetch_radius, bool) or not 0 <= prefetch_radius <= 5:
            raise ValueError("prefetch_radius must be 0..5")
        if isinstance(max_pending, bool) or not 1 <= max_pending <= 8:
            raise ValueError("max_pending must be 1..8")
        if isinstance(max_history, bool) or not 1 <= max_history <= 1000:
            raise ValueError("max_history must be 1..1000")
        if type(max_cached_pages) is not int or not 0 <= max_cached_pages <= 64:
            raise ValueError("max_cached_pages must be 0..64")
        tabs = pages.parent()
        if not isinstance(tabs, QtMaterialTabs) or not Shiboken.isValid(tabs):
            raise TypeError("AsyncLazyTabs must be parented to QtMaterialTabs")

        super().__init__(tabs)
        self._tabs = tabs
        self._pages = pages
        self._radius = prefetch_radius
        self._limit = max_pending
        self._max_history = max_history
        self._max_cached_pages = max_cached_pages
        self._cached_keys: list[int] = []
        self._providers: dict[str, tuple[Callable, Callable]] = {}
        self._applied: dict[str, int] = {}
        self._history: list[str] = []
        self._cursor = -1
        self._states: dict[str, Any] = {}
        self._restoring = False
        self._closed = False

        self._prefetch_timer = QTimer(self)
        self._prefetch_timer.setSingleShot(True)
        self._prefetch_timer.timeout.connect(self.prefetchNeighbors)
        # QTabWidget may emit currentChanged *before* QtMaterialTabs has
        # finished reindexing its descriptors on insert/remove. Coalesce
        # these notifications until the native operation has settled.
        self._sync_timer = QTimer(self)
        self._sync_timer.setSingleShot(True)
        self._sync_timer.timeout.connect(self._record_current)
        tabs.currentChanged.connect(self._on_current_changed)
        tabs.currentRouteChanged.connect(self._on_current_route_changed)
        tabs.routeChanged.connect(self._on_route_changed)
        pages.dataReady.connect(self._on_data_ready)
        pages.pageReady.connect(self._on_page_ready)
        self._record_current()

    def _require_gui(self) -> None:
        if (self._closed or not Shiboken.isValid(self._tabs)
                or not Shiboken.isValid(self._pages)
                or getattr(self._pages, "_closed", False)):
            raise RuntimeError("NavigationSession has been closed or destroyed")
        if QThread.currentThread() != self.thread():
            raise RuntimeError("NavigationSession is restricted to the Qt GUI thread")

    def _route(self, index: int) -> str:
        if index < 0 or index >= self._tabs.count():
            return ""
        return self._tabs.route(index).path()

    def _normalize(self, path: str) -> str:
        if not isinstance(path, str):
            raise TypeError("Route must be a string")
        normalized = QtMaterialRoute(path).path()
        if not normalized:
            raise ValueError("Empty navigation route")
        return normalized

    def _index(self, path: str) -> int:
        if not path:
            return -1
        matches = [
            index for index in range(self._tabs.count())
            if self._route(index) == path
        ]
        return matches[0] if len(matches) == 1 else -1

    def _record_current(self) -> None:
        if self._restoring or self._closed or not Shiboken.isValid(self._tabs):
            return
        route = self._route(self._tabs.currentIndex())
        if not route or self._index(route) < 0:
            return
        if self._cursor >= 0 and self._history[self._cursor] == route:
            self._schedule_prefetch()
            return
        self._history = self._history[:self._cursor + 1]
        self._history.append(route)
        if len(self._history) > self._max_history:
            self._history = self._history[-self._max_history:]
        self._cursor = len(self._history) - 1
        self.routeChanged.emit(route)
        self.historyChanged.emit(self.canGoBack(), self.canGoForward())
        self._schedule_prefetch()

    def _on_current_changed(self, _index: int) -> None:
        if not self._restoring and not self._closed:
            if 0 <= self._cursor < len(self._history):
                self._capture_route(self._history[self._cursor])
            self._sync_timer.start(0)

    def _on_current_route_changed(self, _route: object) -> None:
        if not self._restoring and not self._closed:
            self._sync_timer.start(0)

    def _on_route_changed(self, index: int, _route: object) -> None:
        if index == self._tabs.currentIndex() and not self._restoring:
            self._sync_timer.start(0)

    def _schedule_prefetch(self) -> None:
        if self._radius and not self._restoring and not self._closed:
            self._prefetch_timer.start(0)

    def currentRoute(self) -> str:
        self._require_gui()
        return self._route(self._tabs.currentIndex())

    def history(self) -> list[str]:
        self._require_gui()
        return list(self._history)

    def canGoBack(self) -> bool:
        if self._closed or not Shiboken.isValid(self._tabs):
            return False
        return any(self._index(path) >= 0
                   for path in self._history[:max(0, self._cursor)])

    def canGoForward(self) -> bool:
        if self._closed or not Shiboken.isValid(self._tabs):
            return False
        return any(self._index(path) >= 0
                   for path in self._history[self._cursor + 1:])

    def navigate(self, route: str) -> bool:
        self._require_gui()
        index = self._index(self._normalize(route))
        if index < 0:
            return False
        if index != self._tabs.currentIndex():
            if 0 <= self._cursor < len(self._history):
                self._capture_route(self._history[self._cursor])
            self._tabs.setCurrentIndex(index)
        self._sync_timer.stop()
        self._record_current()
        return True

    def _travel(self, direction: int) -> bool:
        self._require_gui()
        target = self._cursor + direction
        while 0 <= target < len(self._history):
            index = self._index(self._history[target])
            if index >= 0:
                if 0 <= self._cursor < len(self._history):
                    self._capture_route(self._history[self._cursor])
                self._cursor = target
                self._restoring = True
                try:
                    self._tabs.setCurrentIndex(index)
                finally:
                    self._restoring = False
                self.routeChanged.emit(self._history[target])
                self.historyChanged.emit(self.canGoBack(), self.canGoForward())
                self._schedule_prefetch()
                return True
            target += direction
        return False

    def back(self) -> bool:
        return self._travel(-1)

    def forward(self) -> bool:
        return self._travel(1)

    def prefetchNeighbors(self) -> list[int]:
        """Bound pending data jobs; render only when AsyncLazyTabs activates tab."""
        self._require_gui()
        if self._radius == 0 or getattr(self._pages, "_closed", False):
            return []
        current = self._tabs.currentIndex()
        if current < 0:
            return []
        inflight = sum(self._pages.isLoading(i) for i in range(self._tabs.count()))
        started: list[int] = []
        for distance in range(1, self._radius + 1):
            for index in (current + distance, current - distance):
                if index < 0 or index >= self._tabs.count() or inflight >= self._limit:
                    continue
                if self._pages.isLoading(index) or self._pages.isReady(index):
                    continue
                self._pages.requestPage(index)
                if self._pages.isLoading(index):
                    inflight += 1
                    started.append(index)
                    self.prefetchStarted.emit(index)
        return started

    def _on_data_ready(self, index: int) -> None:
        """Keep only a bounded LRU of inactive, prefetched plain data."""
        if self._closed or not Shiboken.isValid(self._tabs):
            return
        if not 0 <= index < self._tabs.count():
            return
        page = self._tabs.widget(index)
        key = id(page)
        if key in self._cached_keys:
            self._cached_keys.remove(key)
        if index != self._tabs.currentIndex() and self._pages.hasCachedData(index):
            self._cached_keys.append(key)
        self._trim_cache()

    def _trim_cache(self) -> None:
        while len(self._cached_keys) > self._max_cached_pages:
            key = self._cached_keys.pop(0)
            for index in range(self._tabs.count()):
                if id(self._tabs.widget(index)) == key:
                    if self._pages.evictCachedData(index):
                        self.cacheEvicted.emit(index)
                    break

    def _on_page_ready(self, index: int, widget: object) -> None:
        if self._closed or not Shiboken.isValid(self._tabs):
            return
        key = id(self._tabs.widget(index)) if 0 <= index < self._tabs.count() else None
        if key in self._cached_keys:
            self._cached_keys.remove(key)
        path = self._route(index)
        if path:
            self._restore_route(path)

    def registerStateProvider(
        self, route: str, capture: Callable[[object], Any],
        apply: Callable[[object, Any], None],
    ) -> None:
        """Register optional GUI-thread state callbacks for a loaded page."""
        self._require_gui()
        path = self._normalize(route)
        if self._index(path) < 0:
            raise ValueError("Cannot register a missing or ambiguous route")
        if not callable(capture) or not callable(apply):
            raise TypeError("State provider callbacks must be callable")
        if path in self._providers:
            raise ValueError("State provider already registered")
        self._providers[path] = (capture, apply)
        self._restore_route(path)

    def registerForm(self, route: str) -> None:
        """Persist safe, named Qt input values through FormState helpers."""
        self.registerStateProvider(route, snapshotForm, restoreForm)

    def unregisterStateProvider(self, route: str) -> bool:
        self._require_gui()
        path = self._normalize(route)
        self._applied.pop(path, None)
        return self._providers.pop(path, None) is not None

    def _content_for_route(self, route: str):
        index = self._index(route)
        if index < 0 or not self._pages.isReady(index):
            return None
        entry = self._pages._entry(index)
        return entry.content if entry is not None else None

    def _capture_route(self, route: str) -> None:
        if self._closed or route not in self._providers:
            return
        content = self._content_for_route(route)
        if content is None:
            return
        try:
            value = self._providers[route][0](content)
            self.setPageState(route, value)
            # Capturing a current form must not replay it into the same
            # instance; it already contains the values we just captured.
            self._applied[route] = id(content)
        except Exception:
            self.stateProviderFailed.emit(route, traceback.format_exc())

    def _restore_route(self, route: str) -> None:
        if self._closed or route not in self._providers or route not in self._states:
            return
        content = self._content_for_route(route)
        if content is None or self._applied.get(route) == id(content):
            return
        try:
            self._providers[route][1](content, _json_copy(self._states[route]))
            self._applied[route] = id(content)
        except Exception:
            self.stateProviderFailed.emit(route, traceback.format_exc())

    def setPageState(self, route: str, value: Any) -> None:
        self._require_gui()
        path = self._normalize(route)
        if self._index(path) < 0:
            raise ValueError("Cannot store state for an unknown or duplicate route")
        new_state = dict(self._states)
        new_state[path] = _json_copy(value)
        _json_copy(new_state)
        self._states = new_state
        self._applied.pop(path, None)

    def pageState(self, route: str, default: Any = None) -> Any:
        self._require_gui()
        path = self._normalize(route)
        return _json_copy(self._states[path]) if path in self._states else default

    def saveState(self) -> dict[str, Any]:
        self._require_gui()
        for path in tuple(self._providers):
            self._capture_route(path)
        return _json_copy({
            "version": 1,
            "history": self._history,
            "cursor": self._cursor,
            "currentRoute": self.currentRoute(),
            "pages": self._states,
        })

    def restoreState(self, data: dict[str, Any] | str) -> bool:
        """Restore valid routes atomically; silently filter removed destinations."""
        self._require_gui()
        if isinstance(data, str):
            data = json.loads(data)
        data = _json_copy(data)
        if (not isinstance(data, dict)
                or type(data.get("version")) is not int
                or data["version"] != 1):
            raise ValueError("Unsupported navigation session schema")
        history = data.get("history")
        cursor = data.get("cursor")
        states = data.get("pages")
        current = data.get("currentRoute")
        if (not isinstance(history, list) or len(history) > 1000
                or not isinstance(cursor, int) or isinstance(cursor, bool)
                or not isinstance(states, dict) or not isinstance(current, str)):
            raise ValueError("Malformed navigation session")
        if history and not 0 <= cursor < len(history):
            raise ValueError("Cursor outside saved navigation history")
        if not history and cursor != -1:
            raise ValueError("Empty history requires cursor -1")
        if any(not isinstance(path, str) for path in history):
            raise ValueError("Navigation history must contain route strings")
        if any(not isinstance(path, str) for path in states):
            raise ValueError("Page state keys must be route strings")
        if len(data.get("history")) and data["history"][cursor] != current:
            raise ValueError("Saved cursor/currentRoute mismatch")
        # Filtering is intentional: newly deployed versions may remove pages.
        retained = []
        new_cursor = -1
        for pos, path in enumerate(history):
            if self._index(path) >= 0 and (not retained or retained[-1] != path):
                retained.append(path)
            if pos == cursor:
                new_cursor = len(retained) - 1
        if len(retained) > self._max_history:
            trim = len(retained) - self._max_history
            retained = retained[trim:]
            new_cursor = max(0, new_cursor - trim)
        normalized_states = {
            path: state for path, state in states.items()
            if self._index(path) >= 0
        }
        target = retained[new_cursor] if retained and new_cursor >= 0 else ""
        for path in tuple(self._providers):
            self._capture_route(path)
        self._restoring = True
        try:
            self._history = retained
            self._cursor = new_cursor
            self._states = normalized_states
            if target:
                self._tabs.setCurrentIndex(self._index(target))
        finally:
            self._restoring = False
        self.historyChanged.emit(self.canGoBack(), self.canGoForward())
        self.routeChanged.emit(self.currentRoute())
        for path in tuple(self._providers):
            self._restore_route(path)
        self._schedule_prefetch()
        return bool(target)

    def close(self) -> None:
        self._require_gui()
        self._closed = True
        self._prefetch_timer.stop()
        self._sync_timer.stop()
        self._history.clear()
        self._states.clear()
        self._cached_keys.clear()
        self._providers.clear()
        self._applied.clear()
