#!/usr/bin/env python3
"""Governance checker for the QtMaterial3 component registry.

This tool layers project-governance invariants on top of the authoritative
existing generator/validator in scripts/generate_component_status.py.

No third-party Python package is required. The JSON Schema file is published
for editors/tooling; this script enforces the critical invariants in CI.
"""

from __future__ import annotations

import argparse
import importlib.util
import json
import re
import subprocess
import sys
from collections import defaultdict
from datetime import date
from pathlib import Path
from typing import Any, Iterable, Sequence

ROOT = Path(__file__).resolve().parents[1]
GENERATOR_PATH = ROOT / "scripts" / "generate_component_status.py"
REGISTRY_PATH = ROOT / "docs" / "components" / "component-registry.json"
SCHEMA_PATH = ROOT / "docs" / "components" / "component-registry.schema.json"

ID_RE = re.compile(r"^[a-z0-9]+(?:[.-][a-z0-9]+)*$")
COMPONENT_FIELDS = {
    "id", "name", "family", "maturity", "maturityPolicy", "publicHeader",
    "specType", "widgetType", "testTarget", "galleryRoute", "docsPath",
    "releaseScope", "referenceCandidate", "maturityAxes",
}


def load_generator():
    spec = importlib.util.spec_from_file_location("qtm3_component_status_generator", GENERATOR_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"cannot load {GENERATOR_PATH}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def validate_schema_contract(path: Path = SCHEMA_PATH) -> list[str]:
    errors: list[str] = []
    try:
        schema = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError:
        return [f"missing schema: {path.relative_to(ROOT) if path.is_absolute() else path}"]
    except json.JSONDecodeError as exc:
        return [f"invalid JSON schema: {exc}"]

    if schema.get("$schema") != "https://json-schema.org/draft/2020-12/schema":
        errors.append("component registry schema must use JSON Schema draft 2020-12")
    if schema.get("x-qtm3-schemaVersion") != 1:
        errors.append("component registry schema version must be 1")
    if schema.get("type") != "array":
        errors.append("component registry v1 schema must describe an array")
    required = set(schema.get("items", {}).get("required", []))
    missing = sorted(COMPONENT_FIELDS - required)
    if missing:
        errors.append("schema missing required fields: " + ", ".join(missing))
    item_schema = schema.get("items", {})
    if item_schema.get("additionalProperties") is not False:
        errors.append("component registry items must reject additional properties")
    axes_schema = item_schema.get("properties", {}).get("maturityAxes", {})
    required_axes = set(axes_schema.get("required", []))
    expected_axes = {
        "api", "rendering", "states", "accessibility", "keyboard",
        "hidpi", "rtl", "tests", "example", "docs", "lastReviewed",
        "gaps", "nextActions", "evidence",
    }
    missing_axes = sorted(expected_axes - required_axes)
    if missing_axes:
        errors.append(
            "maturityAxes schema missing required fields: "
            + ", ".join(missing_axes)
        )
    return errors


def _duplicates(values: Iterable[tuple[str, str]]) -> dict[str, list[str]]:
    by_value: dict[str, list[str]] = defaultdict(list)
    for cid, value in values:
        if value:
            by_value[value].append(cid)
    return {value: ids for value, ids in by_value.items() if len(ids) > 1}


def _valid_iso_date(value: Any) -> bool:
    if not isinstance(value, str):
        return False
    try:
        date.fromisoformat(value)
    except ValueError:
        return False
    return True


def validate_governance(components: list[dict[str, Any]], *, axes: Sequence[str]) -> tuple[list[str], list[str]]:
    errors: list[str] = []
    warnings: list[str] = []
    generator = load_generator()

    for item in components:
        cid = str(item.get("id", ""))
        for field in sorted(set(item) - COMPONENT_FIELDS):
            errors.append(f"{cid}: unknown component registry field `{field}`")
        if cid and not ID_RE.fullmatch(cid):
            errors.append(f"{cid}: id must match {ID_RE.pattern}")

        header = item.get("publicHeader")
        if isinstance(header, str) and header and not header.startswith("qtmaterial/widgets/"):
            errors.append(f"{cid}: publicHeader must live under qtmaterial/widgets/")

        if "maturityPolicy" not in item:
            errors.append(f"{cid}: maturityPolicy must be explicit")
        if not isinstance(item.get("releaseScope"), bool):
            errors.append(f"{cid}: releaseScope must be an explicit boolean")
        if not isinstance(item.get("referenceCandidate"), bool):
            errors.append(f"{cid}: referenceCandidate must be an explicit boolean")

        effective = str(item.get("maturity", "planned"))
        policy = item.get("maturityPolicy", "manual")
        maturity_axes = item.get("maturityAxes")
        if not isinstance(maturity_axes, dict):
            errors.append(f"{cid}: maturityAxes must be explicit")
            continue

        for axis in axes:
            if axis not in maturity_axes or maturity_axes.get(axis) is None:
                errors.append(f"{cid}: maturityAxes.{axis} must be evaluated")

        declared = str(item.get("maturity", "planned"))
        derived = generator.derived_maturity(maturity_axes)
        if declared != derived:
            errors.append(
                f"{cid}: declared maturity `{declared}` differs from "
                f"evaluated maturity `{derived}`"
            )

        reviewed = maturity_axes.get("lastReviewed")
        if not _valid_iso_date(reviewed):
            errors.append(f"{cid}: maturityAxes.lastReviewed must be an ISO date")
        elif date.fromisoformat(str(reviewed)) > date.today():
            errors.append(f"{cid}: maturityAxes.lastReviewed cannot be in the future")

        gaps = maturity_axes.get("gaps")
        if not isinstance(gaps, list):
            errors.append(f"{cid}: maturityAxes.gaps must be an array")
        elif any(not isinstance(value, str) or not value.strip() for value in gaps):
            errors.append(f"{cid}: maturityAxes.gaps must contain non-empty statements")
        elif declared == "complete" and gaps:
            errors.append(f"{cid}: complete component must have no maturity gaps")
        elif declared != "complete" and not gaps:
            errors.append(f"{cid}: non-complete component must declare a maturity gap")

        actions = maturity_axes.get("nextActions")
        if (
            not isinstance(actions, list)
            or not actions
            or any(not isinstance(value, str) or not value.strip() for value in actions)
        ):
            errors.append(f"{cid}: maturityAxes.nextActions must be non-empty")

        evidence = maturity_axes.get("evidence")
        if not isinstance(evidence, dict):
            errors.append(f"{cid}: maturityAxes.evidence must be an object")
        else:
            unknown_evidence = sorted(set(evidence) - set(axes) - {"notes"})
            for field in unknown_evidence:
                errors.append(f"{cid}: unknown maturityAxes.evidence field `{field}`")
            for axis in axes:
                values = evidence.get(axis)
                if (
                    not isinstance(values, list)
                    or not values
                    or any(not isinstance(value, str) or not value.strip() for value in values)
                ):
                    errors.append(
                        f"{cid}: maturityAxes.evidence.{axis} must contain "
                        "non-empty statements"
                    )
            notes = evidence.get("notes")
            if notes is not None and (
                not isinstance(notes, list)
                or not notes
                or any(not isinstance(value, str) or not value.strip() for value in notes)
            ):
                errors.append(
                    f"{cid}: maturityAxes.evidence.notes must contain "
                    "non-empty statements"
                )

        if effective == "complete":
            for axis in axes:
                if axis not in maturity_axes:
                    errors.append(f"{cid}: complete component missing maturityAxes.{axis}")
                    continue
                value = maturity_axes.get(axis)
                if value != "N/A" and value != 4:
                    errors.append(
                        f"{cid}: complete component maturityAxes.{axis} must be 4 or N/A, got {value!r}"
                    )

            if not isinstance(evidence, dict):
                errors.append(f"{cid}: complete component requires maturityAxes.evidence")

    uniqueness_fields = ["publicHeader", "widgetType", "galleryRoute"]
    for field in uniqueness_fields:
        duplicates = _duplicates(
            (str(item.get("id", "")), str(item.get(field, "")))
            for item in components
        )
        for value, ids in sorted(duplicates.items()):
            errors.append(f"{field} `{value}` is shared by multiple components: {', '.join(ids)}")

    references: dict[str, list[str]] = defaultdict(list)
    for item in components:
        if item.get("referenceCandidate") is True:
            cid = str(item.get("id", ""))
            family = str(item.get("family", ""))
            references[family].append(cid)
            if str(item.get("maturity", "")) != "complete":
                errors.append(f"{cid}: referenceCandidate must be complete")

    for family, ids in sorted(references.items()):
        if len(ids) > 1:
            errors.append(
                f"{family}: at most one referenceCandidate is allowed in registry v1; got {', '.join(ids)}"
            )

    return errors, warnings


def run_generated_check(*, strict: bool) -> int:
    command = [sys.executable, str(GENERATOR_PATH), "--check"]
    if strict:
        command.append("--strict")
    return subprocess.run(command, cwd=str(ROOT), check=False).returncode


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Validate QtMaterial3 component-registry governance.")
    parser.add_argument("--strict", action="store_true", help="treat governance and generator warnings as errors")
    parser.add_argument(
        "--check-generated",
        action="store_true",
        help="also verify STATUS.md/docs generated files are in sync",
    )
    args = parser.parse_args(argv)

    schema_errors = validate_schema_contract()

    try:
        generator = load_generator()
        components = generator.load_registry()
        base_errors, base_warnings = generator.validate_registry(components, strict=False)
    except Exception as exc:
        print(f"component registry validation failed to initialize: {exc}", file=sys.stderr)
        return 2

    governance_errors, governance_warnings = validate_governance(
        components, axes=generator.AXES
    )

    errors = schema_errors + list(base_errors) + governance_errors
    warnings = list(base_warnings) + governance_warnings

    if args.strict and warnings:
        errors.extend(warnings)
        warnings = []

    if warnings:
        print("component registry warnings:")
        for warning in warnings:
            print(f" - {warning}")

    if errors:
        print("component registry validation failed:")
        for error in errors:
            print(f" - {error}")
        return 1

    if args.check_generated:
        rc = run_generated_check(strict=args.strict)
        if rc != 0:
            return rc

    print(
        f"component registry OK: {len(components)} components, "
        "schema v1, governance invariants satisfied"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
