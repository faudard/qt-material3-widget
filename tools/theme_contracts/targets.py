#!/usr/bin/env python3
"""Validate the physical ThemeModel/ThemeIO/ThemeRuntime decomposition."""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path
from typing import Iterable, Sequence

ROOT = Path(__file__).resolve().parents[2]

MODEL_HEADERS = (
    "qtmaterialcolortoken.h",
    "qtmaterialcolorbackend.h",
    "qtmaterialcolorscheme.h",
    "qtmaterialthemeoptions.h",
    "qtmaterialtheme.h",
    "qtmaterialthemebuilder.h",
    "qtmaterialthemevalidation.h",
    "qtmaterialtypographyscale.h",
    "qtmaterialshapescale.h",
    "qtmaterialelevationscale.h",
    "qtmaterialmotiontokens.h",
    "qtmaterialstatelayer.h",
    "qtmaterialdensitytokens.h",
    "qtmaterialiconsizetokens.h",
    "qtmaterialcomponenttokens.h",
    "qtmaterialaccessibilitytokens.h",
)
IO_HEADERS = (
    "qtmaterialthemeserializer.h",
)
RUNTIME_HEADERS = (
    "qtmaterialthemecontext.h",
    "qtmaterialthemecontexthost.h",
    "qtmaterialthememanager.h",
    "qtmaterialthemeobserver.h",
    "qtmaterialsystemtheme.h",
)

FORBIDDEN_SPECS_HEADERS = (
    "qtmaterialthememanager.h",
    "qtmaterialthemecontext.h",
    "qtmaterialthemecontexthost.h",
    "qtmaterialthemeobserver.h",
    "qtmaterialsystemtheme.h",
    "qtmaterialthemeserializer.h",
    "qtmaterialxmlthemeadapter.h",
)

def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")

def require_tokens(path: Path, tokens: Iterable[str]) -> list[str]:
    if not path.is_file():
        return [f"missing file: {path.relative_to(ROOT)}"]
    text = read(path)
    return [
        f"{path.relative_to(ROOT)} missing contract token: {token}"
        for token in tokens if token not in text
    ]

def validate_header_exports() -> list[str]:
    errors: list[str] = []
    base = ROOT / "include/qtmaterial/theme"
    for name in MODEL_HEADERS:
        path = base/name
        if path.is_file() and "QTMATERIAL3_THEME_EXPORT" in read(path):
            errors.append(f"{path.relative_to(ROOT)} still uses QTMATERIAL3_THEME_EXPORT")
        if path.is_file() and "QTMATERIAL3_THEME_MODEL_EXPORT" not in read(path):
            # Some pure enum/value headers may have no exported symbol at all.
            text = read(path)
            if "QTMATERIAL3_" in text and "EXPORT" in text:
                errors.append(f"{path.relative_to(ROOT)} does not use THEME_MODEL export")
    for name in IO_HEADERS:
        path = base/name
        if path.is_file() and "QTMATERIAL3_THEME_EXPORT" in read(path):
            errors.append(f"{path.relative_to(ROOT)} still uses QTMATERIAL3_THEME_EXPORT")
        if path.is_file() and "QTMATERIAL3_THEME_IO_EXPORT" not in read(path):
            errors.append(f"{path.relative_to(ROOT)} does not use THEME_IO export")
    for name in RUNTIME_HEADERS:
        path = base/name
        if path.is_file() and "QTMATERIAL3_THEME_EXPORT" in read(path):
            errors.append(f"{path.relative_to(ROOT)} still uses QTMATERIAL3_THEME_EXPORT")
        if path.is_file() and "QTMATERIAL3_THEME_RUNTIME_EXPORT" not in read(path):
            errors.append(f"{path.relative_to(ROOT)} does not use THEME_RUNTIME export")
    return errors

def validate_specs_boundary() -> list[str]:
    errors: list[str] = []
    cmake = ROOT/"src/specs/CMakeLists.txt"
    if not cmake.is_file():
        return ["missing src/specs/CMakeLists.txt"]
    text = read(cmake)
    if "qtmaterial3_theme_model" not in text:
        errors.append("Specs must link qtmaterial3_theme_model")
    for forbidden in ("qtmaterial3_theme\n", "qtmaterial3_theme_io", "qtmaterial3_theme_runtime"):
        if forbidden in text:
            errors.append(f"Specs must not link {forbidden.strip()}")

    for base_rel in ("src/specs", "include/qtmaterial/specs"):
        base = ROOT/base_rel
        if not base.exists():
            continue
        for path in base.rglob("*"):
            if not path.is_file() or path.suffix.lower() not in {".h",".hpp",".cpp",".cc",".cxx"}:
                continue
            content = read(path)
            for header in FORBIDDEN_SPECS_HEADERS:
                if header in content:
                    errors.append(
                        f"{path.relative_to(ROOT)} crosses Specs boundary via {header}"
                    )
    return errors

def validate_contract(repo_root: Path = ROOT) -> list[str]:
    global ROOT
    old = ROOT
    ROOT = repo_root
    try:
        errors: list[str] = []
        errors += require_tokens(
            ROOT/"src/theme/CMakeLists.txt",
            [
                "qtmaterial3_theme_model",
                "ThemeModel",
                "qtmaterial3_theme_io",
                "ThemeIO",
                "qtmaterial3_theme_runtime",
                "ThemeRuntime",
            ],
        )
        errors += require_tokens(
            ROOT/"include/qtmaterial/qtmaterialglobal.h",
            [
                "QTMATERIAL3_THEME_MODEL_EXPORT",
                "QTMATERIAL3_THEME_IO_EXPORT",
                "QTMATERIAL3_THEME_RUNTIME_EXPORT",
            ],
        )
        errors += require_tokens(
            ROOT/"CMakeLists.txt",
            [
                "qtmaterial3_theme_model",
                "qtmaterial3_theme_io",
                "qtmaterial3_theme_runtime",
            ],
        )
        errors += require_tokens(
            ROOT/"packaging/QtMaterial3WidgetsConfig.cmake.in",
            ["ThemeModel", "ThemeIO", "ThemeRuntime"],
        )
        theme_cmake = read(ROOT/"src/theme/CMakeLists.txt")
        if "add_library(qtmaterial3_theme INTERFACE)" in theme_cmake or "QtMaterial3::Theme" in theme_cmake:
            errors.append("pre-1.0 Theme umbrella target must not be reintroduced")
        package_config = read(ROOT/"packaging/QtMaterial3WidgetsConfig.cmake.in")
        if re.search(r"^\\s*Theme\\s*$", package_config, re.MULTILINE):
            errors.append("Theme compatibility component must not be exported")

        errors += validate_header_exports()
        errors += validate_specs_boundary()
        return errors
    finally:
        ROOT = old

def main(argv: Sequence[str] | None = None) -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--root", type=Path, default=ROOT)
    args = p.parse_args(argv)
    errors = validate_contract(args.root.resolve())
    if errors:
        print("Theme target decomposition FAILED:")
        for error in errors:
            print(" -", error)
        return 1
    print(
        "Theme target decomposition OK: Specs -> ThemeModel; "
        "ThemeIO/ThemeRuntime are physically separate"
    )
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
