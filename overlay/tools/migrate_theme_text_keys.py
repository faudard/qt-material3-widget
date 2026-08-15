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
    'floatingActionButton': 'FloatingActionButton',
    'FloatingActionButton': 'FloatingActionButton',
    'fab.small': 'SmallFloatingActionButton',
    'SmallFab': 'SmallFloatingActionButton',
    'fab.large': 'LargeFloatingActionButton',
    'LargeFab': 'LargeFloatingActionButton',
    'fab.extended': 'ExtendedFloatingActionButton',
    'ExtendedFab': 'ExtendedFloatingActionButton',
    'ExtendedFloatingActionButton': 'ExtendedFloatingActionButton',
    'selection': 'Custom',
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
}

# Umbrella aliases may be represented by typed base IDs or omitted if they
# resolve to Custom and a more specific first-party ID follows.
SKIP_ENUM = {"Custom"}

LIST_RE = re.compile(
    r"QStringList\s*\{(?P<body>(?:\s*QStringLiteral\s*\(\s*\"[^\"]+\"\s*\)\s*,?)+)\s*\}",
    re.MULTILINE,
)
LITERAL_RE = re.compile(r'QStringLiteral\s*\(\s*"([^"]+)"\s*\)')


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

    return LIST_RE.sub(replace, text), unknown


def migrate_file(path: Path, apply: bool) -> tuple[bool, list[str]]:
    text = path.read_text(encoding="utf-8")
    new_text, unknown = convert_text(text)
    if unknown:
        return False, unknown
    changed = new_text != text
    if changed:
        new_text = new_text.replace("#include <QStringList>", "#include <QVector>")
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
    for path in sorted((root/"src/specs").glob("*specresolver.cpp")):
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
