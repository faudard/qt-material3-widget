#!/usr/bin/env python3
from __future__ import annotations
import argparse
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[2]

RUNTIME_HEADERS = (
    "qtmaterialthemecontext.h","qtmaterialthemecontextdefaults.h",
    "qtmaterialthemecontexthost.h","qtmaterialthememanager.h",
    "qtmaterialthemeobserver.h","qtmaterialsystemtheme.h",
)
RUNTIME_SOURCES = (
    "qtmaterialthemecontext.cpp","qtmaterialthemecontextdefaults.cpp",
    "qtmaterialthemecontexthost.cpp","qtmaterialthememanager.cpp",
    "qtmaterialthemeobserver.cpp","qtmaterialsystemtheme.cpp",
)
FORBIDDEN_IO = (
    "ThemeSerializer","QJsonDocument","QJsonObject",
    "ThemeReadMode","exportThemeJson","exportThemeToFile",
    "importThemeJson","importThemeFromFile",
    "themePreferenceFromString","QString toString(ThemePreference",
)

def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")

def validate(root: Path = ROOT) -> list[str]:
    errors: list[str] = []
    cmake = root/"src/theme/CMakeLists.txt"
    if not cmake.is_file():
        errors.append("missing src/theme/CMakeLists.txt")
    else:
        data = read(cmake)
        start = data.find("target_link_libraries(qtmaterial3_theme_runtime")
        if start < 0:
            errors.append("ThemeRuntime target link block missing")
        else:
            end = data.find(")", start)
            block = data[start:end+1]
            if "qtmaterial3_theme_io" in block:
                errors.append("ThemeRuntime must not link ThemeIO")
        for token in ("qtmaterialthemecontextdefaults.cpp",
                      "qtmaterialthemecontextdefaults.h"):
            if token not in data:
                errors.append(f"ThemeRuntime target missing {token}")

    for name in RUNTIME_HEADERS:
        path = root/"include/qtmaterial/theme"/name
        if path.is_file():
            data = read(path)
            for token in FORBIDDEN_IO:
                if token in data:
                    errors.append(
                        f"{path.relative_to(root)} contains ThemeIO token {token}")
    for name in RUNTIME_SOURCES:
        path = root/"src/theme"/name
        if path.is_file():
            data = read(path)
            for token in FORBIDDEN_IO:
                if token in data:
                    errors.append(
                        f"{path.relative_to(root)} contains ThemeIO token {token}")

    mh = root/"include/qtmaterial/theme/qtmaterialthememanager.h"
    mc = root/"src/theme/qtmaterialthememanager.cpp"
    if mh.is_file():
        data = read(mh)
        if "quint64 m_revision" in data:
            errors.append("ThemeManager must not own a second revision counter")
        if "m_themeFingerprint" in data or "stableFingerprint" in data:
            errors.append("ThemeManager must not own fingerprint identity")
    if mc.is_file():
        data = read(mc)
        for token in (
            "return m_defaultContext->revision();",
            "theme == m_defaultContext->theme()",
            "m_defaultContext->setTheme(theme)",
        ):
            if token not in data:
                errors.append(f"ThemeManager missing deterministic token: {token}")

    defaults = root/"src/theme/qtmaterialthemecontextdefaults.cpp"
    if defaults.is_file() and \
       "ThemeManager::instance().defaultContext()" not in read(defaults):
        errors.append("ThemeContextDefaults must delegate to ThemeManager default")

    binding = root/"src/core/qtmaterialthemecontextbinding.cpp"
    if binding.is_file():
        data = read(binding)
        if "ThemeManager::instance" in data or \
           "qtmaterialthememanager.h" in data:
            errors.append("Core binding must not depend directly on ThemeManager")
        for token in (
            "ThemeContextDefaults::defaultContext()",
            "QEvent::ParentChange",
            "ThemeContextPropagation::eventType()",
        ):
            if token not in data:
                errors.append(f"Core binding missing {token}")


    core_cmake = root/"src/core/CMakeLists.txt"
    if core_cmake.is_file():
        data = read(core_cmake)
        if "        qtmaterial3_theme\n" in data:
            errors.append("Core must not link the Theme umbrella because it pulls ThemeIO")
        for token in ("qtmaterial3_theme_model", "qtmaterial3_theme_runtime"):
            if token not in data:
                errors.append(f"Core must link {token}")

    observer = root/"src/theme/qtmaterialthemeobserver.cpp"
    if observer.is_file():
        data = read(observer)
        if "ThemeManager::instance" in data:
            errors.append("ThemeObserver must observe ThemeContext, not ThemeManager")
        if "ThemeContextDefaults::defaultContext()" not in data:
            errors.append("ThemeObserver must support default fallback")

    return errors

def main(argv: Sequence[str] | None = None) -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--root", type=Path, default=ROOT)
    args = p.parse_args(argv)
    errors = validate(args.root.resolve())
    if errors:
        print("ThemeRuntime contract FAILED:")
        for error in errors:
            print(" -", error)
        return 1
    print("ThemeRuntime contract OK")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
