#!/usr/bin/env python3
from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> tuple[int, str]:
    completed = subprocess.run(
        command,
        cwd=str(ROOT),
        check=False,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )
    return completed.returncode, completed.stdout


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Check that Buttons maturity scores are synchronized with the axis audit.")
    parser.add_argument("--build-dir", type=Path, default=None, help="build directory used with --run-ctest")
    parser.add_argument("--run-ctest", action="store_true", help="include focused Buttons CTest suite")
    parser.add_argument("--strict", action="store_true", help="fail when any Button axis is not ready for 4")
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)

    audit_cmd = [sys.executable, "scripts/audit_buttons_maturity_axes.py", "--run-generator-check"]
    sync_cmd = [sys.executable, "scripts/sync_buttons_maturity_axes.py", "--dry-run", "--run-generator-check"]

    if args.strict:
        audit_cmd.append("--fail-on-blockers")
        sync_cmd.append("--fail-on-blockers")

    if args.run_ctest:
        if args.build_dir is None:
            print("error: --run-ctest requires --build-dir", file=sys.stderr)
            return 2
        audit_cmd.extend(["--build-dir", str(args.build_dir), "--run-ctest"])
        sync_cmd.extend(["--build-dir", str(args.build_dir), "--run-ctest"])

    print("+ " + " ".join(audit_cmd))
    audit_rc, audit_output = run(audit_cmd)
    print(audit_output, end="" if audit_output.endswith("\n") else "\n")
    if audit_rc != 0:
        return audit_rc

    print("+ " + " ".join(sync_cmd))
    sync_rc, sync_output = run(sync_cmd)
    print(sync_output, end="" if sync_output.endswith("\n") else "\n")
    if sync_rc != 0:
        return sync_rc

    drift_markers = [
        "docs/components/component-registry.json current",
        "Planned score updates:",
    ]
    has_diff = "docs/components/component-registry.json synchronized" in sync_output
    has_score_updates = "No score updates needed." not in sync_output

    if has_diff or has_score_updates:
        print("Buttons maturity registry is out of sync with the axis audit.", file=sys.stderr)
        print("Run:", file=sys.stderr)
        print("  python scripts/sync_buttons_maturity_axes.py --apply", file=sys.stderr)
        print("  python scripts/generate_component_status.py", file=sys.stderr)
        return 1

    print("Buttons maturity registry is synchronized with the axis audit.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
