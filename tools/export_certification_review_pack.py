#!/usr/bin/env python3
"""Export reviewer worklists without writing certification evidence or scores."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

import certification_center as center
import certification_report_1_22 as health


def export_pack(output: Path, root: Path = health.ROOT) -> None:
    report = health.build_report(root)
    if report["issues"]:
        raise ValueError("invalid evidence contracts: " + "; ".join(report["issues"]))
    output.mkdir(parents=True, exist_ok=True)
    # A new export must never overwrite a tester's completed session.
    sessions = output / "sessions"
    sessions.mkdir(exist_ok=True)
    rows = {item["id"]: item for item in report["components"]}
    lines = [
        "# 1.22 — native AT and visual review worklist", "",
        "Generated from the canonical ledger. No result below is an approval.",
        "Use real native screen readers; record OS, Qt build, spoken output,",
        "actual reviewer, date, and durable evidence. Do not edit registry scores.",
        "The JSON templates intentionally contain pending checks.", "",
    ]
    for version, config in center.MILESTONES.items():
        ledger = center.load_ledger(version, root)
        slug = version.replace("/", "-")
        lines.extend([f"## {version}: {config['name']}", ""])
        for platform in health.READERS:
            record = ledger["platforms"][platform]
            name = f"{slug}-{platform}.json"
            if record["status"] == "pass":
                lines.append(f"- {platform}: recorded pass; see ledger for provenance.")
                continue
            session = center.build_session_template(version, platform, ledger)
            # Exclusive create protects manually edited work on repeated exports.
            with (sessions / name).open("x", encoding="utf-8") as stream:
                json.dump(session, stream, indent=2)
                stream.write("\n")
            lines.extend([
                f"### {platform} — {record['screenReader']}", "",
                f"Session: sessions/{name}", "",
            ])
            for cid, checks in record["components"].items():
                route = rows[cid]["galleryRoute"]
                lines.append(f"- {cid} ({route or 'see component documentation'})")
                for check, state in checks.items():
                    lines.append(f"  - {check}: {state}")
            lines.append("")
        if config["visual"]:
            visual = ledger["visual"]
            lines.extend(["### Visual review", "",
                          f"Ledger status: {visual['status']}", ""])
            for key, value in visual["renderer"].items():
                lines.append(f"- {key}: {value}")
            lines.extend(["", "Required references (presence is not approval):", ""])
            for relative in visual["requiredGoldens"]:
                availability = "present" if (root / relative).is_file() else "missing"
                lines.append(f"- {relative}: {availability}")
            lines.append("")
    lines.extend([
        "## Sign-off", "",
        "Use certification_center.py record --session <file> --confirm-native-session",
        "only after a real session. Review all candidate goldens before recording",
        "visual approval. Run certification_center.py promote (preflight), then",
        "promote --apply only when all gates pass, and finally run",
        "certification_report_1_22.py --require-complete.", "",
    ])
    (output / "review-worklist.md").write_text("\n".join(lines), encoding="utf-8")


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args(argv)
    try:
        export_pack(args.output)
    except (OSError, ValueError) as error:
        parser.exit(1, f"Review pack export failed: {error}\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
