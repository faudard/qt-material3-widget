#!/usr/bin/env python3
"""Promote 1.9 Adaptive/Desktop after reviewed visuals and native AT evidence."""

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

import check_adaptive_desktop_1_9 as certification  # noqa: E402
import check_release  # noqa: E402
import component_registry  # noqa: E402

REGISTRY = ROOT / "docs/components/component-registry.json"
RULES = ROOT / "tools/release_rules.json"

REPRESENTATIVE_PREFIXES = {
    "navigation.suite": "adaptive_desktop_compact_ltr",
    "layout.adaptive-shell": "adaptive_desktop_expanded_ltr",
}


def promotion_prerequisite_errors(
    components: list[dict[str, Any]],
) -> list[str]:
    errors: list[str] = []
    by_id = {str(item.get("id", "")): item for item in components}

    for component_id in certification.COMPONENT_CHECKS:
        component = by_id.get(component_id)
        if component is None:
            errors.append(f"missing 1.9 component {component_id}")
            continue

        axes = component.get("maturityAxes")
        if not isinstance(axes, dict):
            errors.append(f"{component_id} has no maturity axes")
            continue

        for axis in component_registry.AXES:
            if axis in {"rendering", "accessibility"}:
                continue
            value = axes.get(axis)
            if value not in (4, "N/A"):
                errors.append(
                    f"{component_id} has {axis}={value!r}; "
                    "all non-visual/non-AT axes must be 4/4 or N/A"
                )

        for axis in ("rendering", "accessibility"):
            if axes.get(axis) not in (3, 4):
                errors.append(
                    f"{component_id} has {axis}={axes.get(axis)!r}; "
                    "expected 3 or 4 before promotion"
                )

    return errors


def promote_registry(
    components: list[dict[str, Any]],
    *,
    reviewed_at: str,
) -> None:
    by_id = {str(item.get("id", "")): item for item in components}
    visual_evidence = (
        "Reviewed pinned Qt 6.4.0 / Fusion / xcb Adaptive/Desktop goldens "
        "cover Compact, Medium, Expanded, Large and ExtraLarge in LTR/RTL "
        "and light/dark/high-contrast variants; certification is recorded "
        "in docs/components/adaptive-desktop-certification-1.9.json."
    )
    accessibility_evidence = (
        "Native NVDA, Orca and VoiceOver certification for responsive "
        "Navigation Suite and Adaptive Shell transitions is recorded in "
        "docs/components/adaptive-desktop-certification-1.9.json."
    )
    maintenance = (
        "Maintain complete status by keeping 1.9 breakpoint, density, RTL, "
        "visual and native assistive-technology evidence in sync."
    )

    for component_id in certification.COMPONENT_CHECKS:
        component = by_id[component_id]
        axes = component["maturityAxes"]
        axes["rendering"] = 4
        axes["accessibility"] = 4
        axes["lastReviewed"] = reviewed_at
        axes["gaps"] = []
        axes["nextActions"] = [maintenance]

        evidence = axes.setdefault("evidence", {})
        rendering = evidence.setdefault("rendering", [])
        if visual_evidence not in rendering:
            rendering.append(visual_evidence)
        accessibility = evidence.setdefault("accessibility", [])
        if accessibility_evidence not in accessibility:
            accessibility.append(accessibility_evidence)

        component["releaseScope"] = True
        component["referenceCandidate"] = False
        component["maturity"] = "complete"


def promote_rules(rules: dict[str, Any]) -> None:
    stable = rules["base"]["stable_release"]
    golden_list = stable.setdefault("visual_goldens", [])
    for relative in certification.VISUAL_GOLDENS:
        if relative not in golden_list:
            golden_list.append(relative)

    prefixes = stable.setdefault("component_visual_prefixes", {})
    for component_id, prefix in REPRESENTATIVE_PREFIXES.items():
        prefixes[component_id] = prefix


def reviewed_at(payload: dict[str, Any]) -> str:
    dates = [
        str(payload["visual"].get("reviewedAt", "")).strip(),
        *[
            str(record.get("reviewedAt", "")).strip()
            for record in payload["platforms"].values()
        ],
    ]
    dates = [date for date in dates if date]
    if not dates:
        raise ValueError("1.9 certification contains no review dates")
    return max(dates)


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--apply",
        action="store_true",
        help="write the 1.9 promotion after all evidence is complete",
    )
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    payload = certification.read_ledger()
    errors = certification.validate(
        ROOT,
        payload,
        require_complete=True,
    )

    components = component_registry.load_registry(ROOT)
    errors.extend(promotion_prerequisite_errors(components))
    if errors:
        print("QtMaterial3 1.9 promotion is blocked:")
        for error in errors:
            print(f" - {error}")
        return 1

    if not args.apply:
        print(
            "QtMaterial3 1.9 promotion is eligible. "
            "Run again with --apply to write complete/release-scope status."
        )
        return 0

    rules = check_release.load_rules(RULES)
    promote_registry(components, reviewed_at=reviewed_at(payload))
    promote_rules(rules)

    registry_errors, registry_warnings = component_registry.validate_registry(
        components,
        strict=True,
        root=ROOT,
    )
    for warning in registry_warnings:
        print(f"warning: {warning}")
    if registry_errors:
        print("1.9 promotion would violate the component registry:")
        for error in registry_errors:
            print(f" - {error}")
        return 1

    promoted = [
        component
        for component in components
        if component.get("id") in certification.COMPONENT_CHECKS
    ]
    enterprise_errors = check_release.validate_enterprise_components(
        promoted,
        rules["base"]["stable_release"],
    )
    if enterprise_errors:
        print("1.9 promotion would violate Enterprise component rules:")
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
        "QtMaterial3 1.9 Adaptive/Desktop promoted to complete and release scope."
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
