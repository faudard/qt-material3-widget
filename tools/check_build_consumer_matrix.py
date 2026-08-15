#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_MATRIX = ROOT / "tests" / "consumers" / "consumer-matrix.json"

KINDS = {"source", "add_subdirectory", "fetchcontent", "install", "invalid_component"}
LINKAGES = {"shared", "static"}

def load_matrix(path: Path = DEFAULT_MATRIX) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))

def validate_matrix(data: dict[str, Any]) -> list[str]:
    errors: list[str] = []
    if data.get("schemaVersion") != 1:
        errors.append("consumer matrix schemaVersion must be 1")

    scenarios = data.get("scenarios")
    profiles = data.get("profiles")
    if not isinstance(scenarios, dict) or not scenarios:
        return errors + ["scenarios must be a non-empty object"]
    if not isinstance(profiles, dict) or not profiles:
        return errors + ["profiles must be a non-empty object"]

    for name, scenario in scenarios.items():
        if not isinstance(scenario, dict):
            errors.append(f"{name}: scenario must be an object")
            continue
        if scenario.get("kind") not in KINDS:
            errors.append(f"{name}: unsupported kind {scenario.get('kind')!r}")
        if scenario.get("linkage") not in LINKAGES:
            errors.append(f"{name}: unsupported linkage {scenario.get('linkage')!r}")
        if scenario.get("kind") == "invalid_component" and scenario.get("expectConfigureFailure") is not True:
            errors.append(f"{name}: invalid_component must expect configure failure")

    for profile_name, entries in profiles.items():
        if not isinstance(entries, list) or not entries:
            errors.append(f"{profile_name}: profile must be a non-empty list")
            continue
        unknown = [name for name in entries if name not in scenarios]
        if unknown:
            errors.append(f"{profile_name}: unknown scenario(s): {', '.join(sorted(unknown))}")
        if len(entries) != len(set(entries)):
            errors.append(f"{profile_name}: duplicate scenarios")

    ci = profiles.get("ci", [])
    ci_scenarios = [scenarios[n] for n in ci if n in scenarios and scenarios[n]["kind"] != "invalid_component"]
    ci_kinds = {s["kind"] for s in ci_scenarios}
    for required in {"source", "add_subdirectory", "fetchcontent", "install"}:
        if required not in ci_kinds:
            errors.append(f"ci profile does not cover consumer kind {required}")

    if {s["linkage"] for s in ci_scenarios} != LINKAGES:
        errors.append("ci profile must cover both shared and static linkage")

    full = set(profiles.get("full", []))
    for name in [
        "source-shared","source-static",
        "add-subdirectory-shared","add-subdirectory-static",
        "fetchcontent-shared","fetchcontent-static",
        "install-shared","install-static",
        "invalid-package-component",
    ]:
        if name not in full:
            errors.append(f"full profile missing {name}")

    return errors

def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--matrix", type=Path, default=DEFAULT_MATRIX)
    args = parser.parse_args(argv)
    try:
        data = load_matrix(args.matrix)
    except (OSError, json.JSONDecodeError) as exc:
        print(f"consumer matrix load failed: {exc}", file=sys.stderr)
        return 2

    errors = validate_matrix(data)
    if errors:
        print("consumer matrix validation failed:")
        for error in errors:
            print(f" - {error}")
        return 1

    print(f"consumer matrix OK: {len(data['scenarios'])} scenarios")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
