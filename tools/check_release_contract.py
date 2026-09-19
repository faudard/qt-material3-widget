#!/usr/bin/env python3
"""Validate the qt-material3-widget release contract."""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path
from typing import Any

ALLOWED_MATURITY = {"complete", "usable", "partial", "skeleton", "planned"}
RELEASE_READY = {"complete", "usable"}


def score(axes: dict[str, Any], key: str) -> int | None:
    value = axes.get(key)
    if value == "N/A":
        return 4
    return value if isinstance(value, int) else None


def derived_maturity(axes: dict[str, Any]) -> str:
    api = score(axes, "api")
    rendering = score(axes, "rendering")
    states = score(axes, "states")
    accessibility = score(axes, "accessibility")
    keyboard = score(axes, "keyboard")
    hidpi = score(axes, "hidpi")
    tests = score(axes, "tests")
    example = score(axes, "example")
    docs = score(axes, "docs")

    if api is None or api < 1 or rendering is None or rendering == 0:
        return "planned"
    if (
        api >= 4
        and rendering >= 4
        and states is not None and states >= 4
        and accessibility is not None and accessibility >= 3
        and keyboard is not None and keyboard >= 4
        and hidpi is not None and hidpi >= 3
        and tests is not None and tests >= 4
        and example is not None and example >= 4
        and docs is not None and docs >= 4
    ):
        return "complete"
    if (
        api >= 2
        and rendering >= 2
        and states is not None and states >= 2
        and tests is not None and tests >= 1
        and example is not None and example >= 1
        and docs is not None and docs >= 1
    ):
        return "usable"
    if api >= 1 or rendering is not None or states is not None:
        return "partial"
    return "skeleton"


def effective_maturity(component: dict[str, Any]) -> str:
    axes = component.get("maturityAxes")
    if component.get("maturityPolicy") == "derived" and isinstance(axes, dict):
        return derived_maturity(axes)
    maturity = str(component.get("maturity", "planned"))
    return maturity if maturity in ALLOWED_MATURITY else "planned"


def project_version(cmake_text: str) -> str | None:
    match = re.search(
        r"project\s*\(\s*qt-material3-widgets\s+VERSION\s+([0-9]+\.[0-9]+\.[0-9]+)",
        cmake_text,
        flags=re.IGNORECASE | re.MULTILINE,
    )
    return match.group(1) if match else None


def validate(root: Path, expected_version: str | None = None) -> list[str]:
    errors: list[str] = []

    required = [
        "LICENSE",
        "README.md",
        "CHANGELOG.md",
        "ROADMAP.md",
        "CONTRIBUTING.md",
        "SECURITY.md",
        "packaging/QtMaterial3WidgetsConfig.cmake.in",
        "docs/components/component-registry.json",
    ]
    for relative in required:
        if not (root / relative).is_file():
            errors.append(f"missing required release file: {relative}")

    cmake_path = root / "CMakeLists.txt"
    if not cmake_path.is_file():
        errors.append("missing CMakeLists.txt")
        return errors

    cmake_text = cmake_path.read_text(encoding="utf-8", errors="replace")
    version = project_version(cmake_text)
    if not version:
        errors.append("cannot resolve project VERSION from CMakeLists.txt")
        return errors

    if expected_version and version != expected_version:
        errors.append(f"project version is {version}, expected {expected_version}")

    version_surfaces = {
        "docs/conf.py": f'release = "{version}"',
        "Doxyfile": f"PROJECT_NUMBER         = {version}",
        "src/theme/qtmaterialthemeserializer.cpp": f'QStringLiteral("{version}")',
    }
    for relative, marker in version_surfaces.items():
        path = root / relative
        if not path.is_file():
            errors.append(f"missing version surface: {relative}")
            continue
        if marker not in path.read_text(encoding="utf-8", errors="replace"):
            errors.append(f"{relative} does not report release version {version}")

    for marker in [
        'set(CPACK_GENERATOR "TGZ;ZIP")',
        'set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")',
        "include(CPack)",
    ]:
        if marker not in cmake_text:
            errors.append(f"CMake packaging contract missing: {marker}")

    registry_path = root / "docs/components/component-registry.json"
    if registry_path.is_file():
        try:
            data = json.loads(registry_path.read_text(encoding="utf-8"))
            components = data if isinstance(data, list) else data.get("components", [])
        except (json.JSONDecodeError, OSError) as exc:
            errors.append(f"cannot parse component registry: {exc}")
            components = []

        for component in components:
            if not isinstance(component, dict) or not component.get("releaseScope", False):
                continue
            maturity = effective_maturity(component)
            if maturity not in RELEASE_READY:
                errors.append(
                    f"release-scoped component {component.get('id', '<unknown>')} "
                    f"is {maturity}; expected usable or complete"
                )

    changelog = root / "CHANGELOG.md"
    if changelog.is_file() and f"## [{version}]" not in changelog.read_text(
        encoding="utf-8", errors="replace"
    ):
        errors.append(f"CHANGELOG.md has no [{version}] section")

    return errors


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", default=".")
    parser.add_argument("--expected-version")
    args = parser.parse_args()

    root = Path(args.root).resolve()
    errors = validate(root, args.expected_version)
    if errors:
        print("Release contract FAILED:")
        for error in errors:
            print(f"  - {error}")
        return 1

    print("Release contract OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())
