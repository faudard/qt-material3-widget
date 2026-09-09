#!/usr/bin/env python3
"""Migrate component override lookups from textual aliases to ComponentId.

The migration is deliberately fail-closed: any unknown literal aborts instead
of silently mapping it to a generic component.
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path
from typing import Sequence

ALIASES = {
    'button': 'Button',
    'Button': 'Button',
    'button.text': 'ButtonText',
    'TextButton': 'ButtonText',
    'button.filled': 'ButtonFilled',
    'FilledButton': 'ButtonFilled',
    'button.filledTonal': 'ButtonFilledTonal',
    'FilledTonalButton': 'ButtonFilledTonal',
    'button.outlined': 'ButtonOutlined',
    'OutlinedButton': 'ButtonOutlined',
    'button.elevated': 'ButtonElevated',
    'ElevatedButton': 'ButtonElevated',
    'iconButton': 'IconButton',
    'IconButton': 'IconButton',
    'iconButton.standard': 'IconButtonStandard',
    'StandardIconButton': 'IconButtonStandard',
    'iconButton.filled': 'IconButtonFilled',
    'FilledIconButton': 'IconButtonFilled',
    'iconButton.filledTonal': 'IconButtonFilledTonal',
    'FilledTonalIconButton': 'IconButtonFilledTonal',
    'iconButton.outlined': 'IconButtonOutlined',
    'OutlinedIconButton': 'IconButtonOutlined',
    'fab': 'FloatingActionButton',
    'Fab': 'FloatingActionButton',
    'floatingActionButton': 'FloatingActionButton',
    'FloatingActionButton': 'FloatingActionButton',
    'fab.small': 'SmallFloatingActionButton',
    'SmallFab': 'SmallFloatingActionButton',
    'fab.large': 'LargeFloatingActionButton',
    'LargeFab': 'LargeFloatingActionButton',
    'fab.extended': 'ExtendedFloatingActionButton',
    'ExtendedFab': 'ExtendedFloatingActionButton',
    'ExtendedFloatingActionButton': 'ExtendedFloatingActionButton',
    'selection': 'Selection',
    'checkbox': 'Checkbox',
    'Checkbox': 'Checkbox',
    'radio': 'RadioButton',
    'radioButton': 'RadioButton',
    'RadioButton': 'RadioButton',
    'switch': 'Switch',
    'Switch': 'Switch',
    'dialog': 'Dialog',
    'Dialog': 'Dialog',
    'navigationDrawer': 'NavigationDrawer',
    'NavigationDrawer': 'NavigationDrawer',
    'bottomSheet': 'BottomSheet',
    'BottomSheet': 'BottomSheet',
    'banner': 'Banner',
    'Banner': 'Banner',
    'card': 'Card',
    'Card': 'Card',
    'topAppBar': 'TopAppBar',
    'TopAppBar': 'TopAppBar',
    'bottomAppBar': 'BottomAppBar',
    'BottomAppBar': 'BottomAppBar',
    'input': 'Input',
    'textField': 'TextField',
    'TextField': 'TextField',
    'textField.outlined': 'TextFieldOutlined',
    'OutlinedTextField': 'TextFieldOutlined',
    'textField.filled': 'TextFieldFilled',
    'FilledTextField': 'TextFieldFilled',
    'autocomplete': 'AutoComplete',
    'Autocomplete': 'AutoComplete',
    'AutoComplete': 'AutoComplete',
    'autocompletePopup': 'AutoCompletePopup',
    'AutocompletePopup': 'AutoCompletePopup',
    'dateField': 'DateField',
    'DateField': 'DateField',
    'list': 'List',
    'List': 'List',
    'listItem': 'ListItem',
    'ListItem': 'ListItem',
    'divider': 'Divider',
    'Divider': 'Divider',
    'tabs': 'Tabs',
    'Tabs': 'Tabs',
    'snackbar': 'Snackbar',
    'Snackbar': 'Snackbar',
    'progressIndicator': 'ProgressIndicator',
    'ProgressIndicator': 'ProgressIndicator',
    'navigationRail': 'NavigationRail',
    'NavigationRail': 'NavigationRail',
    'surface': 'Surface',
    'Surface': 'Surface',

    'data': 'Data',
    'table': 'Table',
    'Table': 'Table',
    'gridList': 'GridList',
    'GridList': 'GridList',
    'carousel': 'Carousel',
    'Carousel': 'Carousel',
    'datePicker': 'DatePicker',
    'DatePicker': 'DatePicker',

    'navigation': 'Navigation',

    'chip': 'Chip',
    'Chip': 'Chip',
    'chip.assist': 'AssistChip',
    'AssistChip': 'AssistChip',
    'chip.filter': 'FilterChip',
    'FilterChip': 'FilterChip',
    'chip.input': 'InputChip',
    'InputChip': 'InputChip',
    'chip.suggestion': 'SuggestionChip',
    'SuggestionChip': 'SuggestionChip',

    'menu': 'Menu',
    'Menu': 'Menu',
    'segmentedButton': 'SegmentedButton',
    'SegmentedButton': 'SegmentedButton',
}

# Custom remains reserved for opaque third-party serialized names. First-party
# umbrella keys (selection/data/navigation/...) always have a real typed ID so
# family-level overrides keep their precedence semantics.
SKIP_ENUM = {"Custom"}

LIST_RE = re.compile(
    r"QStringList\s*\{(?P<body>(?:\s*QStringLiteral\s*\(\s*\"[^\"]+\"\s*\)\s*,?)+)\s*\}",
    re.MULTILINE,
)
LITERAL_RE = re.compile(r'QStringLiteral\s*\(\s*"([^"]+)"\s*\)')
COMPONENT_OVERRIDE_CALL_RE = re.compile(
    r"(?P<prefix>\bcomponentOverrides\s*\(\s*\)\s*\.\s*"
    r"(?:setOverride|contains|overrideFor|removeOverride)\s*\(\s*)"
    r"QStringLiteral\s*\(\s*\"(?P<name>[^\"]+)\"\s*\)",
    re.MULTILINE,
)

SOURCE_ROOTS = ("include", "src", "tests", "examples")
SOURCE_SUFFIXES = {".h", ".hh", ".hpp", ".cpp", ".cc", ".cxx"}


def convert_text(text: str) -> tuple[str, list[str]]:
    unknown: list[str] = []

    def replace(match: re.Match[str]) -> str:
        names = LITERAL_RE.findall(match.group("body"))
        enums: list[str] = []
        for name in names:
            enum = ALIASES.get(name)
            if enum is None:
                unknown.append(name)
                continue
            if enum in SKIP_ENUM:
                continue
            token = f"ComponentId::{enum}"
            if token not in enums:
                enums.append(token)
        if not enums:
            return match.group(0)
        return "QVector<ComponentId>{ " + ", ".join(enums) + " }"

    converted = LIST_RE.sub(replace, text)

    def replace_component_override_call(match: re.Match[str]) -> str:
        name = match.group("name")
        enum = ALIASES.get(name)
        if enum is None or enum in SKIP_ENUM:
            unknown.append(name)
            return match.group(0)
        return match.group("prefix") + f"ComponentId::{enum}"

    converted = COMPONENT_OVERRIDE_CALL_RE.sub(
        replace_component_override_call,
        converted,
    )
    return converted, unknown


def migrate_file(path: Path, apply: bool) -> tuple[bool, list[str]]:
    text = path.read_text(encoding="utf-8")
    new_text, unknown = convert_text(text)
    if unknown:
        return False, unknown
    changed = new_text != text
    if changed:
        new_text = new_text.replace("#include <QStringList>", "#include <QVector>")
        if "QVector<ComponentId>" in new_text and "#include <QVector>" not in new_text:
            include_matches = list(re.finditer(r'^#include[^\n]*$', new_text, re.MULTILINE))
            if include_matches:
                pos = include_matches[-1].end()
                new_text = new_text[:pos] + "\n#include <QVector>" + new_text[pos:]
            else:
                new_text = "#include <QVector>\n" + new_text
        if apply:
            path.write_text(new_text, encoding="utf-8")
    return changed, []


def main(argv: Sequence[str] | None = None) -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--root", type=Path, default=Path("."))
    p.add_argument("--apply", action="store_true")
    args = p.parse_args()

    root = args.root.resolve()
    failures = []
    changed = []
    source_files = {
        path
        for base_name in SOURCE_ROOTS
        for path in (root / base_name).rglob("*")
        if path.is_file() and path.suffix.lower() in SOURCE_SUFFIXES
    }
    for path in sorted(source_files):
        did_change, unknown = migrate_file(path, args.apply)
        if unknown:
            failures.append((path, unknown))
        elif did_change:
            changed.append(path)

    if failures:
        print("Typed component-key migration FAILED:")
        for path, names in failures:
            print(f" - {path.relative_to(root)} unknown aliases: {', '.join(names)}")
        return 1

    for path in changed:
        print(("MIGRATED" if args.apply else "WOULD MIGRATE"),
              path.relative_to(root))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
