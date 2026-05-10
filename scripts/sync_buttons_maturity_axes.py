#!/usr/bin/env python3
from __future__ import annotations

import argparse
import difflib
import importlib.util
import json
import sys
from datetime import date
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
REGISTRY = ROOT / "docs" / "components" / "component-registry.json"
AUDITOR = ROOT / "scripts" / "audit_buttons_maturity_axes.py"
BUTTON_IDS = [
    "button.text",
    "button.filled",
    "button.filled-tonal",
    "button.outlined",
    "button.elevated",
    "button.icon",
    "button.fab",
    "button.extended-fab",
]


class SyncError(RuntimeError):
    pass


def load_auditor_module():
    if not AUDITOR.exists():
        raise SyncError("scripts/audit_buttons_maturity_axes.py is missing; run apply_buttons_axis_audit.py first")

    module_name = "audit_buttons_maturity_axes"
    spec = importlib.util.spec_from_file_location(module_name, AUDITOR)
    if spec is None or spec.loader is None:
        raise SyncError("could not load audit_buttons_maturity_axes.py")

    module = importlib.util.module_from_spec(spec)

    previous = sys.modules.get(module_name)
    sys.modules[module_name] = module
    try:
        spec.loader.exec_module(module)
    except Exception:
        if previous is None:
            sys.modules.pop(module_name, None)
        else:
            sys.modules[module_name] = previous
        raise

    return module


def load_registry() -> list[dict[str, Any]]:
    try:
        data = json.loads(REGISTRY.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        raise SyncError(f"invalid JSON in {REGISTRY}: {exc}") from exc

    if not isinstance(data, list):
        raise SyncError("component-registry.json must contain a JSON array")

    for index, item in enumerate(data):
        if not isinstance(item, dict):
            raise SyncError(f"registry item #{index} must be an object")

    return data


def dump_registry(data: list[dict[str, Any]]) -> str:
    return json.dumps(data, indent=2, ensure_ascii=False) + "\n"


def registry_by_id(data: list[dict[str, Any]]) -> dict[str, dict[str, Any]]:
    return {str(item.get("id", "")): item for item in data}


def unique_keep_order(values: list[str]) -> list[str]:
    seen: set[str] = set()
    result: list[str] = []
    for value in values:
        if value not in seen:
            result.append(value)
            seen.add(value)
    return result


def sync_button(item: dict[str, Any], audit: Any, *, force_failing_to_null: bool) -> tuple[bool, list[str]]:
    changed = False
    messages: list[str] = []
    axes = item.setdefault("maturityAxes", {})
    if not isinstance(axes, dict):
        axes = {}
        item["maturityAxes"] = axes
        changed = True

    evidence = axes.setdefault("evidence", {})
    if not isinstance(evidence, dict):
        evidence = {}
        axes["evidence"] = evidence
        changed = True

    gaps: list[str] = []

    for axis, result in audit.axes.items():
        target = result.target_value
        current_value = axes.get(axis)

        if result.ok:
            if current_value != target:
                axes[axis] = target
                changed = True
                messages.append(f"{audit.button_id}.{axis} -> {target}")
            current_evidence = evidence.get(axis)
            if current_evidence != result.evidence:
                evidence[axis] = list(result.evidence)
                changed = True
        else:
            if force_failing_to_null and axes.get(axis) != None:
                axes[axis] = None
                changed = True
                messages.append(f"{audit.button_id}.{axis} -> null")
            for blocker in result.blockers:
                gaps.append(f"{axis}: {blocker}")

    complete_ready = audit.complete_ready
    desired_maturity = "complete" if complete_ready else "usable"
    if item.get("maturity") != desired_maturity:
        item["maturity"] = desired_maturity
        changed = True
        messages.append(f"{audit.button_id}.maturity -> {desired_maturity}")

    if item.get("maturityPolicy") != "manual":
        item["maturityPolicy"] = "manual"
        changed = True

    if item.get("releaseScope") is not True:
        item["releaseScope"] = True
        changed = True

    last_reviewed = date.today().isoformat()
    if axes.get("lastReviewed") != last_reviewed:
        axes["lastReviewed"] = last_reviewed
        changed = True

    deduped_gaps = unique_keep_order(gaps)
    if axes.get("gaps") != deduped_gaps:
        axes["gaps"] = deduped_gaps
        changed = True

    desired_actions = [
        "Resolve remaining axis blockers before complete promotion."
    ] if deduped_gaps else [
        "Maintain complete status by keeping registry, tests, gallery and documentation evidence in sync."
    ]
    if axes.get("nextActions") != desired_actions:
        axes["nextActions"] = desired_actions
        changed = True

    return changed, messages


def unified_diff(old: str, new: str, from_file: str, to_file: str) -> str:
    return "\n".join(
        difflib.unified_diff(
            old.splitlines(),
            new.splitlines(),
            fromfile=from_file,
            tofile=to_file,
            lineterm="",
        )
    )


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Synchronize Buttons maturity axes from the axis auditor.")

    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--dry-run", action="store_true", help="show diff without writing; default")
    mode.add_argument("--apply", action="store_true", help="write component-registry.json")

    parser.add_argument("--build-dir", type=Path, default=None, help="build directory used with --run-ctest")
    parser.add_argument("--run-ctest", action="store_true", help="run focused Buttons CTest suite through the auditor")
    parser.add_argument("--run-generator-check", action="store_true", help="run generate_component_status.py --check through the auditor")
    parser.add_argument("--force-failing-to-null", action="store_true", help="reset failing axes to null instead of preserving their current score")
    parser.add_argument("--fail-on-blockers", action="store_true", help="return non-zero when any button is not complete-ready")
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    dry_run = not args.apply

    try:
        auditor = load_auditor_module()
        data = load_registry()
        by_id = registry_by_id(data)

        audits, diagnostics = auditor.audit_all(
            build_dir=args.build_dir,
            run_ctest=args.run_ctest,
            run_generator_check=args.run_generator_check,
        )

        missing = [button_id for button_id in BUTTON_IDS if button_id not in by_id]
        if missing:
            raise SyncError("missing button registry entries: " + ", ".join(missing))

        before = REGISTRY.read_text(encoding="utf-8")
        messages: list[str] = []

        for audit in audits:
            if audit.button_id not in BUTTON_IDS:
                continue
            _, updates = sync_button(
                by_id[audit.button_id],
                audit,
                force_failing_to_null=args.force_failing_to_null,
            )
            messages.extend(updates)

        after = dump_registry(data)
        blockers = [audit for audit in audits if not audit.complete_ready]
        diagnostic_failures = [item for item in diagnostics if "FAIL" in item]

        print("Buttons maturity axis sync")
        print(f"registry: {REGISTRY}")
        print(f"mode: {'dry-run' if dry_run else 'apply'}")
        print()

        if diagnostics:
            print("Diagnostics:")
            for diagnostic in diagnostics:
                if diagnostic:
                    print(f" - {diagnostic.replace(chr(10), chr(10) + '   ')}")
            print()

        if messages:
            print("Planned score updates:")
            for message in messages:
                print(f" - {message}")
            print()
        else:
            print("No score updates needed.")
            print()

        if blockers:
            print("Buttons still not complete-ready:")
            for audit in blockers:
                failed_axes = [axis for axis, result in audit.axes.items() if not result.ok]
                print(f" - {audit.button_id}: {', '.join(failed_axes)}")
            print()
        else:
            print("All Buttons are complete-ready according to the axis auditor.")
            print()

        if before == after:
            print("Registry is already synchronized.")
            return 1 if args.fail_on_blockers and (blockers or diagnostic_failures) else 0

        if dry_run:
            print(unified_diff(
                before,
                after,
                "docs/components/component-registry.json current",
                "docs/components/component-registry.json synchronized",
            ))
            print()
            print("Dry-run only. Re-run with --apply to write the registry.")
            return 1 if args.fail_on_blockers and (blockers or diagnostic_failures) else 0

        REGISTRY.write_text(after, encoding="utf-8")
        print("wrote docs/components/component-registry.json")
        print()
        print("Recommended next commands:")
        print("  python scripts/generate_component_status.py")
        print("  python scripts/audit_buttons_maturity_axes.py --verbose")
        print("  python scripts/sync_buttons_maturity_axes.py --run-generator-check --fail-on-blockers")
        return 1 if args.fail_on_blockers and (blockers or diagnostic_failures) else 0

    except SyncError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
