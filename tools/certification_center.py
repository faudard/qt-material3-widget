#!/usr/bin/env python3
"""Unified fail-closed certification workflow for the final QtMaterial3 60/60 closure."""

from __future__ import annotations

import argparse
import copy
import json
import subprocess
import sys
from datetime import date
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
RESULTS = {"pending", "pass", "fail"}

MILESTONES: dict[str, dict[str, Any]] = {
    "1.5": {
        "name": "Enterprise accessibility",
        "ledger": "docs/components/enterprise-accessibility-1.5.json",
        "checker": "tools/check_enterprise_accessibility.py",
        "promoter": "tools/promote_enterprise_1_5.py",
        "visual": False,
    },
    "1.7": {
        "name": "Missing Material 3 catalogue",
        "ledger": "docs/components/material3-catalogue-certification-1.7.json",
        "checker": "tools/check_material3_catalogue_1_7.py",
        "promoter": "tools/promote_material3_catalogue_1_7.py",
        "visual": True,
    },
    "1.9": {
        "name": "Adaptive / Desktop",
        "ledger": "docs/components/adaptive-desktop-certification-1.9.json",
        "checker": "tools/check_adaptive_desktop_1_9.py",
        "promoter": "tools/promote_adaptive_desktop_1_9.py",
        "visual": True,
    },
    "1.10/1.11": {
        "name": "Expressive Catalogue / Data",
        "ledger": "docs/components/expressive-certification-1.10-1.11.json",
        "checker": "tools/check_expressive_catalogue_1_10_1_11.py",
        "promoter": "tools/promote_expressive_1_10_1_11.py",
        "visual": True,
    },
}


def ledger_path(milestone: str, root: Path = ROOT) -> Path:
    return root / str(MILESTONES[milestone]["ledger"])


def load_ledger(milestone: str, root: Path = ROOT) -> dict[str, Any]:
    path = ledger_path(milestone, root)
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise ValueError(f"{path} must contain a JSON object")
    return payload


def save_ledger(milestone: str, payload: dict[str, Any], root: Path = ROOT) -> None:
    ledger_path(milestone, root).write_text(
        json.dumps(payload, indent=2) + "\n",
        encoding="utf-8",
    )


def checked_results(record: dict[str, Any]) -> list[str]:
    values: list[str] = []
    components = record.get("components")
    if not isinstance(components, dict):
        return values
    for checks in components.values():
        if not isinstance(checks, dict):
            continue
        for result in checks.values():
            if isinstance(result, str) and result in RESULTS:
                values.append(result)
    return values


def derive_platform_status(checks: dict[str, dict[str, str]]) -> str:
    values = [value for component in checks.values() for value in component.values()]
    if not values:
        raise ValueError("native AT session contains no checks")
    invalid = sorted({value for value in values if value not in RESULTS})
    if invalid:
        raise ValueError(f"invalid native AT results: {', '.join(invalid)}")
    if "pending" in values:
        return "pending"
    if "fail" in values:
        return "fail"
    return "pass"


def parse_review_date(value: str) -> str:
    value = value.strip()
    try:
        date.fromisoformat(value)
    except ValueError as error:
        raise ValueError("reviewedAt must use YYYY-MM-DD") from error
    return value


def build_session_template(
    milestone: str,
    platform: str,
    payload: dict[str, Any],
) -> dict[str, Any]:
    platforms = payload.get("platforms")
    if not isinstance(platforms, dict) or platform not in platforms:
        raise ValueError(f"{milestone} has no platform {platform!r}")
    record = platforms[platform]
    if not isinstance(record, dict):
        raise ValueError(f"{milestone}/{platform} platform record is invalid")
    components = record.get("components")
    if not isinstance(components, dict):
        raise ValueError(f"{milestone}/{platform} has no component contract")

    pending_components: dict[str, dict[str, str]] = {}
    for component_id, checks in components.items():
        if not isinstance(checks, dict):
            raise ValueError(f"{milestone}/{platform}/{component_id} checks are invalid")
        pending_components[component_id] = {str(check): "pending" for check in checks}

    return {
        "schemaVersion": 1,
        "milestone": milestone,
        "platform": platform,
        "screenReader": str(record.get("screenReader", "")),
        "reviewer": "",
        "reviewedAt": "",
        "evidence": "",
        "notes": "",
        "checks": pending_components,
    }


def validate_session(
    session: dict[str, Any],
    payload: dict[str, Any],
    *,
    require_final_results: bool,
) -> list[str]:
    errors: list[str] = []
    milestone = str(session.get("milestone", ""))
    platform = str(session.get("platform", ""))

    if session.get("schemaVersion") != 1:
        errors.append("session must use schemaVersion 1")

    platforms = payload.get("platforms")
    if not isinstance(platforms, dict):
        return errors + ["ledger platforms must be an object"]
    record = platforms.get(platform)
    if not isinstance(record, dict):
        return errors + [f"unknown platform {platform!r} for milestone {milestone}"]

    expected_reader = str(record.get("screenReader", ""))
    if session.get("screenReader") != expected_reader:
        errors.append(
            f"screenReader must be {expected_reader!r} for {platform}"
        )

    expected_components = record.get("components")
    checks = session.get("checks")
    if not isinstance(expected_components, dict) or not isinstance(checks, dict):
        return errors + ["session checks must match the ledger component contract"]

    if set(checks) != set(expected_components):
        errors.append("session component set does not match the ledger contract")

    all_values: list[str] = []
    for component_id, expected in expected_components.items():
        actual = checks.get(component_id)
        if not isinstance(expected, dict) or not isinstance(actual, dict):
            errors.append(f"{component_id} checks must be an object")
            continue
        if set(actual) != set(expected):
            errors.append(
                f"{component_id} check names do not match the ledger contract"
            )
            continue
        for check_name, result in actual.items():
            if result not in RESULTS:
                errors.append(
                    f"{component_id}/{check_name} has invalid result {result!r}"
                )
            else:
                all_values.append(result)

    if require_final_results and "pending" in all_values:
        errors.append("recorded native AT sessions cannot contain pending checks")

    if require_final_results:
        for field in ("reviewer", "reviewedAt", "evidence"):
            value = session.get(field)
            if not isinstance(value, str) or not value.strip():
                errors.append(f"recorded session requires non-empty {field}")
        reviewed_at = session.get("reviewedAt")
        if isinstance(reviewed_at, str) and reviewed_at.strip():
            try:
                parse_review_date(reviewed_at)
            except ValueError as error:
                errors.append(str(error))

    return errors


def apply_session(
    payload: dict[str, Any],
    session: dict[str, Any],
) -> dict[str, Any]:
    updated = copy.deepcopy(payload)
    platform = str(session["platform"])
    record = updated["platforms"][platform]
    checks = copy.deepcopy(session["checks"])
    status = derive_platform_status(checks)

    record["components"] = checks
    record["status"] = status
    record["reviewer"] = str(session["reviewer"]).strip()
    record["reviewedAt"] = parse_review_date(str(session["reviewedAt"]))
    record["evidence"] = str(session["evidence"]).strip()
    if "notes" in record:
        record["notes"] = str(session.get("notes", "")).strip()
    return updated


def apply_visual_review(
    payload: dict[str, Any],
    *,
    status: str,
    reviewer: str,
    reviewed_at: str,
    evidence: str,
) -> dict[str, Any]:
    if status not in {"pass", "fail"}:
        raise ValueError("visual review status must be pass or fail")
    reviewer = reviewer.strip()
    evidence = evidence.strip()
    if not reviewer:
        raise ValueError("visual review requires a reviewer")
    if not evidence:
        raise ValueError("visual review requires durable evidence")
    reviewed_at = parse_review_date(reviewed_at)

    updated = copy.deepcopy(payload)
    visual = updated.get("visual")
    if not isinstance(visual, dict):
        raise ValueError("ledger has no visual review record")

    previous_evidence = str(visual.get("evidence", "")).strip()
    manual_evidence = f"manual review: {evidence}"
    visual["status"] = status
    visual["reviewer"] = reviewer
    visual["reviewedAt"] = reviewed_at
    visual["evidence"] = (
        f"{previous_evidence}; {manual_evidence}"
        if previous_evidence
        else manual_evidence
    )
    return updated


def milestone_summary(
    milestone: str,
    payload: dict[str, Any],
    *,
    root: Path = ROOT,
) -> dict[str, Any]:
    platforms = payload.get("platforms")
    if not isinstance(platforms, dict):
        raise ValueError(f"{milestone} platforms must be an object")

    platform_summaries: dict[str, Any] = {}
    native_totals = {"pass": 0, "fail": 0, "pending": 0}
    for platform_id, record in platforms.items():
        if not isinstance(record, dict):
            continue
        values = checked_results(record)
        counts = {result: values.count(result) for result in ("pass", "fail", "pending")}
        for result, count in counts.items():
            native_totals[result] += count
        platform_summaries[platform_id] = {
            "screenReader": record.get("screenReader", ""),
            "status": record.get("status", ""),
            "checks": counts,
            "reviewer": record.get("reviewer", ""),
            "reviewedAt": record.get("reviewedAt", ""),
            "evidence": bool(str(record.get("evidence", "")).strip()),
        }

    visual_summary: dict[str, Any] | None = None
    if MILESTONES[milestone]["visual"]:
        visual = payload.get("visual")
        if not isinstance(visual, dict):
            raise ValueError(f"{milestone} visual record must be an object")
        required = visual.get("requiredGoldens", [])
        missing = []
        if isinstance(required, list):
            missing = [
                str(item)
                for item in required
                if not (root / str(item)).is_file()
            ]
        visual_summary = {
            "status": visual.get("status", ""),
            "reviewer": visual.get("reviewer", ""),
            "reviewedAt": visual.get("reviewedAt", ""),
            "evidence": bool(str(visual.get("evidence", "")).strip()),
            "requiredGoldens": len(required) if isinstance(required, list) else 0,
            "missingGoldens": missing,
        }

    complete = (
        native_totals["pending"] == 0
        and native_totals["fail"] == 0
        and all(
            item.get("status") == "pass"
            and bool(str(item.get("reviewer", "")).strip())
            and bool(str(item.get("reviewedAt", "")).strip())
            and bool(item.get("evidence"))
            for item in platform_summaries.values()
        )
        and (
            visual_summary is None
            or (
                visual_summary["status"] == "pass"
                and bool(str(visual_summary.get("reviewer", "")).strip())
                and bool(str(visual_summary.get("reviewedAt", "")).strip())
                and bool(visual_summary.get("evidence"))
                and not visual_summary["missingGoldens"]
            )
        )
    )

    return {
        "milestone": milestone,
        "name": MILESTONES[milestone]["name"],
        "complete": complete,
        "nativeChecks": native_totals,
        "platforms": platform_summaries,
        "visual": visual_summary,
    }


def certification_status(root: Path = ROOT) -> dict[str, Any]:
    milestones = {
        milestone: milestone_summary(
            milestone,
            load_ledger(milestone, root),
            root=root,
        )
        for milestone in MILESTONES
    }
    return {
        "complete": all(item["complete"] for item in milestones.values()),
        "milestones": milestones,
    }


def print_status(status: dict[str, Any]) -> None:
    print("QtMaterial3 final certification status")
    print("=" * 38)
    for milestone, summary in status["milestones"].items():
        native = summary["nativeChecks"]
        state = "READY" if summary["complete"] else "BLOCKED"
        print(
            f"{milestone} {summary['name']}: {state} | "
            f"AT pass={native['pass']} fail={native['fail']} "
            f"pending={native['pending']}"
        )
        visual = summary.get("visual")
        if visual is not None:
            print(
                f"  visual={visual['status']} "
                f"goldens={visual['requiredGoldens']} "
                f"missing={len(visual['missingGoldens'])}"
            )
        for platform_id, platform in summary["platforms"].items():
            counts = platform["checks"]
            print(
                f"  {platform_id}/{platform['screenReader']}: "
                f"{platform['status']} "
                f"(pass={counts['pass']}, fail={counts['fail']}, "
                f"pending={counts['pending']})"
            )


def run_checked(command: list[str]) -> None:
    print("+", " ".join(command))
    subprocess.run(command, cwd=ROOT, check=True)


def command_status(args: argparse.Namespace) -> int:
    status = certification_status(ROOT)
    if args.json:
        print(json.dumps(status, indent=2))
    else:
        print_status(status)
    if args.require_complete and not status["complete"]:
        return 1
    return 0


def command_template(args: argparse.Namespace) -> int:
    payload = load_ledger(args.milestone)
    template = build_session_template(args.milestone, args.platform, payload)
    output = Path(args.output)
    if output.exists() and not args.force:
        raise ValueError(f"{output} already exists; pass --force to replace it")
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(template, indent=2) + "\n", encoding="utf-8")
    print(f"Wrote native AT session template: {output}")
    return 0


def command_record(args: argparse.Namespace) -> int:
    if not args.confirm_native_session:
        raise ValueError(
            "refusing to record native AT evidence without "
            "--confirm-native-session"
        )

    session_path = Path(args.session)
    session = json.loads(session_path.read_text(encoding="utf-8"))
    if not isinstance(session, dict):
        raise ValueError("session file must contain a JSON object")

    milestone = str(session.get("milestone", ""))
    if milestone not in MILESTONES:
        raise ValueError(f"unknown milestone {milestone!r}")

    payload = load_ledger(milestone)
    errors = validate_session(session, payload, require_final_results=True)
    if errors:
        raise ValueError("; ".join(errors))

    updated = apply_session(payload, session)
    save_ledger(milestone, updated)
    status = updated["platforms"][str(session["platform"])]["status"]
    print(
        f"Recorded {milestone}/{session['platform']} native AT session: "
        f"{status}"
    )
    return 0


def command_visual(args: argparse.Namespace) -> int:
    config = MILESTONES.get(args.milestone)
    if not isinstance(config, dict) or not config.get("visual", False):
        raise ValueError(
            f"visual review is not supported for milestone {args.milestone!r}"
        )
    if not args.confirm_reviewed_all_goldens:
        raise ValueError(
            "refusing to record visual review without "
            "--confirm-reviewed-all-goldens"
        )

    payload = load_ledger(args.milestone)
    visual = payload.get("visual")
    if not isinstance(visual, dict):
        raise ValueError("ledger has no visual review record")
    required = visual.get("requiredGoldens")
    if not isinstance(required, list) or not required:
        raise ValueError("visual review has no required goldens")
    missing = [str(item) for item in required if not (ROOT / str(item)).is_file()]
    if missing:
        raise ValueError(
            "cannot record visual review; missing goldens: " + ", ".join(missing)
        )

    updated = apply_visual_review(
        payload,
        status=args.status,
        reviewer=args.reviewer,
        reviewed_at=args.reviewed_at,
        evidence=args.evidence,
    )
    save_ledger(args.milestone, updated)
    print(f"Recorded {args.milestone} visual review: {args.status}")
    return 0


def command_promote(args: argparse.Namespace) -> int:
    status = certification_status(ROOT)
    if not status["complete"]:
        print_status(status)
        print("Final promotion is blocked by incomplete certification evidence.")
        return 1

    # Preflight every gate and every promotion helper before mutating the tree.
    for milestone, config in MILESTONES.items():
        run_checked(
            [
                sys.executable,
                str(ROOT / str(config["checker"])),
                "--require-complete",
            ]
        )
        run_checked([sys.executable, str(ROOT / str(config["promoter"]))])

    if not args.apply:
        print(
            "All final certification gates are complete. "
            "Run again with --apply to promote 1.5 -> 1.7 -> 1.9 -> 1.10/1.11 to 60/60."
        )
        return 0

    for milestone, config in MILESTONES.items():
        run_checked(
            [
                sys.executable,
                str(ROOT / str(config["promoter"])),
                "--apply",
            ]
        )

    print("Final QtMaterial3 certification promotion completed through 60/60.")
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)

    status_parser = subparsers.add_parser(
        "status",
        help="show the combined 1.5 / 1.7 / 1.9 / 1.10-1.11 certification state",
    )
    status_parser.add_argument("--json", action="store_true")
    status_parser.add_argument("--require-complete", action="store_true")
    status_parser.set_defaults(func=command_status)

    template_parser = subparsers.add_parser(
        "template",
        help="create a fail-closed native AT session template",
    )
    template_parser.add_argument(
        "--milestone",
        choices=tuple(MILESTONES),
        required=True,
    )
    template_parser.add_argument("--platform", required=True)
    template_parser.add_argument("--output", required=True)
    template_parser.add_argument("--force", action="store_true")
    template_parser.set_defaults(func=command_template)

    record_parser = subparsers.add_parser(
        "record",
        help="record one completed native AT session into its ledger",
    )
    record_parser.add_argument("--session", required=True)
    record_parser.add_argument(
        "--confirm-native-session",
        action="store_true",
        help="confirm that the real named screen reader session was executed",
    )
    record_parser.set_defaults(func=command_record)

    visual_parser = subparsers.add_parser(
        "review-visual",
        help="record explicit human review of committed visual certification goldens",
    )
    visual_parser.add_argument(
        "--milestone",
        choices=tuple(
            milestone
            for milestone, config in MILESTONES.items()
            if config.get("visual", False)
        ),
        required=True,
    )
    visual_parser.add_argument("--status", choices=("pass", "fail"), required=True)
    visual_parser.add_argument("--reviewer", required=True)
    visual_parser.add_argument("--reviewed-at", required=True)
    visual_parser.add_argument("--evidence", required=True)
    visual_parser.add_argument(
        "--confirm-reviewed-all-goldens",
        action="store_true",
        help="confirm every required committed golden was actually inspected",
    )
    visual_parser.set_defaults(func=command_visual)

    promote_parser = subparsers.add_parser(
        "promote",
        help="preflight or apply the complete 1.5 -> 1.7 -> 1.9 -> 1.10/1.11 promotion chain",
    )
    promote_parser.add_argument("--apply", action="store_true")
    promote_parser.set_defaults(func=command_promote)

    return parser


def main(argv: list[str]) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    try:
        return int(args.func(args))
    except (
        OSError,
        ValueError,
        json.JSONDecodeError,
        subprocess.CalledProcessError,
    ) as error:
        print(f"Certification Center failed: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
