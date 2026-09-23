#!/usr/bin/env python3
"""Validate the 0.6 Theme & Tokens release contract."""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

ROOT_REQUIRED = {"formatVersion", "source", "resolved", "metadata"}
SOURCE_REQUIRED = {"seedColor", "mode", "preference", "contrast", "variant", "colorBackendPolicy"}
RESOLVED_REQUIRED = {
    "colorScheme", "typographyScale", "shapeScale", "elevationScale",
    "motionTokens", "stateLayer", "accessibility", "interactions",
    "density", "iconSizes", "componentOverrides",
}
METADATA_REQUIRED = {"generatorVersion", "libraryVersion", "qtVersion"}

def read(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace")

def project_version(text: str) -> str | None:
    match = re.search(
        r"project\s*\(\s*qt-material3-widgets\s+VERSION\s+([0-9]+\.[0-9]+\.[0-9]+)",
        text, re.IGNORECASE | re.MULTILINE,
    )
    return match.group(1) if match else None

def validate(root: Path, expected_version: str | None = None) -> list[str]:
    errors: list[str] = []
    version = project_version(read(root / "CMakeLists.txt"))
    if not version:
        return ["cannot resolve project version"]
    if expected_version and version != expected_version:
        errors.append(f"project version is {version}, expected {expected_version}")

    schema_path = root / "docs/schema/theme.schema.json"
    try:
        schema = json.loads(read(schema_path))
    except (OSError, json.JSONDecodeError) as exc:
        return errors + [f"cannot parse theme schema: {exc}"]

    if schema.get("properties", {}).get("formatVersion", {}).get("const") != 1:
        errors.append("theme schema does not freeze formatVersion 1")
    required = set(schema.get("required", []))
    if not ROOT_REQUIRED.issubset(required):
        errors.append("theme schema is missing required root blocks")

    definitions = schema.get("$defs", {})
    for block, expected in (
        ("source", SOURCE_REQUIRED),
        ("resolved", RESOLVED_REQUIRED),
        ("metadata", METADATA_REQUIRED),
    ):
        actual = set(definitions.get(block, {}).get("required", []))
        if not expected.issubset(actual):
            errors.append(f"theme schema {block} required set is incomplete")

    header = read(root / "include/qtmaterial/theme/qtmaterialthemeserializer.h")
    if "kCurrentFormatVersion = 1" not in header:
        errors.append("ThemeSerializer current format version is not 1")
    if "validateJson" not in header:
        errors.append("ThemeSerializer has no public validateJson contract")

    serializer = read(root / "src/theme/qtmaterialthemeserializer.cpp")
    for marker in (
        f'QStringLiteral("{version}")',
        "extensionComponentNames(overrides)",
        "extensionOverrideFor(overrides, extensionName)",
        "validateStrictCurrent",
    ):
        if marker not in serializer:
            errors.append(f"theme serializer contract missing marker: {marker}")

    fixtures = sorted((root / "tests/theme/fixtures").glob("*.json"))
    if not fixtures:
        errors.append("theme JSON fixture corpus is empty")
    for fixture in fixtures:
        try:
            payload = json.loads(read(fixture))
        except (OSError, json.JSONDecodeError) as exc:
            errors.append(f"invalid fixture {fixture.name}: {exc}")
            continue
        if payload.get("formatVersion") != 1:
            errors.append(f"fixture {fixture.name} is not formatVersion 1")
        if not ROOT_REQUIRED.issubset(payload):
            errors.append(f"fixture {fixture.name} is missing root blocks")

    required_files = {
        "tests/theme/tst_theme_json_contract.cpp": [
            "semanticRoundTripIsLossless", "opaqueExtensionOverrideRoundTrips",
            "compactSerializationIsDeterministic",
        ],
        "tests/theme/tst_theme_token_matrix.cpp": [
            "fallbackMatrix", "mcuMatrixWhenAvailable",
        ],
        "tests/theme/tst_systemtheme.cpp": [
            "policyResolutionIsDeterministic", "contrastResolutionIsDeterministic",
        ],
        "examples/theme-studio/themestudiocontroller.cpp": [
            "ThemeReadMode::Strict", "setBackendPolicy", "setPreference", "validateJson",
        ],
        "benchmarks/theme/benchmark_theme_manager_fanout.cpp": [
            "500-observers", "1000-observers",
        ],
        "docs/public-api/component-overrides.md": ["Precedence"],
        "docs/public-api/system-theme.md": ["FollowSystem"],
    }
    for relative, markers in required_files.items():
        path = root / relative
        if not path.is_file():
            errors.append(f"missing theme release artifact: {relative}")
            continue
        content = read(path)
        for marker in markers:
            if marker not in content:
                errors.append(f"{relative} missing contract marker: {marker}")

    return errors

def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", default=".")
    parser.add_argument("--expected-version")
    args = parser.parse_args()
    errors = validate(Path(args.root).resolve(), args.expected_version)
    if errors:
        print("Theme release contract FAILED:")
        for error in errors:
            print(f"  - {error}")
        return 1
    print("Theme release contract OK")
    return 0

if __name__ == "__main__":
    sys.exit(main())
