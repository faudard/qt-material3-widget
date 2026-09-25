#!/usr/bin/env python3
"""Validate ThemeModel autonomy and structural value semantics."""

from __future__ import annotations

import argparse
import re
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[2]

MODEL_SOURCE_NAMES = {
    "qtmaterialcolorscheme.cpp",
    "qtmaterialthemeoptions.cpp",
    "qtmaterialtheme.cpp",
    "qtmaterialthemeidentity.cpp",
    "qtmaterialthemebuilder.cpp",
    "qtmaterialcolorbackend.cpp",
    "qtmaterialmcuadapter.cpp",
    "qtmaterialthemevalidation.cpp",
    "qtmaterialtypographyscale.cpp",
    "qtmaterialshapescale.cpp",
    "qtmaterialelevationscale.cpp",
    "qtmaterialmotiontokens.cpp",
    "qtmaterialstatelayer.cpp",
    "qtmaterialdensitytokens.cpp",
    "qtmaterialiconsizetokens.cpp",
    "qtmaterialcomponenttokens.cpp",
    "qtmaterialaccessibilitytokens.cpp",
}

MODEL_HEADER_NAMES = {
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
}

FORBIDDEN_MODEL_TOKENS = (
    "QObject",
    "QWidget",
    "QApplication",
    "QGuiApplication",
    "ThemeManager",
    "ThemeContext",
    "ThemeObserver",
    "SystemTheme",
    "ThemeSerializer",
    "QJsonDocument",
    "QJsonObject",
    "QFile",
    "QSettings",
)

def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")

def validate_model_surface(root: Path) -> list[str]:
    errors: list[str] = []
    theme_src = root/"src/theme"
    theme_inc = root/"include/qtmaterial/theme"

    for name in sorted(MODEL_SOURCE_NAMES):
        path = theme_src/name
        if not path.is_file():
            errors.append(f"missing ThemeModel source: {path.relative_to(root)}")
            continue
        text = read(path)
        for token in FORBIDDEN_MODEL_TOKENS:
            if token in text:
                errors.append(f"{path.relative_to(root)} contains forbidden ThemeModel token {token}")

    for name in sorted(MODEL_HEADER_NAMES):
        path = theme_inc/name
        if not path.is_file():
            errors.append(f"missing ThemeModel header: {path.relative_to(root)}")
            continue
        text = read(path)
        for token in FORBIDDEN_MODEL_TOKENS:
            if token in text:
                errors.append(f"{path.relative_to(root)} contains forbidden ThemeModel token {token}")

    return errors

def require(path: Path, tokens: tuple[str, ...], root: Path) -> list[str]:
    if not path.is_file():
        return [f"missing file: {path.relative_to(root)}"]
    text = read(path)
    return [f"{path.relative_to(root)} missing contract token: {t}" for t in tokens if t not in text]

def validate_contract(root: Path = ROOT) -> list[str]:
    errors = validate_model_surface(root)

    errors += require(
        root/"include/qtmaterial/theme/qtmaterialtheme.h",
        (
            "Theme(const Theme&) = default;",
            "Theme(Theme&&) = default;",
            "operator==(const Theme& lhs, const Theme& rhs)",
            "operator!=(const Theme& lhs, const Theme& rhs)",
        ),
        root,
    )
    errors += require(
        root/"src/theme/qtmaterialthemeidentity.cpp",
        (
            "equalColorScheme",
            "equalTypography",
            "equalComponentOverrides",
            "lhs.options() == rhs.options()",
        ),
        root,
    )
    errors += require(
        root/"src/theme/CMakeLists.txt",
        ("qtmaterialthemeidentity.cpp",),
        root,
    )
    errors += require(
        root/"src/theme/qtmaterialthemecontext.cpp",
        ("theme == m_theme",),
        root,
    )

    context_cpp = root/"src/theme/qtmaterialthemecontext.cpp"
    context_h = root/"include/qtmaterial/theme/qtmaterialthemecontext.h"
    for path in (context_cpp, context_h):
        if path.is_file():
            text = read(path)
            for forbidden in ("ThemeSerializer", "QJsonDocument", "stableFingerprint", "m_themeFingerprint"):
                if forbidden in text:
                    errors.append(f"{path.relative_to(root)} still depends on IO identity via {forbidden}")

    return errors

def main(argv: Sequence[str] | None = None) -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--root", type=Path, default=ROOT)
    args = p.parse_args(argv)
    errors = validate_contract(args.root.resolve())
    if errors:
        print("ThemeModel contract FAILED:")
        for error in errors:
            print(" -", error)
        return 1
    print("ThemeModel contract OK: value semantics are structural and IO/runtime independent")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
