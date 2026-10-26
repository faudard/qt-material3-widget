"""Typed adapters for native, versioned QtMaterial workspace snapshots.

The C++ widget implementation remains the only serializer and validator.
A view's model, delegate, actions, QObject pointers and runtime data are not
included in its snapshot. Restore only after installing a compatible model.
"""
from __future__ import annotations

from typing import Protocol

from PySide6.QtCore import QByteArray


class WorkspaceStateful(Protocol):
    """A native QtMaterial view exposing its C++ workspace-state contract."""

    def saveWorkspaceState(self) -> QByteArray: ...
    def restoreWorkspaceState(self, state: QByteArray) -> bool: ...


def save_workspace_state(view: WorkspaceStateful) -> bytes:
    """Return the native versioned snapshot as transportable Python bytes."""
    return bytes(view.saveWorkspaceState())


def restore_workspace_state(view: WorkspaceStateful, snapshot: bytes) -> bool:
    """Ask the native view to validate and restore a snapshot, fail closed."""
    if not isinstance(snapshot, bytes):
        raise TypeError("workspace snapshot must be bytes")
    if not snapshot:
        return False
    return bool(view.restoreWorkspaceState(QByteArray(snapshot)))
