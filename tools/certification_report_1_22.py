#!/usr/bin/env python3
"""1.22 Quality & Certification: derived, fail-closed release-health report.

Reads the canonical registry and existing native-AT / visual ledgers. This is
read-only: no registry score, evidence, golden or release status is promoted.
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from collections import Counter
from datetime import date
from pathlib import Path, PurePosixPath

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import certification_center as center
import component_registry as registry

READERS = {"windows-nvda": "NVDA", "linux-orca": "Orca", "macos-voiceover": "VoiceOver"}


def attributed(record):
    day = record.get("reviewedAt")
    if not isinstance(day, str) or not re.fullmatch(r"\d{4}-\d{2}-\d{2}", day):
        return False
    try:
        date.fromisoformat(day)
    except ValueError:
        return False
    return all(isinstance(record.get(key), str) and record[key].strip()
               for key in ("reviewer", "evidence"))


def safe_path(path):
    return (isinstance(path, str) and bool(path) and "\\" not in path
            and not PurePosixPath(path).is_absolute()
            and all(segment not in ("", ".", "..") for segment in path.split("/")))


def build_report(root=ROOT):
    """Invalid contracts become issues. Pending manual reviews are blockers, not issues."""
    issues = []
    components = {}
    for item in registry.load_registry(root):
        cid = item["id"]
        if cid in components:
            issues.append(f"duplicate component {cid}")
            continue
        axes = item.get("maturityAxes", {})
        maturity = registry.effective_maturity(item)
        if item.get("maturity") != maturity:
            issues.append(f"{cid}: declared/effective maturity mismatch")
        gaps = [{"axis": key, "score": axes.get(key)} for key in registry.AXES
                if axes.get(key) != "N/A" and axes.get(key) != 4]
        components[cid] = {
            "id": cid, "name": item.get("name", cid), "maturity": maturity,
            "releaseScope": item.get("releaseScope", True),
            "axisGaps": gaps, "declaredGaps": axes.get("gaps", []),
            "nextActions": axes.get("nextActions", []),
            "registryEvidence": "docs/components/component-registry.json",
            "nativeAT": [], "visual": [], "blockers": [],
        }

    for version, config in center.MILESTONES.items():
        source = config["ledger"]
        ledger = center.load_ledger(version, root)
        if ledger.get("schemaVersion") != 1:
            issues.append(f"{source}: unsupported schemaVersion")
        required = ledger.get("requiredComponents")
        if not isinstance(required, list) or not required or len(set(required)) != len(required):
            issues.append(f"{source}: invalid or duplicate requiredComponents")
            continue
        platforms = ledger.get("platforms")
        if not isinstance(platforms, dict):
            issues.append(f"{source}: missing platforms")
            platforms = {}
        if set(platforms) != set(READERS):
            issues.append(f"{source}: expected three native screen-reader platforms")
        for cid in required:
            if cid not in components:
                issues.append(f"{source}: unknown required component {cid}")
                continue
            sessions = []
            for platform, reader in READERS.items():
                record = platforms.get(platform, {})
                checks = record.get("components", {}).get(cid) if isinstance(record.get("components"), dict) else None
                if record.get("screenReader") != reader:
                    issues.append(f"{source}/{platform}: wrong screen reader")
                if not isinstance(checks, dict) or not checks:
                    issues.append(f"{source}/{platform}/{cid}: missing check contract")
                    checks = {}
                invalid = [key for key, value in checks.items() if value not in center.RESULTS]
                if invalid:
                    issues.append(f"{source}/{platform}/{cid}: invalid results {invalid}")
                counts = Counter(checks.values())
                passed = bool(checks) and all(value == "pass" for value in checks.values())
                certified = record.get("status") == "pass" and attributed(record) and passed
                sessions.append({
                    "platform": platform, "screenReader": reader, "status": record.get("status", "pending"),
                    "certified": bool(certified),
                    "checks": {state: counts[state] for state in ("pass", "fail", "pending")},
                    "pendingChecks": [name for name, state in checks.items() if state != "pass"],
                    "reviewer": record.get("reviewer", ""), "reviewedAt": record.get("reviewedAt", ""),
                    "evidence": record.get("evidence", ""),
                })
            at_complete = all(s["certified"] for s in sessions)
            components[cid]["nativeAT"].append({
                "milestone": version, "ledger": source, "certified": at_complete,
                "platforms": sessions,
            })
            if not at_complete:
                components[cid]["blockers"].append(f"{version}: native AT review pending")

        for platform, record in platforms.items():
            if not isinstance(record, dict):
                issues.append(f"{source}/{platform}: invalid platform record")
                continue
            if record.get("status") not in center.RESULTS:
                issues.append(f"{source}/{platform}: invalid status")
            if record.get("status") == "pass":
                checks_by_component = record.get("components", {})
                if not attributed(record) or not isinstance(checks_by_component, dict) or any(
                    not isinstance(checks, dict) or not checks or any(v != "pass" for v in checks.values())
                    for checks in checks_by_component.values()
                ):
                    issues.append(f"{source}/{platform}: unsubstantiated native AT pass")

        if config["visual"]:
            visual = ledger.get("visual")
            if not isinstance(visual, dict):
                issues.append(f"{source}: missing visual record")
                visual = {}
            goldens = visual.get("requiredGoldens", [])
            if not isinstance(goldens, list):
                issues.append(f"{source}: invalid requiredGoldens")
                goldens = []
            if not goldens or len(set(goldens)) != len(goldens):
                issues.append(f"{source}: missing or duplicated requiredGoldens")
            for path in goldens:
                if not safe_path(path):
                    issues.append(f"{source}: unsafe golden path {path!r}")
            missing = [path for path in goldens if safe_path(path) and not (root / path).is_file()]
            renderer = visual.get("renderer")
            if not isinstance(renderer, dict) or not all(
                isinstance(renderer.get(key), str) and renderer[key].strip()
                for key in ("qtVersion", "style", "platform", "scaleFactor", "fontDpi")
            ):
                issues.append(f"{source}: renderer provenance incomplete")
            certified = (visual.get("status") == "pass" and attributed(visual)
                         and bool(goldens) and not missing)
            if visual.get("status") not in center.RESULTS:
                issues.append(f"{source}: invalid visual status")
            if visual.get("status") == "pass" and not certified:
                issues.append(f"{source}: visual pass lacks human review or committed goldens")
            for cid in required:
                if cid in components:
                    components[cid]["visual"].append({
                        "milestone": version, "ledger": source, "status": visual.get("status", "pending"),
                        "certified": bool(certified), "renderer": renderer,
                        "requiredGoldens": goldens, "missingGoldens": missing,
                        "reviewer": visual.get("reviewer", ""), "reviewedAt": visual.get("reviewedAt", ""),
                        "evidence": visual.get("evidence", ""),
                    })
                    if not certified:
                        components[cid]["blockers"].append(f"{version}: human visual review pending")

    for cid, row in components.items():
        if row["releaseScope"] and row["maturity"] != "complete":
            if not row["nativeAT"]:
                issues.append(f"{cid}: release component has no native AT certification ledger")
            row["blockers"].extend(f"registry axis {gap['axis']}={gap['score']}"
                                   for gap in row["axisGaps"])
    rows = sorted(components.values(), key=lambda c: (c["maturity"] == "complete", c["id"]))
    counts = Counter(row["maturity"] for row in rows)
    blocked = sum(bool(row["blockers"]) and row["releaseScope"] for row in rows)
    return {
        "schemaVersion": 1, "milestone": "1.22.0",
        "summary": {
            "tracked": len(rows), "maturity": {k: counts[k] for k in registry.MATURITY_ORDER},
            "notComplete": sum(row["maturity"] != "complete" for row in rows),
            "releaseBlocked": blocked, "validationIssues": len(issues),
            "readyForPromotion": blocked == 0 and not issues,
        },
        "issues": issues, "components": rows,
    }


def markdown(data):
    summary = data["summary"]
    lines = [
        "# QtMaterial3 1.22 — derived certification health", "",
        "> Snapshot only. Native AT spoken output and visual references require actual human review.",
        "> The component registry remains authoritative; this report never promotes scores.", "",
        f"**{summary['maturity']['complete']}/{summary['tracked']} complete**, "
        f"{summary['notComplete']} incomplete, {summary['releaseBlocked']} release-blocked, "
        f"{summary['validationIssues']} evidence-contract issues.", "",
        "| Component | Maturity gaps | Native AT | Visual evidence | Provenance |",
        "| --- | --- | --- | --- | --- |",
    ]
    for c in data["components"]:
        if not c["releaseScope"] or not c["blockers"]:
            continue
        axes = ", ".join(f"{a['axis']}={a['score']}" for a in c["axisGaps"]) or "none"
        at = ", ".join(f"{x['milestone']}:{'approved' if x['certified'] else 'pending'}"
                       for x in c["nativeAT"]) or "unmapped"
        visual = ", ".join(
            f"{x['milestone']}:{'approved' if x['certified'] else 'pending'} "
            f"({len(x['requiredGoldens']) - len(x['missingGoldens'])}/"
            f"{len(x['requiredGoldens'])} files)"
            for x in c["visual"]
        ) or "N/A"
        sources = ", ".join(x["ledger"] for x in c["nativeAT"]) or c["registryEvidence"]
        lines.append(f"| {c['id']} | {axes} | {at} | {visual} | {sources} |")
    lines += ["", "## Outstanding validation issues", ""]
    if data["issues"]:
        lines.extend("- " + error for error in data["issues"])
    else:
        lines.append("No malformed evidence contracts. Pending reviews remain open blockers.")
    lines += ["", "## Next actions", ""]
    for c in data["components"]:
        if c["releaseScope"] and c["blockers"]:
            lines.append("### " + c["id"])
            lines.extend("- " + str(action) for action in c["nextActions"])
            for gate in c["nativeAT"]:
                for platform in gate["platforms"]:
                    if not platform["certified"]:
                        lines.append(f"- {gate['ledger']} / {platform['platform']}: "
                                     f"{platform['checks']['pending']} pending, "
                                     f"{platform['checks']['fail']} failed native AT checks")
            for visual in c["visual"]:
                if not visual["certified"]:
                    lines.append(f"- {visual['ledger']}: visual review {visual['status']}; "
                                 f"{len(visual['missingGoldens'])} required images missing")
            lines.append("")
    return "\n".join(lines).rstrip() + "\n"


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--format", choices=("json", "markdown"), default="markdown")
    parser.add_argument("--output", type=Path, help="file path (stdout by default)")
    parser.add_argument("--validate", action="store_true",
                        help="reject invalid evidence contracts, but not pending manual reviews")
    parser.add_argument("--require-complete", action="store_true",
                        help="fail unless the entire required release scope is certified")
    args = parser.parse_args(argv)
    try:
        data = build_report()
    except (OSError, ValueError, json.JSONDecodeError) as error:
        parser.exit(2, f"1.22 certification report failed: {error}\n")
    output = (json.dumps(data, indent=2, ensure_ascii=False) + "\n"
              if args.format == "json" else markdown(data))
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(output, encoding="utf-8")
    else:
        print(output, end="")
    if args.validate and data["issues"]:
        return 1
    if args.require_complete and not data["summary"]["readyForPromotion"]:
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
