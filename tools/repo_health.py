#!/usr/bin/env python3
from __future__ import annotations

import argparse
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
QUALITY_WORKFLOW = ROOT / ".github" / "workflows" / "quality.yml"

@dataclass(frozen=True)
class CheckResult:
    name: str
    ok: bool
    detail: str = ""

def _top_level_yaml_keys(text: str) -> set[str]:
    keys: set[str] = set()
    pattern = re.compile(r"""^(?P<key>["']?[A-Za-z_][A-Za-z0-9_-]*["']?)\s*:""")
    for raw in text.splitlines():
        if raw and not raw[0].isspace() and not raw.lstrip().startswith("#"):
            m = pattern.match(raw)
            if m:
                keys.add(m.group("key").strip("\"'"))
    return keys

def check_quality_workflow_structure(path: Path = QUALITY_WORKFLOW) -> CheckResult:
    if not path.is_file():
        return CheckResult("quality-workflow", False, f"missing: {path}")
    text = path.read_text(encoding="utf-8")
    missing = sorted({"name","on","jobs"} - _top_level_yaml_keys(text))
    if missing:
        return CheckResult("quality-workflow", False,
                           "missing top-level key(s): " + ", ".join(missing))
    lines = text.splitlines()
    try:
        jobs_index = next(i for i,l in enumerate(lines)
                          if re.match(r"^jobs\s*:\s*(?:#.*)?$", l))
    except StopIteration:
        return CheckResult("quality-workflow", False, "jobs mapping not found")
    has_job = any(re.match(r"^  [A-Za-z0-9_-]+\s*:\s*(?:#.*)?$", l)
                  for l in lines[jobs_index+1:])
    if not has_job:
        return CheckResult("quality-workflow", False, "jobs mapping is empty")
    return CheckResult("quality-workflow", True)

def run_command_check(name: str, command: Sequence[str], cwd: Path = ROOT) -> CheckResult:
    completed = subprocess.run(
        list(command), cwd=str(cwd), text=True,
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT, check=False
    )
    if completed.stdout.rstrip():
        print(f"\n--- {name} output ---")
        print(completed.stdout.rstrip())
    return CheckResult(name, completed.returncode == 0,
                       f"exit={completed.returncode}")

def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--strict", action="store_true")
    args = parser.parse_args(argv)

    py = sys.executable
    commands = [
        ("architecture-contracts",
         [py, str(ROOT/"tools/check_architecture_contracts.py")] +
         (["--strict"] if args.strict else [])),
        ("component-registry",
         [py, str(ROOT/"tools/check_component_registry.py"), "--check-generated"] +
         (["--strict"] if args.strict else [])),
        ("build-consumer-matrix-contract",
         [py, str(ROOT/"tools/check_build_consumer_matrix.py")]),
        ("qt5-qt6-compatibility-contract",
         [py, str(ROOT/"tools/check_qt_compatibility_contract.py")]),
        ("no-legacy-specfactory",
         [py, str(ROOT/"tools/check_no_legacy_specfactory.py")]),
        ("theme-target-decomposition",
         [py, str(ROOT/"tools/check_theme_target_decomposition.py")]),
        ("theme-model",
         [py, str(ROOT/"tools/check_theme_model.py")]),
        ("theme-io",
         [py, str(ROOT/"tools/check_theme_io.py")]),
    ]
    results = [check_quality_workflow_structure()]
    results.extend(run_command_check(n,c) for n,c in commands)
    for r in results:
        print(("PASS" if r.ok else "FAIL"), r.name, r.detail)
    return 1 if any(not r.ok for r in results) else 0

if __name__ == "__main__":
    raise SystemExit(main())
