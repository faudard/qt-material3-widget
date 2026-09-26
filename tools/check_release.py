#!/usr/bin/env python3
"""Unified release contract checker for QtMaterial3."""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path
from typing import Any, Sequence

TOOLS = Path(__file__).resolve().parent
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))

import component_registry
import header_surface

ROOT = Path(__file__).resolve().parents[1]
RULES = Path("tools/release_rules.json")
RELEASE_READY = {"complete", "usable"}


class ReleaseConfigurationError(RuntimeError):
    pass


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace")


def project_version(cmake_text: str) -> str | None:
    match = re.search(
        r"project\s*\(\s*qt-material3-widgets\s+VERSION\s+"
        r"([0-9]+\.[0-9]+\.[0-9]+)",
        cmake_text,
        flags=re.IGNORECASE | re.MULTILINE,
    )
    return match.group(1) if match else None


def load_rules(path: Path) -> dict[str, Any]:
    try:
        data = json.loads(read(path))
    except FileNotFoundError as exc:
        raise ReleaseConfigurationError(f"missing release rules: {path}") from exc
    except json.JSONDecodeError as exc:
        raise ReleaseConfigurationError(
            f"invalid release rules JSON: {exc}"
        ) from exc
    if not isinstance(data, dict) or data.get("schema_version") != 1:
        raise ReleaseConfigurationError(
            "release rules must use schema_version 1"
        )
    for scope in ("base", "theme", "interaction", "api_freeze"):
        if not isinstance(data.get(scope), dict):
            raise ReleaseConfigurationError(
                f"release rules missing scope: {scope}"
            )
    return data


def validate_version(
    root: Path,
    rules: dict[str, Any],
    expected_version: str | None,
) -> tuple[str | None, list[str]]:
    errors: list[str] = []
    cmake_path = root / "CMakeLists.txt"
    if not cmake_path.is_file():
        return None, ["missing CMakeLists.txt"]

    cmake_text = read(cmake_path)
    version = project_version(cmake_text)
    if not version:
        return None, [
            "cannot resolve project VERSION from CMakeLists.txt"
        ]
    if expected_version and version != expected_version:
        errors.append(
            f"project version is {version}, expected {expected_version}"
        )

    for relative, marker in rules["base"].get(
        "version_surfaces", {}
    ).items():
        path = root / relative
        if not path.is_file():
            errors.append(f"missing version surface: {relative}")
            continue
        if str(marker) not in read(path):
            errors.append(
                f"{relative} is not wired to the project release version"
            )

    return version, errors


def validate_base(
    root: Path,
    rules: dict[str, Any],
    version: str,
) -> list[str]:
    errors: list[str] = []
    base = rules["base"]

    for relative in base.get("required_files", []):
        if not (root / str(relative)).is_file():
            errors.append(
                f"missing required release file: {relative}"
            )

    cmake_text = read(root / "CMakeLists.txt")
    for marker in base.get("cmake_markers", []):
        if str(marker) not in cmake_text:
            errors.append(
                f"CMake packaging contract missing: {marker}"
            )

    try:
        components = component_registry.load_registry(root)
    except Exception as exc:
        errors.append(f"cannot parse component registry: {exc}")
        components = []

    for component in components:
        if not component.get("releaseScope", False):
            continue
        maturity = component_registry.effective_maturity(component)
        if maturity not in RELEASE_READY:
            errors.append(
                "release-scoped component "
                f"{component.get('id', '<unknown>')} is {maturity}; "
                "expected usable or complete"
            )

    changelog = root / "CHANGELOG.md"
    if changelog.is_file() and f"## [{version}]" not in read(changelog):
        errors.append(f"CHANGELOG.md has no [{version}] section")

    return errors


def validate_theme(
    root: Path,
    rules: dict[str, Any],
) -> list[str]:
    errors: list[str] = []
    config = rules["theme"]
    schema_config = config.get("schema", {})
    root_required = set(schema_config.get("root_required", []))

    schema_path = root / "docs/schema/theme.schema.json"
    try:
        schema = json.loads(read(schema_path))
    except (OSError, json.JSONDecodeError) as exc:
        return [f"cannot parse theme schema: {exc}"]

    if (
        schema.get("properties", {})
        .get("formatVersion", {})
        .get("const")
        != 1
    ):
        errors.append("theme schema does not freeze formatVersion 1")
    if not root_required.issubset(set(schema.get("required", []))):
        errors.append("theme schema is missing required root blocks")

    definitions = schema.get("$defs", {})
    for block, key in (
        ("source", "source_required"),
        ("resolved", "resolved_required"),
        ("metadata", "metadata_required"),
    ):
        expected = set(schema_config.get(key, []))
        actual = set(definitions.get(block, {}).get("required", []))
        if not expected.issubset(actual):
            errors.append(
                f"theme schema {block} required set is incomplete"
            )

    header = read(
        root
        / "include/qtmaterial/theme/qtmaterialthemeserializer.h"
    )
    if "kCurrentFormatVersion = 1" not in header:
        errors.append(
            "ThemeSerializer current format version is not 1"
        )
    if "validateJson" not in header:
        errors.append(
            "ThemeSerializer has no public validateJson contract"
        )

    serializer = read(
        root / "src/theme/qtmaterialthemeserializer.cpp"
    )
    for marker in (
        "QTMATERIAL3_VERSION_STRING",
        "extensionComponentNames(overrides)",
        "extensionOverrideFor(overrides, extensionName)",
        "validateStrictCurrent",
    ):
        if marker not in serializer:
            errors.append(
                f"theme serializer contract missing marker: {marker}"
            )

    fixtures = sorted((root / "tests/theme/fixtures").glob("*.json"))
    if not fixtures:
        errors.append("theme JSON fixture corpus is empty")
    for fixture in fixtures:
        try:
            payload = json.loads(read(fixture))
        except (OSError, json.JSONDecodeError) as exc:
            errors.append(
                f"invalid fixture {fixture.name}: {exc}"
            )
            continue
        if payload.get("formatVersion") != 1:
            errors.append(
                f"fixture {fixture.name} is not formatVersion 1"
            )
        if not root_required.issubset(payload):
            errors.append(
                f"fixture {fixture.name} is missing root blocks"
            )

    errors.extend(
        validate_required_markers(
            root,
            config.get("required_files", {}),
            "theme release artifact",
        )
    )
    return errors


def validate_required_markers(
    root: Path,
    contracts: dict[str, Any],
    label: str,
) -> list[str]:
    errors: list[str] = []
    for relative, markers in contracts.items():
        path = root / relative
        if not path.is_file():
            errors.append(f"missing {label}: {relative}")
            continue
        content = read(path)
        for marker in markers:
            if str(marker) not in content:
                errors.append(
                    f"{relative} missing contract marker: {marker}"
                )
    return errors


def validate_interaction(
    root: Path,
    rules: dict[str, Any],
) -> list[str]:
    return validate_required_markers(
        root,
        rules["interaction"].get("required_files", {}),
        "interaction release artifact",
    )



def parse_cmake_list(text: str, variable: str) -> list[str]:
    match = re.search(
        rf"set\\(\\s*{re.escape(variable)}\\s*(.*?)\\)",
        text,
        flags=re.DOTALL,
    )
    if not match:
        return []
    body = re.sub(r"#.*", "", match.group(1))
    return re.findall(r"[A-Za-z][A-Za-z0-9_.:+-]*", body)


def validate_api_freeze(
    root: Path,
    rules: dict[str, Any],
) -> list[str]:
    errors: list[str] = []
    config = rules["api_freeze"]

    manifest = root / header_surface.MANIFEST
    try:
        public_headers, _ = header_surface.parse_manifest(manifest)
    except (OSError, ValueError) as exc:
        return [f"cannot parse public-header manifest: {exc}"]

    public_set = set(public_headers)

    forbidden_suffixes = tuple(
        str(value)
        for value in config.get("forbidden_public_header_suffixes", [])
    )
    for header in public_headers:
        if forbidden_suffixes and header.endswith(forbidden_suffixes):
            errors.append(
                f"API freeze: internal resolver/header is public: {header}"
            )

    forbidden_text = [
        str(value)
        for value in config.get("forbidden_public_text", [])
    ]
    for header in public_headers:
        path = root / "include" / header
        if not path.is_file():
            errors.append(f"API freeze: missing public header: {header}")
            continue
        content = read(path)
        for marker in forbidden_text:
            if marker in content:
                errors.append(
                    f"API freeze: public header {header} contains "
                    f"compatibility-only marker: {marker}"
                )

    try:
        registry_headers = {
            str(item.get("publicHeader"))
            for item in component_registry.load_registry(root)
            if item.get("publicHeader")
        }
    except Exception as exc:
        errors.append(f"API freeze: cannot load component registry: {exc}")
        registry_headers = set()

    support_headers = {
        str(value)
        for value in config.get("public_support_headers", [])
    }
    widget_headers = {
        header
        for header in public_headers
        if header.startswith("qtmaterial/widgets/")
    }
    unowned = sorted(
        widget_headers - registry_headers - support_headers
    )
    if unowned:
        errors.append(
            "API freeze: unowned public widget headers: "
            + ", ".join(unowned)
        )

    stale_support = sorted(support_headers - public_set)
    if stale_support:
        errors.append(
            "API freeze: support-header allowlist contains non-public "
            "entries: " + ", ".join(stale_support)
        )

    package_config = (
        root / "packaging" / "QtMaterial3WidgetsConfig.cmake.in"
    )
    if not package_config.is_file():
        errors.append("API freeze: missing package config template")
    else:
        actual_components = parse_cmake_list(
            read(package_config),
            "_QtMaterial3Widgets_supported_components",
        )
        expected_components = [
            str(value)
            for value in config.get("package_components", [])
        ]
        if actual_components != expected_components:
            errors.append(
                "API freeze: public CMake components differ from frozen "
                f"contract: {actual_components!r}"
            )

    tests_root = root / "tests"
    tests_cmake = tests_root / "CMakeLists.txt"
    standalone = {
        str(value)
        for value in config.get("standalone_cpp_tests", [])
    }
    if tests_root.is_dir() and tests_cmake.is_file():
        cmake_text = read(tests_cmake)
        unregistered: list[str] = []
        for source in sorted(tests_root.rglob("*.cpp")):
            relative = source.relative_to(root).as_posix()
            if relative in standalone:
                continue
            source_token = source.relative_to(tests_root).as_posix()
            if source_token not in cmake_text:
                unregistered.append(relative)
        if unregistered:
            errors.append(
                "API freeze: retained C++ tests are not registered: "
                + ", ".join(unregistered)
            )

        missing_standalone = sorted(
            relative
            for relative in standalone
            if not (root / relative).is_file()
        )
        if missing_standalone:
            errors.append(
                "API freeze: standalone C++ test allowlist is stale: "
                + ", ".join(missing_standalone)
            )
    else:
        errors.append("API freeze: tests/CMakeLists.txt is missing")

    return errors


def normalize_scopes(scopes: Sequence[str] | None) -> tuple[str, ...]:
    if not scopes or "all" in scopes:
        return ("base", "theme", "interaction", "api-freeze")
    result = tuple(dict.fromkeys(scopes))
    unknown = sorted(
        set(result) - {"base", "theme", "interaction", "api-freeze"}
    )
    if unknown:
        raise ReleaseConfigurationError(
            "unknown release scope(s): " + ", ".join(unknown)
        )
    return result


def validate(
    root: Path,
    expected_version: str | None = None,
    scopes: Sequence[str] | None = None,
    rules_path: Path | None = None,
) -> list[str]:
    root = root.resolve()
    path = (
        rules_path.resolve()
        if rules_path is not None
        else (root / RULES).resolve()
    )
    rules = load_rules(path)
    selected = normalize_scopes(scopes)

    version, errors = validate_version(
        root, rules, expected_version
    )
    if version is None:
        return errors

    if "base" in selected:
        errors.extend(validate_base(root, rules, version))
    if "theme" in selected:
        errors.extend(validate_theme(root, rules))
    if "interaction" in selected:
        errors.extend(validate_interaction(root, rules))
    if "api-freeze" in selected:
        errors.extend(validate_api_freeze(root, rules))
    return errors


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument("--expected-version")
    parser.add_argument(
        "--scope",
        action="append",
        choices=("all", "base", "theme", "interaction", "api-freeze"),
        default=[],
    )
    parser.add_argument("--rules", type=Path)
    args = parser.parse_args(argv)

    try:
        errors = validate(
            args.root,
            expected_version=args.expected_version,
            scopes=args.scope,
            rules_path=args.rules,
        )
    except ReleaseConfigurationError as exc:
        print(f"Release checker configuration error: {exc}")
        return 2

    if errors:
        print("Release contract FAILED:")
        for error in errors:
            print(f" - {error}")
        return 1

    scopes = ", ".join(normalize_scopes(args.scope))
    print(f"Release contract OK ({scopes})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
