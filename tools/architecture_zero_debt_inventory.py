#!/usr/bin/env python3
"""Produce a deterministic architecture convergence report.

This tool does not create waivers. It groups current strict violations by rule
and path so the remaining remediation can be split into coherent commits.
"""

from __future__ import annotations

import argparse
import importlib.util
import json
import sys
from collections import Counter, defaultdict
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
CHECKER = ROOT / "tools/check_architecture_contracts.py"


def load_checker():
    spec = importlib.util.spec_from_file_location("qtm3_arch_contracts", CHECKER)
    if spec is None or spec.loader is None:
        raise RuntimeError("cannot load architecture checker")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def collect(root: Path) -> list:
    checker = load_checker()
    config = checker.load_json(root / "tools/architecture_contracts.json")
    reporter = checker.Reporter()
    checker.check_sources(root, config, reporter)
    checker.check_cmake(root, config, reporter)
    return reporter.items


def report_json(violations: list) -> dict:
    rules = Counter(v.rule for v in violations)
    paths = Counter(v.path for v in violations)
    return {
        "schemaVersion": 1,
        "total": len(violations),
        "rules": dict(sorted(rules.items())),
        "paths": dict(sorted(paths.items())),
        "violations": [
            {
                "rule": v.rule,
                "path": v.path,
                "line": v.line,
                "kind": v.kind,
                "message": v.message,
                "evidence": v.evidence,
                "key": v.key,
            }
            for v in violations
        ],
    }


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument("--json", type=Path)
    args = parser.parse_args(argv)

    root = args.root.resolve()
    violations = collect(root)
    data = report_json(violations)

    print(f"Architecture debt inventory: {data['total']} strict violation(s)")
    for rule, count in data["rules"].items():
        print(f"  {count:3d}  {rule}")
    if violations:
        print("\nFiles:")
        for path, count in data["paths"].items():
            print(f"  {count:3d}  {path}")

    if args.json:
        output = args.json
        if not output.is_absolute():
            output = root / output
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")
        print("\nWrote:", output)

    return 1 if violations else 0


if __name__ == "__main__":
    raise SystemExit(main())
