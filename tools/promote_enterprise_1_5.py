#!/usr/bin/env python3
"""Promote the six remaining 1.5 components after native AT certification."""

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

import check_release  # noqa: E402
import component_registry  # noqa: E402

REGISTRY = ROOT / "docs/components/component-registry.json"
RULES = ROOT / "tools/release_rules.json"


def promotion_prerequisite_errors(
    components: list[dict[str, Any]],
) -> list[str]:
    errors: list[str] = []
    by_id = {str(item.get("id", "")): item for item in components}

    for component_id in check_release.ENTERPRISE_AT_COMPONENTS:
        component = by_id.get(component_id)
        if component is None:
            errors.append(f"missing Enterprise component {component_id}")
            continue
        if not component.get("releaseScope", False):
            errors.append(f"{component_id} is not release scoped")

        axes = component.get("maturityAxes")
        if not isinstance(axes, dict):
            errors.append(f"{component_id} has no maturity axes")
            continue

        for axis in component_registry.AXES:
            if axis == "accessibility":
                continue
            value = axes.get(axis)
            if value not in (4, "N/A"):
                errors.append(
                    f"{component_id} has {axis}={value!r}; "
                    "all non-accessibility axes must already be 4/4 or N/A"
                )

        if axes.get("accessibility") not in (3, 4):
            errors.append(
                f"{component_id} has accessibility="
                f"{axes.get('accessibility')!r}; expected 3 or 4"
            )

    return errors


def promote_registry(
    components: list[dict[str, Any]],
    *,
    reviewed_at: str,
) -> None:
    by_id = {str(item.get("id", "")): item for item in components}
    evidence_text = (
        "Native NVDA, Orca and VoiceOver certification is recorded in "
        "docs/components/enterprise-accessibility-1.5.json; traversal, "
        "state announcements, activation and focus checks pass on all "
        "three platform readers."
    )
    maintenance = (
        "Maintain complete status by keeping automated contracts, reviewed "
        "visual evidence and native NVDA/Orca/VoiceOver evidence in sync."
    )

    for component_id in check_release.ENTERPRISE_AT_COMPONENTS:
        component = by_id[component_id]
        axes = component["maturityAxes"]
        axes["accessibility"] = 4
        axes["lastReviewed"] = reviewed_at
        axes["gaps"] = []
        axes["nextActions"] = [maintenance]
        evidence = axes.setdefault("evidence", {})
        accessibility = evidence.setdefault("accessibility", [])
        if evidence_text not in accessibility:
            accessibility.append(evidence_text)
        component["maturity"] = "complete"


def promote_rules(rules: dict[str, Any]) -> None:
    rules["base"]["stable_release"]["enterprise_complete"] = True


def reviewed_at_from_evidence(
    root: Path,
    stable: dict[str, Any],
) -> str:
    path = root / str(stable["accessibility_evidence"])
    payload = json.loads(path.read_text(encoding="utf-8"))
    dates = [
        str(record.get("reviewedAt", "")).strip()
        for record in payload["platforms"].values()
        if str(record.get("reviewedAt", "")).strip()
    ]
    return max(dates)


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Promote the final six QtMaterial3 1.5 components only after "
            "NVDA/Orca/VoiceOver evidence is complete."
        )
    )
    parser.add_argument(
        "--apply",
        action="store_true",
        help="write the promotion; without this flag only validate eligibility",
    )
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    rules = check_release.load_rules(RULES)
    stable = rules["base"]["stable_release"]

    errors = check_release.validate_enterprise_accessibility_evidence(
        ROOT,
        stable,
        require_complete=True,
    )
    components = component_registry.load_registry(ROOT)
    errors.extend(promotion_prerequisite_errors(components))
    if errors:
        print("Enterprise 1.5 promotion is blocked:")
        for error in errors:
            print(f" - {error}")
        return 1

    if not args.apply:
        print(
            "Enterprise 1.5 promotion is eligible. "
            "Run again with --apply to write 49/49 closure."
        )
        return 0

    reviewed_at = reviewed_at_from_evidence(ROOT, stable)
    promote_registry(components, reviewed_at=reviewed_at)
    promote_rules(rules)

    registry_errors, registry_warnings = component_registry.validate_registry(
        components,
        strict=True,
        root=ROOT,
    )
    if registry_warnings:
        for warning in registry_warnings:
            print(f"warning: {warning}")
    if registry_errors:
        print("Promotion would violate the component registry:")
        for error in registry_errors:
            print(f" - {error}")
        return 1

    enterprise_errors = check_release.validate_enterprise_components(
        components,
        rules["base"]["stable_release"],
    )
    if enterprise_errors:
        print("Promotion would violate Enterprise release rules:")
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
        "QtMaterial3 1.5 Enterprise promoted to 49/49 complete; "
        "enterprise_complete=true."
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
