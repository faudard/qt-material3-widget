#!/usr/bin/env python3
"""Stage explicitly reviewed, byte-identical 1.22 CI captures as baselines.

This is not certification sign-off. It never changes maturity or native AT records.
"""
from __future__ import annotations

import argparse
import json
import re
import subprocess
from pathlib import Path

import check_navigation_visual_repeatability as navigation
import check_missing_material3_visual_repeatability as material3
import check_adaptive_desktop_visual_repeatability as adaptive
import check_expressive_catalogue_visual_repeatability as expressive

ROOT = Path(__file__).resolve().parents[1]
FAMILIES = {
    "navigation-desktop": ("navigation", navigation),
    "missing-material3": ("missing-material3", material3),
    "adaptive-desktop": ("adaptive-desktop", adaptive),
    "expressive-catalogue": ("expressive-catalogue", expressive),
}


def collect(artifacts: Path):
    files = {}
    reports = {}
    for family, (prefix, checker) in FAMILIES.items():
        folder = artifacts / (family + "-visual-evidence")
        first, second = folder / (prefix + "-first"), folder / (prefix + "-second")
        report = checker.compare(first, second)
        stored = json.loads((folder / (prefix + "-repeatability.json")).read_text(encoding="utf-8"))
        if report != stored:
            raise ValueError(f"{family}: repeatability report does not match the captures")
        reports[family] = report
        for name in checker.CASES:
            data, _ = checker.png(first, name)
            if name in files:
                raise ValueError(f"Duplicate candidate {name}")
            files[name] = data
    commits = {report["sourceCommit"] for report in reports.values()}
    if len(commits) != 1:
        raise ValueError("Artifacts must originate from the same source commit")
    return commits.pop(), files, reports


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--artifacts", type=Path, required=True,
                        help="directory produced by gh run download")
    parser.add_argument("--run-id", required=True, type=int)
    parser.add_argument("--source-commit", required=True)
    parser.add_argument("--apply", action="store_true")
    parser.add_argument("--confirm-reviewed", action="store_true",
                        help="confirm every changed/new candidate was inspected")
    args = parser.parse_args()
    try:
        if args.run_id <= 0 or not re.fullmatch(r"[0-9a-f]{40}", args.source_commit):
            raise ValueError("Expected a CI run ID and a full 40-character source SHA")
        commit, candidates, reports = collect(args.artifacts)
        if commit != args.source_commit:
            raise ValueError(f"Artifact commit {commit} does not match requested source")
        # Ensure the captured production and test sources match this checkout.
        subprocess.run(["git", "diff", "--quiet", commit, "HEAD", "--",
                        "src", "include", "tests", "CMakeLists.txt",
                        ":(exclude)tests/visual/goldens"], cwd=ROOT, check=True)
        golden_dir = ROOT / "tests/visual/goldens"
        changed = {}
        for name, data in sorted(candidates.items()):
            path = golden_dir / (name + ".png")
            if not path.is_file() or path.read_bytes() != data:
                changed[path] = data
                print(("UPDATE" if path.exists() else "ADD") + " " + str(path.relative_to(ROOT)))
        print(f"{len(candidates)} verified candidates; {len(changed)} changed/new baselines")
        if not args.apply:
            print("Dry run: no files written.")
            return 0
        if not args.confirm_reviewed:
            raise ValueError("--apply requires --confirm-reviewed; visual inspection is mandatory")
        for path, data in changed.items():
            path.write_bytes(data)
        provenance = ROOT / "docs/release/visual-baselines-1.22.json"
        provenance.write_text(json.dumps({
            "schemaVersion": 1, "sourceCommit": commit, "runId": args.run_id,
            "visualReview": "pending", "families": reports,
        }, indent=2) + "\n", encoding="utf-8")
        print("Baselines updated; certification ledger unchanged. Review git diff and rerun strict CI.")
    except (OSError, ValueError, KeyError, TypeError, subprocess.CalledProcessError) as error:
        parser.exit(1, f"Import rejected: {error}\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
