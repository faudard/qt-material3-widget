#!/usr/bin/env python3
"""Validate the 0.9 desktop/productivity release contract."""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path


DESKTOP_COMPONENTS = {
    "data.tree-view": "qtmaterial/widgets/data/qtmaterialtreeview.h",
    "data.pagination": "qtmaterial/widgets/data/qtmaterialpagination.h",
    "layout.split-view": "qtmaterial/widgets/layouts/qtmaterialsplitview.h",
    "navigation.breadcrumb": "qtmaterial/widgets/navigation/qtmaterialbreadcrumb.h",
    "navigation.command-palette": "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h",
}


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace")


def project_version(text: str) -> str | None:
    match = re.search(
        r"project\s*\(\s*qt-material3-widgets\s+VERSION\s+([0-9]+\.[0-9]+\.[0-9]+)",
        text,
        re.IGNORECASE | re.MULTILINE,
    )
    return match.group(1) if match else None


def validate(root: Path, expected_version: str | None = None) -> list[str]:
    errors: list[str] = []
    version = project_version(read(root / "CMakeLists.txt"))
    if not version:
        return ["cannot resolve project version"]
    if expected_version and version != expected_version:
        errors.append(f"project version is {version}, expected {expected_version}")

    registry_path = root / "docs/components/component-registry.json"
    try:
        registry = json.loads(read(registry_path))
    except (OSError, json.JSONDecodeError) as exc:
        return [f"cannot read component registry: {exc}"]

    by_id = {str(item.get("id")): item for item in registry}
    for component_id, header in DESKTOP_COMPONENTS.items():
        item = by_id.get(component_id)
        if not item:
            errors.append(f"missing 0.9 registry entry: {component_id}")
            continue
        if item.get("maturity") not in {"usable", "complete"}:
            errors.append(f"{component_id} must be usable or complete")
        if item.get("releaseScope") is not True:
            errors.append(f"{component_id} must be release-scoped")
        if item.get("testTarget") != "tst_desktop_productivity":
            errors.append(f"{component_id} must use tst_desktop_productivity")
        if item.get("publicHeader") != header:
            errors.append(f"{component_id} public header mismatch")
        if not (root / "include" / header).is_file():
            errors.append(f"missing public header: include/{header}")

    required_sources = [
        "src/widgets/data/qtmaterialtreeview.cpp",
        "src/widgets/data/qtmaterialpagination.cpp",
        "src/widgets/layouts/qtmaterialsplitview.cpp",
        "src/widgets/navigation/qtmaterialbreadcrumb.cpp",
        "src/widgets/navigation/qtmaterialcommandpalette.cpp",
    ]
    for relative in required_sources:
        if not (root / relative).is_file():
            errors.append(f"missing 0.9 production source: {relative}")

    for relative in [
        "tests/widgets/data/tst_desktop_productivity.cpp",
        "docs/public-api/desktop-productivity.md",
    ]:
        if not (root / relative).is_file():
            errors.append(f"missing 0.9 certification artifact: {relative}")

    widget_cmake = read(root / "src/widgets/CMakeLists.txt")
    tests_cmake = read(root / "tests/CMakeLists.txt")
    manifest = read(root / "cmake/QtMaterial3HeaderSurfaceManifest.cmake")
    for header in DESKTOP_COMPONENTS.values():
        if header not in manifest:
            errors.append(f"public header manifest missing {header}")
    for source in required_sources:
        if source.removeprefix("src/widgets/") not in widget_cmake:
            errors.append(f"widgets CMake missing {source}")
    if "tst_desktop_productivity" not in tests_cmake:
        errors.append("tst_desktop_productivity is not registered")

    table_header = read(root / "include/qtmaterial/widgets/data/qtmaterialtable.h")
    for marker in ["columnReorderingEnabled", "cellSelectionEnabled", "dragDropEnabled"]:
        if marker not in table_header:
            errors.append(f"QtMaterialTable missing desktop API: {marker}")

    tests = read(root / "tests/widgets/data/tst_desktop_productivity.cpp")
    if "100000" not in tests:
        errors.append("desktop certification must exercise a 100000-row virtual model")

    docs = read(root / "docs/public-api/desktop-productivity.md")
    for marker in [
        "Tree View",
        "Advanced Table",
        "Pagination",
        "Split View",
        "Desktop navigation and commands",
        "Drag and drop",
        "Certification",
    ]:
        if marker not in docs:
            errors.append(f"desktop docs missing marker: {marker}")

    roadmap = read(root / "ROADMAP.md")
    for marker in [
        "0.9.0 — Desktop and Productivity",
        "TreeView",
        "Data Table",
        "Pagination",
        "Split View",
        "Command Palette",
        "Drag & Drop",
    ]:
        if marker not in roadmap:
            errors.append(f"ROADMAP.md missing 0.9 marker: {marker}")

    return errors


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", default=".")
    parser.add_argument("--expected-version")
    args = parser.parse_args()
    errors = validate(Path(args.root).resolve(), args.expected_version)
    if errors:
        print("Desktop/productivity release contract FAILED:")
        for error in errors:
            print(f"  - {error}")
        return 1
    print("Desktop/productivity release contract OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())
