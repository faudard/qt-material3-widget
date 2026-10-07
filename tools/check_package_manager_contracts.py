#!/usr/bin/env python3
from __future__ import annotations

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def validate(root: Path = ROOT) -> list[str]:
    errors: list[str] = []
    required = [
        root / "conanfile.py",
        root / "test_package" / "conanfile.py",
        root / "test_package" / "CMakeLists.txt",
        root / "packaging" / "vcpkg" / "ports" / "qt-material3-widget" / "vcpkg.json",
        root / "packaging" / "vcpkg" / "ports" / "qt-material3-widget" / "portfile.cmake",
        root / "packaging" / "vcpkg" / "ports" / "qt-material3-widget" / "usage",
    ]
    for path in required:
        if not path.is_file():
            errors.append(f"missing package-manager contract: {path.relative_to(root)}")
    if errors:
        return errors

    conan = (root / "conanfile.py").read_text(encoding="utf-8")
    for token in [
        'name = "qt-material3-widget"',
        '"qt_major": ["5", "6"]',
        '"with_designer": [True, False]',
        'QTMATERIAL3_EXPECT_QT_MAJOR',
        'cmake_find_mode", "none"',
        'QtMaterial3Widgets',
    ]:
        if token not in conan:
            errors.append(f"Conan recipe missing contract token: {token}")

    manifest_path = root / "packaging" / "vcpkg" / "ports" / "qt-material3-widget" / "vcpkg.json"
    try:
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        errors.append(f"vcpkg manifest invalid: {exc}")
        return errors

    if manifest.get("name") != "qt-material3-widget":
        errors.append("vcpkg manifest package name drift")
    if manifest.get("default-features") != ["qt6"]:
        errors.append("vcpkg default feature must be qt6")
    features = manifest.get("features", {})
    for feature in ("qt5", "qt6", "designer-qt5", "designer-qt6"):
        if feature not in features:
            errors.append(f"vcpkg manifest missing feature: {feature}")

    portfile = (manifest_path.parent / "portfile.cmake").read_text(encoding="utf-8")
    if not re.search(r"\bREF [0-9a-f]{40}\b", portfile):
        errors.append("vcpkg port must pin an immutable 40-character Git commit")
    for token in [
        "vcpkg_from_git(",
        "HEAD_REF main",
        "vcpkg_cmake_configure(",
        "vcpkg_cmake_install()",
        "vcpkg_cmake_config_fixup(",
        "PACKAGE_NAME QtMaterial3Widgets",
        'if("qt5" IN_LIST FEATURES AND "qt6" IN_LIST FEATURES)',
    ]:
        if token not in portfile:
            errors.append(f"vcpkg port missing contract token: {token}")

    compatibility = (root / "cmake" / "QtMaterial3QtCompatibility.cmake").read_text(encoding="utf-8")
    if "QTMATERIAL3_EXPECT_QT_MAJOR" not in compatibility:
        errors.append("CMake compatibility gate does not expose QTMATERIAL3_EXPECT_QT_MAJOR")

    installation = (root / "docs" / "installation.md").read_text(encoding="utf-8")
    for heading in ("## Conan 2", "## vcpkg overlay port"):
        if heading not in installation:
            errors.append(f"installation docs missing {heading}")

    return errors


def main() -> int:
    errors = validate()
    if errors:
        print("package-manager contract validation failed:")
        for error in errors:
            print(f" - {error}")
        return 1
    print("package-manager contracts OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())
