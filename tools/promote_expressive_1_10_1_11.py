#!/usr/bin/env python3
"""Promote the five standalone 1.10/1.11 Expressive components after certification."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
TOOLS = ROOT / "tools"
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))

import check_expressive_catalogue_1_10_1_11 as certification  # noqa: E402
import check_release  # noqa: E402
import component_registry  # noqa: E402

REGISTRY = ROOT / "docs/components/component-registry.json"
RULES = ROOT / "tools/release_rules.json"
PRODUCTION_TEST_TARGET = "tst_expressive_production_contracts"

MINIMUM_AXES: dict[str, int | str] = {
    "api": 3,
    "rendering": 2,
    "states": 3,
    "accessibility": 2,
    "keyboard": 3,
    "hidpi": 2,
    "rtl": 2,
    "tests": 3,
    "example": 3,
    "docs": 3,
}


def _meets_minimum(value: Any, minimum: int | str) -> bool:
    if value == "N/A":
        return True
    return isinstance(value, int) and isinstance(minimum, int) and value >= minimum


def promotion_prerequisite_errors(
    components: list[dict[str, Any]],
    *,
    root: Path = ROOT,
) -> list[str]:
    errors: list[str] = []
    by_id = {str(item.get("id", "")): item for item in components}
    tests_cmake = (root / "tests/CMakeLists.txt").read_text(
        encoding="utf-8", errors="replace"
    )
    if PRODUCTION_TEST_TARGET not in tests_cmake:
        errors.append(
            f"production contract test {PRODUCTION_TEST_TARGET} is not registered"
        )

    for component_id in certification.PROMOTION_COMPONENTS:
        component = by_id.get(component_id)
        if component is None:
            errors.append(f"missing 1.10/1.11 component {component_id}")
            continue

        if component.get("maturityPolicy") != "derived":
            errors.append(f"{component_id} must retain derived maturity policy")

        public_header = root / "include" / str(component.get("publicHeader", ""))
        if not public_header.is_file():
            errors.append(f"{component_id} public header is missing")

        docs_path = root / str(component.get("docsPath", ""))
        if not docs_path.is_file():
            errors.append(f"{component_id} documentation is missing")

        test_target = str(component.get("testTarget", ""))
        if not test_target or test_target not in tests_cmake:
            errors.append(
                f"{component_id} test target {test_target!r} is not registered"
            )

        gallery_route = str(component.get("galleryRoute", "")).strip()
        if not gallery_route.startswith("/"):
            errors.append(f"{component_id} has no valid Gallery route")

        axes = component.get("maturityAxes")
        if not isinstance(axes, dict):
            errors.append(f"{component_id} has no maturity axes")
            continue

        for axis, minimum in MINIMUM_AXES.items():
            value = axes.get(axis)
            if not _meets_minimum(value, minimum):
                errors.append(
                    f"{component_id} has {axis}={value!r}; "
                    f"expected at least {minimum!r} before certification promotion"
                )

    return errors


def reviewed_at(payload: dict[str, Any]) -> str:
    dates = [
        str(payload["visual"].get("reviewedAt", "")).strip(),
        *[
            str(record.get("reviewedAt", "")).strip()
            for record in payload["platforms"].values()
        ],
    ]
    dates = [value for value in dates if value]
    if not dates:
        raise ValueError("1.10/1.11 certification contains no review dates")
    return max(dates)


def promote_registry(
    components: list[dict[str, Any]],
    *,
    reviewed_at_value: str,
) -> None:
    by_id = {str(item.get("id", "")): item for item in components}

    evidence_by_axis = {
        "api": (
            "The installed public-header/package contract includes the additive "
            "1.10/1.11 API and remains guarded by the repository public API checks."
        ),
        "rendering": (
            "Reviewed pinned Qt 6.4.0 / Fusion / xcb expressive_catalogue_matrix "
            "goldens cover light, dark, high-contrast, disabled, selected/variant "
            "and RTL presentation."
        ),
        "states": (
            "tst_expressive_catalogue and tst_expressive_production_contracts "
            "exercise state transitions, disabled behavior, selection and collapse/"
            "expand contracts."
        ),
        "accessibility": (
            "Native NVDA, Orca and VoiceOver certification is recorded in "
            "docs/components/expressive-certification-1.10-1.11.json."
        ),
        "keyboard": (
            "Focused production contracts cover keyboard traversal/activation; "
            "inherited keyboard behavior remains backed by complete base controls."
        ),
        "hidpi": (
            "tst_expressive_production_contracts renders the five standalone "
            "Expressive components into DPR 2.0 paint devices."
        ),
        "rtl": (
            "The reviewed Expressive matrix includes RTL rows and production "
            "contracts verify directional propagation/mirroring where applicable."
        ),
        "tests": (
            "Focused functional, production-contract, visual repeatability and "
            "fail-closed certification checker tests are registered in CI."
        ),
        "example": (
            "Each promoted component owns a stable Gallery route used by the "
            "Expressive catalogue demonstration."
        ),
        "docs": (
            "docs/public-api/expressive-catalogue.md documents the promoted "
            "components and their interaction/accessibility contracts."
        ),
    }
    maintenance = (
        "Maintain complete status by keeping the Expressive production contracts, "
        "reviewed visual goldens and native NVDA/Orca/VoiceOver evidence in sync."
    )

    for component_id in certification.PROMOTION_COMPONENTS:
        component = by_id[component_id]
        axes = component["maturityAxes"]
        evidence = axes.setdefault("evidence", {})

        for axis in component_registry.AXES:
            if axes.get(axis) == "N/A":
                continue
            axes[axis] = 4
            entries = evidence.setdefault(axis, [])
            statement = evidence_by_axis[axis]
            if statement not in entries:
                entries.append(statement)

        axes["lastReviewed"] = reviewed_at_value
        axes["gaps"] = []
        axes["nextActions"] = [maintenance]
        component["releaseScope"] = True
        component["referenceCandidate"] = False
        component["maturity"] = "complete"


def promote_rules(rules: dict[str, Any]) -> None:
    stable = rules["base"]["stable_release"]
    goldens = stable.setdefault("visual_goldens", [])
    for relative in certification.VISUAL_GOLDENS:
        if relative not in goldens:
            goldens.append(relative)

    prefixes = stable.setdefault("component_visual_prefixes", {})
    for component_id in certification.PROMOTION_COMPONENTS:
        prefixes[component_id] = "expressive_catalogue_matrix"


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--apply",
        action="store_true",
        help="write the 1.10/1.11 promotion after all evidence is complete",
    )
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    payload = certification.read_ledger()
    errors = certification.validate(ROOT, payload, require_complete=True)

    components = component_registry.load_registry(ROOT)
    errors.extend(promotion_prerequisite_errors(components))
    if errors:
        print("QtMaterial3 1.10/1.11 promotion is blocked:")
        for error in errors:
            print(f" - {error}")
        return 1

    if not args.apply:
        print(
            "QtMaterial3 1.10/1.11 promotion is eligible. "
            "Run again with --apply to write the final 60/60 promotion."
        )
        return 0

    rules = check_release.load_rules(RULES)
    promote_registry(
        components,
        reviewed_at_value=reviewed_at(payload),
    )
    promote_rules(rules)

    registry_errors, registry_warnings = component_registry.validate_registry(
        components,
        strict=True,
        root=ROOT,
    )
    for warning in registry_warnings:
        print(f"warning: {warning}")
    if registry_errors:
        print("Expressive promotion would violate the component registry:")
        for error in registry_errors:
            print(f" - {error}")
        return 1

    promoted = [
        component
        for component in components
        if component.get("id") in certification.PROMOTION_COMPONENTS
    ]
    enterprise_errors = check_release.validate_enterprise_components(
        promoted,
        rules["base"]["stable_release"],
    )
    if enterprise_errors:
        print("Expressive promotion would violate Enterprise component rules:")
        for error in enterprise_errors:
            print(f" - {error}")
        return 1

    REGISTRY.write_text(
        json.dumps(components, indent=2) + "\n",
        encoding="utf-8",
    )
    RULES.write_text(
        json.dumps(rules, indent=2) + "\n",
        encoding="utf-8",
    )

    subprocess.run(
        [
            sys.executable,
            str(ROOT / "scripts/generate_component_status.py"),
            "--strict",
        ],
        cwd=ROOT,
        check=True,
    )
    subprocess.run(
        [
            sys.executable,
            str(ROOT / "tools/check_release.py"),
            "--scope",
            "base",
        ],
        cwd=ROOT,
        check=True,
    )

    print(
        "QtMaterial3 1.10/1.11 Expressive standalone components promoted "
        "to complete/release scope; registry closure is 60/60."
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
