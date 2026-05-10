#!/usr/bin/env python3
"""
Run local Buttons maturity checks.

Usage:
  python scripts/run_buttons_maturity_checks.py
  python scripts/run_buttons_maturity_checks.py --strict
  python scripts/run_buttons_maturity_checks.py --build-dir build --run-ctest
  python scripts/run_buttons_maturity_checks.py --build-dir build --run-ctest --strict

Static checks:
  - generated component status is up to date;
  - Buttons maturity promotion audit has no blockers.

CTest checks, when requested:
  - tst_button_contracts;
  - tst_outlinedbutton_rendering;
  - tst_elevatedbutton_shadow;
  - tst_icon_fab_accessibility;
  - existing button/FAB tests matching tst_.*button or tst_.*fab.
"""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BUTTON_CTEST_REGEX = (
    "tst_button_contracts|"
    "tst_outlinedbutton_rendering|"
    "tst_elevatedbutton_shadow|"
    "tst_icon_fab_accessibility|"
    "tst_.*button|"
    "tst_.*fab"
)


def run(command: list[str], *, cwd: Path = ROOT) -> int:
    print("+ " + " ".join(command))
    completed = subprocess.run(command, cwd=str(cwd), check=False)
    return completed.returncode


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Run Buttons maturity checks.")
    parser.add_argument("--build-dir", type=Path, default=None, help="build directory for CTest")
    parser.add_argument("--run-ctest", action="store_true", help="run focused Buttons CTest suite through the promotion audit")
    parser.add_argument("--strict", action="store_true", help="run generator in strict mode")
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)

    generator_cmd = [sys.executable, "scripts/generate_component_status.py", "--check"]
    if args.strict:
        generator_cmd.append("--strict")

    rc = run(generator_cmd)
    if rc != 0:
        return rc

    promotion_cmd = [
        sys.executable,
        "scripts/promote_buttons_maturity.py",
        "--dry-run",
        "--fail-on-blockers",
        "--run-generator-check",
    ]

    if args.run_ctest:
        if args.build_dir is None:
            print("error: --run-ctest requires --build-dir", file=sys.stderr)
            return 2

        build_dir = args.build_dir if args.build_dir.is_absolute() else ROOT / args.build_dir
        if not build_dir.exists():
            print(f"error: build directory does not exist: {build_dir}", file=sys.stderr)
            return 2

        promotion_cmd.extend(["--build-dir", str(args.build_dir), "--run-ctest"])

    rc = run(promotion_cmd)
    if rc != 0:
        return rc

    if args.run_ctest:
        print(f"Buttons CTest regex: {BUTTON_CTEST_REGEX}")

    print("Buttons maturity checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
