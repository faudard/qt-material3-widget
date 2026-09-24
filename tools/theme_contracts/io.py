#!/usr/bin/env python3
"""Enforce ThemeIO ownership of textual theme identifiers."""

from __future__ import annotations

import argparse
import re
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[2]

MODEL_AND_SPECS = (
    "include/qtmaterial/theme/qtmaterialcomponenttokens.h",
    "src/theme/qtmaterialcomponenttokens.cpp",
    "include/qtmaterial/specs",
    "src/specs",
)
FORBIDDEN = (
    "componentIdToString",
    "componentIdFromString",
    "QStringList{",
    "const QString& componentName",
)
RUNTIME_IO_SYMBOLS = (
    "QJsonDocument",
    "ThemeSerializer",
    "ThemeReadMode",
    "exportThemeJson",
    "exportThemeToFile",
    "importThemeJson",
    "importThemeFromFile",
)

def iter_files(root: Path):
    for rel in MODEL_AND_SPECS:
        base = root/rel
        if base.is_file():
            yield base
        elif base.exists():
            for path in base.rglob("*"):
                if path.is_file() and path.suffix.lower() in {".h",".hpp",".cpp",".cc",".cxx"}:
                    yield path

def validate(root: Path = ROOT) -> list[str]:
    errors: list[str] = []

    codec_h = root/"include/qtmaterial/theme/private/qtmaterialthemetextcodec_p.h"
    codec_cpp = root/"src/theme/qtmaterialthemetextcodec.cpp"
    for path in (codec_h, codec_cpp):
        if not path.is_file():
            errors.append(f"missing ThemeIO codec: {path.relative_to(root)}")

    for path in iter_files(root):
        text = path.read_text(encoding="utf-8")
        for token in FORBIDDEN:
            if token in text:
                errors.append(f"{path.relative_to(root)} contains textual-key token {token}")

    manager_h = root/"include/qtmaterial/theme/qtmaterialthememanager.h"
    manager_cpp = root/"src/theme/qtmaterialthememanager.cpp"
    for path in (manager_h, manager_cpp):
        if not path.is_file():
            continue
        text = path.read_text(encoding="utf-8")
        for token in RUNTIME_IO_SYMBOLS:
            if token in text:
                errors.append(f"{path.relative_to(root)} still owns ThemeIO API/symbol {token}")

    component_h = root/"include/qtmaterial/theme/qtmaterialcomponenttokens.h"
    if component_h.is_file():
        text = component_h.read_text(encoding="utf-8")
        for required in (
            "bool contains(ComponentId componentId) const;",
            "QList<ComponentId> componentIds() const;",
            "friend class ThemeTextCodec;",
        ):
            if required not in text:
                errors.append(f"{component_h.relative_to(root)} missing {required}")

    serializer = root/"src/theme/qtmaterialthemeserializer.cpp"
    if serializer.is_file():
        text = serializer.read_text(encoding="utf-8")
        if "qtmaterialthemetextcodec_p.h" not in text:
            errors.append("ThemeSerializer must use ThemeTextCodec")
        if "setOverride(it.key()" in text:
            errors.append("ThemeSerializer still passes serialized component name into ThemeModel")

    return errors


def main(argv: Sequence[str] | None = None) -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--root", type=Path, default=ROOT)
    args = p.parse_args(argv)
    errors = validate(args.root.resolve())
    if errors:
        print("ThemeIO ownership FAILED:")
        for e in errors:
            print(" -", e)
        return 1
    print("ThemeIO ownership OK: textual component identifiers are isolated in ThemeIO")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
