#!/usr/bin/env python3
"""Validate QtMaterial3 1.10/1.11 Expressive visual and native AT evidence."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
LEDGER = ROOT / "docs/components/expressive-certification-1.10-1.11.json"

PROMOTION_COMPONENTS = {
    "button.split": (
        "primaryAction",
        "secondaryAction",
        "focusOrder",
        "disabledState",
    ),
    "button.group": (
        "traversal",
        "checkedState",
        "activation",
        "rtlTraversal",
    ),
    "navigation.floating-toolbar": (
        "traversal",
        "collapsedState",
        "activation",
        "orientation",
    ),
    "progress.loading-indicator": (
        "busyAnnouncement",
        "idleAnnouncement",
        "reducedMotionState",
        "stateChange",
    ),
    "data.segmented-list": (
        "traversal",
        "selectionState",
        "activation",
        "segmentContext",
    ),
}

INTEGRATED_MODES = {
    "navigation.menu": (
        "traversal",
        "expressiveState",
        "activation",
        "focus",
    ),
    "compact.chip": (
        "checkedState",
        "activation",
        "removal",
        "focus",
    ),
}

COMPONENT_CHECKS = {**PROMOTION_COMPONENTS, **INTEGRATED_MODES}
PLATFORMS = {
    "windows-nvda": "NVDA",
    "linux-orca": "Orca",
    "macos-voiceover": "VoiceOver",
}
RESULTS = {"pending", "pass", "fail"}
VISUAL_GOLDENS = (
    "tests/visual/goldens/expressive_catalogue_matrix_light_standard.png",
    "tests/visual/goldens/expressive_catalogue_matrix_dark_standard.png",
    "tests/visual/goldens/expressive_catalogue_matrix_light_high.png",
)


def read_ledger(path: Path = LEDGER) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise ValueError("certification ledger must be an object")
    return payload


def validate(
    root: Path,
    payload: dict[str, Any],
    *,
    require_complete: bool,
) -> list[str]:
    errors: list[str] = []

    if payload.get("schemaVersion") != 1:
        errors.append("Expressive certification ledger must use schemaVersion 1")

    if payload.get("requiredComponents") != list(PROMOTION_COMPONENTS):
        errors.append("requiredComponents does not match the 1.10/1.11 promotion set")

    if payload.get("integratedModes") != list(INTEGRATED_MODES):
        errors.append("integratedModes does not match Menu/Chip Expressive modes")

    visual = payload.get("visual")
    if not isinstance(visual, dict):
        errors.append("visual evidence must be an object")
    else:
        if visual.get("prefix") != "expressive_catalogue_matrix":
            errors.append("visual prefix must be expressive_catalogue_matrix")
        status = visual.get("status")
        if status not in RESULTS:
            errors.append(f"visual evidence has invalid status {status!r}")
        if visual.get("requiredGoldens") != list(VISUAL_GOLDENS):
            errors.append("visual requiredGoldens does not match the contract")
        if require_complete:
            if status != "pass":
                errors.append(
                    f"visual review is {status!r}; promotion requires pass"
                )
            for field in ("reviewer", "reviewedAt", "evidence"):
                value = visual.get(field)
                if not isinstance(value, str) or not value.strip():
                    errors.append(f"visual review requires non-empty {field}")
            for relative in VISUAL_GOLDENS:
                if not (root / relative).is_file():
                    errors.append(f"reviewed golden is missing: {relative}")

    platforms = payload.get("platforms")
    if not isinstance(platforms, dict):
        return errors + ["platforms must be an object"]

    if set(platforms) != set(PLATFORMS):
        errors.append("platform set must be Windows/NVDA, Linux/Orca and macOS/VoiceOver")

    for platform_id, reader in PLATFORMS.items():
        record = platforms.get(platform_id)
        if not isinstance(record, dict):
            errors.append(f"evidence missing platform {platform_id}")
            continue

        if record.get("screenReader") != reader:
            errors.append(f"{platform_id} must record screenReader={reader}")

        status = record.get("status")
        if status not in RESULTS:
            errors.append(f"{platform_id} has invalid status {status!r}")

        components = record.get("components")
        if not isinstance(components, dict):
            errors.append(f"{platform_id} components must be an object")
            continue

        if set(components) != set(COMPONENT_CHECKS):
            errors.append(f"{platform_id} component set does not match the contract")

        all_pass = True
        for component_id, checks in COMPONENT_CHECKS.items():
            component = components.get(component_id)
            if not isinstance(component, dict):
                errors.append(f"{platform_id} missing component {component_id}")
                all_pass = False
                continue

            if set(component) != set(checks):
                errors.append(
                    f"{platform_id}/{component_id} checks do not match the contract"
                )
                all_pass = False
                continue

            for check in checks:
                result = component.get(check)
                if result not in RESULTS:
                    errors.append(
                        f"{platform_id}/{component_id}/{check} "
                        f"has invalid result {result!r}"
                    )
                    all_pass = False
                elif result != "pass":
                    all_pass = False

        if status == "pass" and not all_pass:
            errors.append(
                f"{platform_id} cannot be pass while component checks are incomplete"
            )

        if require_complete:
            if status != "pass":
                errors.append(
                    f"{platform_id} certification is {status!r}; "
                    "promotion requires pass"
                )
            for field in ("reviewer", "reviewedAt", "evidence"):
                value = record.get(field)
                if not isinstance(value, str) or not value.strip():
                    errors.append(f"{platform_id} requires non-empty {field}")

    return errors


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--require-complete",
        action="store_true",
        help="require reviewed goldens and all native AT checks to pass",
    )
    parser.add_argument("--ledger", type=Path, default=LEDGER)
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    try:
        payload = read_ledger(args.ledger)
        errors = validate(ROOT, payload, require_complete=args.require_complete)
    except (OSError, ValueError, json.JSONDecodeError) as error:
        print(f"Expressive certification evidence invalid: {error}")
        return 2

    if errors:
        print("QtMaterial3 1.10/1.11 certification is blocked:")
        for error in errors:
            print(f" - {error}")
        return 1

    mode = "complete" if args.require_complete else "structural"
    print(f"QtMaterial3 1.10/1.11 certification evidence OK ({mode})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
