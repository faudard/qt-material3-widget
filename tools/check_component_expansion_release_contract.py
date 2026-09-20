#!/usr/bin/env python3
"""Validate the 0.8 component-expansion release contract."""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path


EXPANSION_COMPONENTS = {
    "compact.chip": "qtmaterial/widgets/selection/qtmaterialchip.h",
    "input.search-bar": "qtmaterial/widgets/inputs/qtmaterialsearchbar.h",
    "input.search-view": "qtmaterial/widgets/inputs/qtmaterialsearchview.h",
    "input.combo-box": "qtmaterial/widgets/inputs/qtmaterialcombobox.h",
    "input.slider": "qtmaterial/widgets/inputs/qtmaterialslider.h",
    "input.range-slider": "qtmaterial/widgets/inputs/qtmaterialrangeslider.h",
    "input.time-field": "qtmaterial/widgets/inputs/qtmaterialtimefield.h",
    "input.time-picker": "qtmaterial/widgets/inputs/qtmaterialtimepicker.h",
    "input.date-range-picker": "qtmaterial/widgets/inputs/qtmaterialdaterangepicker.h",
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
    for component_id, header in EXPANSION_COMPONENTS.items():
        item = by_id.get(component_id)
        if not item:
            errors.append(f"missing 0.8 component registry entry: {component_id}")
            continue
        if item.get("maturity") not in {"usable", "complete"}:
            errors.append(f"{component_id} must be usable or complete")
        if item.get("releaseScope") is not True:
            errors.append(f"{component_id} must be release-scoped")
        if item.get("testTarget") != "tst_component_expansion":
            errors.append(f"{component_id} must use tst_component_expansion")
        if item.get("publicHeader") != header:
            errors.append(f"{component_id} public header mismatch")
        if not (root / "include" / header).is_file():
            errors.append(f"missing public header: include/{header}")

    required_files = [
        "src/widgets/inputs/qtmaterialsearchbar.cpp",
        "src/widgets/inputs/qtmaterialsearchview.cpp",
        "src/widgets/inputs/qtmaterialcombobox.cpp",
        "src/widgets/inputs/qtmaterialslider.cpp",
        "src/widgets/inputs/qtmaterialrangeslider.cpp",
        "src/widgets/inputs/qtmaterialtimefield.cpp",
        "src/widgets/inputs/qtmaterialtimepicker.cpp",
        "src/widgets/inputs/qtmaterialdaterangepicker.cpp",
        "tests/widgets/inputs/tst_component_expansion.cpp",
        "docs/public-api/component-expansion.md",
    ]
    for relative in required_files:
        if not (root / relative).is_file():
            errors.append(f"missing 0.8 expansion artifact: {relative}")

    widget_cmake = read(root / "src/widgets/CMakeLists.txt")
    tests_cmake = read(root / "tests/CMakeLists.txt")
    manifest = read(root / "cmake/QtMaterial3HeaderSurfaceManifest.cmake")
    for header in EXPANSION_COMPONENTS.values():
        if header not in manifest:
            errors.append(f"public header manifest missing {header}")
    if "tst_component_expansion" not in tests_cmake:
        errors.append("tst_component_expansion is not registered")
    for source in required_files[:8]:
        if source.removeprefix("src/widgets/") not in widget_cmake:
            errors.append(f"widgets CMake missing {source}")

    docs = read(root / "docs/public-api/component-expansion.md")
    for marker in [
        "Chip family",
        "Search and choice",
        "Slider family",
        "Date and time",
        "Menu and desktop data",
    ]:
        if marker not in docs:
            errors.append(f"component expansion docs missing marker: {marker}")

    roadmap = read(root / "ROADMAP.md")
    for marker in [
        "0.8.0 — Component Expansion",
        "Search",
        "Slider",
        "Date",
        "Menu",
        "desktop-data",
    ]:
        if marker not in roadmap:
            errors.append(f"ROADMAP.md missing 0.8 marker: {marker}")

    return errors


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", default=".")
    parser.add_argument("--expected-version")
    args = parser.parse_args()
    errors = validate(Path(args.root).resolve(), args.expected_version)
    if errors:
        print("Component expansion release contract FAILED:")
        for error in errors:
            print(f"  - {error}")
        return 1
    print("Component expansion release contract OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())
