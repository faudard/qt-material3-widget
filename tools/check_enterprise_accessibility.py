#!/usr/bin/env python3
"""Validate QtMaterial3 1.5 native screen-reader certification evidence."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TOOLS = ROOT / "tools"
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))

import check_release  # noqa: E402


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Validate the 1.5 Enterprise NVDA/Orca/VoiceOver evidence ledger."
    )
    parser.add_argument(
        "--require-complete",
        action="store_true",
        help="require every platform and component check to be pass",
    )
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    rules = check_release.load_rules(ROOT / "tools/release_rules.json")
    stable = rules["base"]["stable_release"]
    errors = check_release.validate_enterprise_accessibility_evidence(
        ROOT,
        stable,
        require_complete=args.require_complete,
    )
    if errors:
        print("Enterprise accessibility evidence validation failed:")
        for error in errors:
            print(f" - {error}")
        return 1

    mode = "complete" if args.require_complete else "structural"
    print(f"Enterprise accessibility evidence OK ({mode})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
