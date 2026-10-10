"""Explicit, bounded and non-sensitive Qt Widgets form snapshot helpers.

Only supported, named inputs are saved. Password fields, fields with a
navigationPersist=false dynamic property, and sensitive object names are
never persisted. Snapshots are JSON-compatible and contain no QObject.
"""
from __future__ import annotations

import re
from typing import Any

from PySide6.QtWidgets import (
    QCheckBox, QComboBox, QDoubleSpinBox, QLineEdit, QPlainTextEdit,
    QRadioButton, QSlider, QSpinBox, QTextEdit, QWidget,
)
from shiboken6 import Shiboken


_SENSITIVE = re.compile(r"pass(?:word)?|secret|token|credential|api[_-]?key|auth", re.I)
_VERSION = 1
_MAX_FIELDS = 256


def _eligible(widget: QWidget) -> bool:
    name = widget.objectName()
    if not name or _SENSITIVE.search(name):
        return False
    if widget.property("navigationPersist") is False:
        return False
    if isinstance(widget, QLineEdit) and widget.echoMode() != QLineEdit.Normal:
        return False
    return True


def _read(widget: QWidget) -> dict[str, Any] | None:
    if isinstance(widget, QLineEdit):
        return {"type": "line", "value": widget.text()}
    if isinstance(widget, QCheckBox):
        return {"type": "check", "value": widget.checkState().value}
    if isinstance(widget, QRadioButton):
        return {"type": "radio", "value": widget.isChecked()}
    if isinstance(widget, QComboBox):
        return {"type": "combo", "value": widget.currentIndex()}
    if isinstance(widget, QDoubleSpinBox):
        return {"type": "double", "value": widget.value()}
    if isinstance(widget, QSpinBox):
        return {"type": "spin", "value": widget.value()}
    if isinstance(widget, QSlider):
        return {"type": "slider", "value": widget.value()}
    if isinstance(widget, QPlainTextEdit):
        return {"type": "plain", "value": widget.toPlainText()}
    if isinstance(widget, QTextEdit):
        return {"type": "text", "value": widget.toPlainText()}
    return None


def _write(widget: QWidget, record: dict[str, Any]) -> bool:
    if not isinstance(record, dict) or not isinstance(record.get("type"), str):
        return False
    value = record.get("value")
    kind = record["type"]
    if isinstance(widget, QLineEdit) and kind == "line" and isinstance(value, str):
        widget.setText(value)
    elif isinstance(widget, QCheckBox) and kind == "check" and type(value) is int and 0 <= value <= 2:
        from PySide6.QtCore import Qt
        widget.setCheckState(Qt.CheckState(value))
    elif isinstance(widget, QRadioButton) and kind == "radio" and type(value) is bool:
        widget.setChecked(value)
    elif isinstance(widget, QComboBox) and kind == "combo" and type(value) is int and -1 <= value < widget.count():
        widget.setCurrentIndex(value)
    elif isinstance(widget, QDoubleSpinBox) and kind == "double" and type(value) in (int, float):
        widget.setValue(value)
    elif isinstance(widget, QSpinBox) and kind == "spin" and type(value) is int:
        widget.setValue(value)
    elif isinstance(widget, QSlider) and kind == "slider" and type(value) is int:
        widget.setValue(value)
    elif isinstance(widget, QPlainTextEdit) and kind == "plain" and isinstance(value, str):
        widget.setPlainText(value)
    elif isinstance(widget, QTextEdit) and kind == "text" and isinstance(value, str):
        widget.setPlainText(value)
    else:
        return False
    return True


def snapshotForm(root: QWidget) -> dict[str, Any]:
    """Capture uniquely named, non-secret inputs under a live Qt widget."""
    if not isinstance(root, QWidget) or not Shiboken.isValid(root):
        raise TypeError("snapshotForm requires a live QWidget")
    elements = [root, *root.findChildren(QWidget)]
    fields: dict[str, dict[str, Any]] = {}
    duplicates: set[str] = set()
    for widget in elements:
        if not _eligible(widget):
            continue
        name = widget.objectName()
        record = _read(widget)
        if record is None:
            continue
        if name in fields:
            duplicates.add(name)
        elif len(fields) >= _MAX_FIELDS:
            raise ValueError("Form has more than 256 persistable fields")
        else:
            fields[name] = record
    for name in duplicates:
        fields.pop(name, None)
    return {"version": _VERSION, "fields": fields}


def restoreForm(root: QWidget, state: dict[str, Any]) -> int:
    """Restore known and eligible inputs by objectName and matching type.

    Unknown fields and mismatched types are ignored. No child widgets are
    created or deleted; ownership and layout are never modified.
    """
    if not isinstance(root, QWidget) or not Shiboken.isValid(root):
        raise TypeError("restoreForm requires a live QWidget")
    if (not isinstance(state, dict) or type(state.get("version")) is not int
            or state["version"] != _VERSION or not isinstance(state.get("fields"), dict)
            or len(state["fields"]) > _MAX_FIELDS):
        raise ValueError("Unsupported or malformed form snapshot")
    targets: dict[str, QWidget] = {}
    duplicates: set[str] = set()
    for widget in [root, *root.findChildren(QWidget)]:
        if not _eligible(widget) or _read(widget) is None:
            continue
        name = widget.objectName()
        if name in targets:
            duplicates.add(name)
        else:
            targets[name] = widget
    restored = 0
    for name, value in state["fields"].items():
        if not isinstance(name, str):
            raise ValueError("Form field name must be a string")
        widget = targets.get(name)
        if name not in duplicates and widget is not None and _write(widget, value):
            restored += 1
    return restored
