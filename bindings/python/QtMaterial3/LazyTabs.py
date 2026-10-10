"""Lifecycle-safe Python page factories for native QtMaterialTabs.

Shiboken's raw std::function<QWidget*()> signatures deliberately remain
unbound. This QObject helper makes the same user-visible lazy-page workflow
available through Qt's signal and QWidget ownership contracts instead.
"""
from __future__ import annotations

from dataclasses import dataclass
import traceback
from typing import Callable

from PySide6.QtCore import QObject, Signal
from PySide6.QtWidgets import QVBoxLayout, QWidget
from shiboken6 import Shiboken

from .Widgets import QtMaterialTabs


@dataclass
class _Page:
    placeholder: QWidget
    factory: Callable[[], QWidget]
    loading: bool = False
    content: QWidget | None = None
    error: str | None = None


class LazyTabs(QObject):
    """Manage deferred QWidget pages while QtMaterialTabs remains the owner.

    A factory must return a live QWidget without an unrelated QWidget parent.
    The first selected page is created immediately; all others are constructed
    on first selection. Failures are captured by loadFailed, not raised from
    a Qt signal callback; ensureLoaded() can be used to retry.
    """

    pageLoaded = Signal(int, QWidget)
    loadFailed = Signal(int, str)

    def __init__(self, tabs: QtMaterialTabs):
        if not isinstance(tabs, QtMaterialTabs) or not Shiboken.isValid(tabs):
            raise TypeError("LazyTabs requires a live QtMaterialTabs instance")
        super().__init__(tabs)
        self._tabs = tabs
        self._pages: dict[int, _Page] = {}
        tabs.currentChanged.connect(self._on_current_changed)

    @property
    def tabs(self) -> QtMaterialTabs:
        return self._tabs

    def _require_tabs(self) -> None:
        if not Shiboken.isValid(self._tabs):
            raise RuntimeError("QtMaterialTabs has already been destroyed")

    def _get_page(self, index: int) -> QWidget:
        self._require_tabs()
        if index < 0 or index >= self._tabs.count():
            raise IndexError(f"Tab index out of range: {index}")
        return self._tabs.widget(index)

    def _entry(self, index: int) -> _Page | None:
        page = self._get_page(index)
        entry = self._pages.get(id(page))
        return entry if entry is not None and entry.placeholder is page else None

    def _prune(self) -> None:
        for token, entry in tuple(self._pages.items()):
            if (not Shiboken.isValid(entry.placeholder)
                    or self._tabs.indexOf(entry.placeholder) < 0):
                self._pages.pop(token, None)

    def addLazyTab(self, label: str, factory: Callable[[], QWidget],
                   *, route: str | None = None) -> int:
        """Add an empty native tab and register a Python page factory."""
        if not callable(factory):
            raise TypeError("factory must be callable")
        self._require_tabs()
        placeholder = QWidget()
        index = self._tabs.addTab(placeholder, label)
        if index < 0:
            raise RuntimeError("QTabWidget rejected the lazy tab")
        if route is not None:
            self._tabs.setRoute(index, route)
        self.registerTab(index, factory)
        return index

    def registerTab(self, index: int, factory: Callable[[], QWidget]) -> None:
        """Register a factory for an existing EMPTY placeholder tab."""
        if not callable(factory):
            raise TypeError("factory must be callable")
        page = self._get_page(index)
        if page.layout() is not None:
            raise ValueError("Lazy tab placeholder must have no layout")
        self._prune()
        if id(page) in self._pages:
            raise ValueError("A factory is already registered for this tab")
        self._pages[id(page)] = _Page(page, factory)
        # Capture only the numeric key, never a QWidget Python wrapper.
        page.destroyed.connect(
            lambda *_args, token=id(page): self._pages.pop(token, None)
        )
        if index == self._tabs.currentIndex():
            self.ensureLoaded(index)

    def unregisterTab(self, index: int) -> bool:
        """Release the callback without changing native Qt page ownership."""
        page = self._get_page(index)
        return self._pages.pop(id(page), None) is not None

    def removeLazyTab(self, index: int) -> QWidget:
        """Unregister and remove a tab; caller retains its removed QWidget."""
        page = self._get_page(index)
        self._pages.pop(id(page), None)
        self._tabs.removeTab(index)
        return page

    def isLoaded(self, index: int) -> bool:
        entry = self._entry(index)
        return bool(entry is not None and entry.content is not None
                    and Shiboken.isValid(entry.content))

    def lastError(self, index: int) -> str | None:
        entry = self._entry(index)
        return None if entry is None else entry.error

    def ensureLoaded(self, index: int) -> QWidget | None:
        """Create page content once. Return None on failure or reentrancy."""
        entry = self._entry(index)
        if entry is None:
            return None
        if entry.content is not None and Shiboken.isValid(entry.content):
            return entry.content
        if entry.loading:
            return None
        entry.loading = True
        try:
            content = entry.factory()
            if not isinstance(content, QWidget) or not Shiboken.isValid(content):
                raise TypeError("Lazy page factory must return a live QWidget")
            if content is entry.placeholder:
                raise ValueError("Factory cannot return its own placeholder")
            parent = content.parentWidget()
            if parent is not None and parent is not entry.placeholder:
                raise ValueError("Factory returned a widget owned by another parent")
            # A factory can remove tabs, or even destroy the owning QWidget.
            # Never transfer the returned widget into a stale placeholder.
            if (not Shiboken.isValid(self._tabs)
                    or not Shiboken.isValid(entry.placeholder)
                    or self._pages.get(id(entry.placeholder)) is not entry
                    or self._tabs.indexOf(entry.placeholder) < 0):
                return None
            layout = entry.placeholder.layout()
            if layout is None:
                layout = QVBoxLayout(entry.placeholder)
                layout.setContentsMargins(0, 0, 0, 0)
                layout.setSpacing(0)
            layout.addWidget(content)
            entry.content = content
            entry.error = None
            self.pageLoaded.emit(self._tabs.indexOf(entry.placeholder), content)
            return content
        except Exception:
            entry.error = traceback.format_exc()
            if Shiboken.isValid(self):
                self.loadFailed.emit(index, entry.error)
            return None
        finally:
            entry.loading = False

    def _on_current_changed(self, index: int) -> None:
        self._prune()
        if index >= 0:
            self.ensureLoaded(index)
