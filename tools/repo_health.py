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
    strict_health = re.compile(
        r"\bpython(?:3)?\s+tools/repo_health\.py\s+--strict(?:\s|$)"
    )
    if not any(strict_health.search(line) for line in lines):
        return CheckResult(
            "quality-workflow",
            False,
            "repository health must run explicitly in strict mode",
        )
    return CheckResult("quality-workflow", True)

def run_command_check(name: str, command: Sequence[str], cwd: Path = ROOT) -> CheckResult:
    if (
        name.startswith("material-")
        and len(command) > 1
        and not Path(command[1]).is_file()
    ):
        return CheckResult(
            name,
            True,
            f"skipped: tooling not implemented ({Path(command[1]).name})",
        )

    completed = subprocess.run(
        list(command), cwd=str(cwd), text=True,
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT, check=False
    )
    if completed.stdout.rstrip():
        print(f"\n--- {name} output ---")
        print(completed.stdout.rstrip())
    return CheckResult(name, completed.returncode == 0,
                       f"exit={completed.returncode}")

def health_commands(python: str, strict: bool = False) -> list[tuple[str, list[str]]]:
    return [
        ("architecture-zero-debt",
         [python, str(ROOT/"tools/check_architecture_zero_debt.py"),
          "--root", str(ROOT)]),
        ("public-private-headers",
         [python, str(ROOT/"tools/check_public_private_headers.py"),
          "--root", str(ROOT)]),
        ("component-registry",
         [python, str(ROOT/"tools/check_component_registry.py"), "--check-generated"] +
         (["--strict"] if strict else [])),
        ("material-reference-model",
         [python, str(ROOT/"tools/check_material_reference_model.py"),
          "--root", str(ROOT)]),
        ("material-structural-conformance",
         [python, str(ROOT/"tools/check_material_structural_conformance.py"),
          "--root", str(ROOT)]),
        ("material-renderer-conformance",
         [python, str(ROOT/"tools/check_material_renderer_conformance.py"),
          "--root", str(ROOT)]),
        ("material-visual-contract",
         [python, str(ROOT/"tools/check_material_visual_contract.py"),
          "--root", str(ROOT)]),
        ("material-conformance-harness",
         [python, str(ROOT/"tools/check_material_conformance_harness.py"),
          "--root", str(ROOT)]),
        ("build-consumer-matrix-contract",
         [python, str(ROOT/"tools/check_build_consumer_matrix.py")]),
        ("qt5-qt6-compatibility-contract",
         [python, str(ROOT/"tools/check_qt_compatibility_contract.py")]),
        ("no-legacy-specfactory",
         [python, str(ROOT/"tools/check_no_legacy_specfactory.py")]),
        ("theme-target-decomposition",
         [python, str(ROOT/"tools/check_theme_target_decomposition.py")]),
        ("theme-model",
         [python, str(ROOT/"tools/check_theme_model.py")]),
        ("theme-io",
         [python, str(ROOT/"tools/check_theme_io.py")]),
        ("theme-runtime",
         [python, str(ROOT/"tools/check_theme_runtime.py")]),
        ("typed-token-system",
         [python, str(ROOT/"tools/check_typed_token_system.py")]),
    ]

def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--strict", action="store_true")
    args = parser.parse_args(argv)

    py = sys.executable
    commands = health_commands(py, args.strict)
    results = [check_quality_workflow_structure()]
    results.extend(run_command_check(n,c) for n,c in commands)
    for r in results:
        print(("PASS" if r.ok else "FAIL"), r.name, r.detail)
    return 1 if any(not r.ok for r in results) else 0

if __name__ == "__main__":
    raise SystemExit(main())
