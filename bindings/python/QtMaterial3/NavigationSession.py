"""Route-based navigation history, JSON session state, and bounded prefetch.

NavigationSession works with native QtMaterialTabs and AsyncLazyTabs. History
and persisted data are identified by stable QtMaterialRoute paths, never
mutable tab positions or live QWidget pointers.
"""
from __future__ import annotations

import json
from typing import Any

from PySide6.QtCore import QObject, QThread, QTimer, Signal
from shiboken6 import Shiboken

from .AsyncLazyTabs import AsyncLazyTabs
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

    def __init__(self, pages: AsyncLazyTabs, *, prefetch_radius: int = 1,
                 max_pending: int = 2, max_history: int = 100):
        if not isinstance(pages, AsyncLazyTabs) or not Shiboken.isValid(pages):
            raise TypeError("NavigationSession requires live AsyncLazyTabs")
        if isinstance(prefetch_radius, bool) or not 0 <= prefetch_radius <= 5:
            raise ValueError("prefetch_radius must be 0..5")
        if isinstance(max_pending, bool) or not 1 <= max_pending <= 8:
            raise ValueError("max_pending must be 1..8")
        if isinstance(max_history, bool) or not 1 <= max_history <= 1000:
            raise ValueError("max_history must be 1..1000")
        tabs = pages.parent()
        if not isinstance(tabs, QtMaterialTabs) or not Shiboken.isValid(tabs):
            raise TypeError("AsyncLazyTabs must be parented to QtMaterialTabs")

        super().__init__(tabs)
        self._tabs = tabs
        self._pages = pages
        self._radius = prefetch_radius
        self._limit = max_pending
        self._max_history = max_history
        self._history: list[str] = []
        self._cursor = -1
        self._states: dict[str, Any] = {}
        self._restoring = False
        self._closed = False

        self._prefetch_timer = QTimer(self)
        self._prefetch_timer.setSingleShot(True)
        self._prefetch_timer.timeout.connect(self.prefetchNeighbors)
        tabs.currentChanged.connect(self._on_current_changed)
        tabs.routeChanged.connect(self._on_route_changed)
        self._record_current()

    def _require_gui(self) -> None:
        if (self._closed or not Shiboken.isValid(self._tabs)
                or not Shiboken.isValid(self._pages)):
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
        self._record_current()

    def _on_route_changed(self, index: int, _route: object) -> None:
        if index == self._tabs.currentIndex():
            self._record_current()

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
            self._tabs.setCurrentIndex(index)
        else:
            self._record_current()
        return True

    def _travel(self, direction: int) -> bool:
        self._require_gui()
        target = self._cursor + direction
        while 0 <= target < len(self._history):
            index = self._index(self._history[target])
            if index >= 0:
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
        if self._radius == 0:
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

    def setPageState(self, route: str, value: Any) -> None:
        self._require_gui()
        path = self._normalize(route)
        if self._index(path) < 0:
            raise ValueError("Cannot store state for an unknown or duplicate route")
        new_state = dict(self._states)
        new_state[path] = _json_copy(value)
        _json_copy(new_state)
        self._states = new_state

    def pageState(self, route: str, default: Any = None) -> Any:
        self._require_gui()
        path = self._normalize(route)
        return _json_copy(self._states[path]) if path in self._states else default

    def saveState(self) -> dict[str, Any]:
        self._require_gui()
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
        if not isinstance(data, dict) or data.get("version") != 1:
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
        self._schedule_prefetch()
        return bool(target)

    def close(self) -> None:
        self._require_gui()
        self._closed = True
        self._prefetch_timer.stop()
        self._history.clear()
        self._states.clear()
