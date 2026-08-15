#!/usr/bin/env python3
"""Repository-level health gate for qt-material3-widget.

The default mode is intentionally compatible with the brownfield architecture
migration: existing architecture debt may remain baselined, but new violations
and stale baseline entries fail. Use --strict only when the architecture debt
has reached zero (release 0.5 gate / spec 011).
"""

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
    """Return simple top-level mapping keys without requiring a YAML package.

    This is a structural smoke check, not a replacement for GitHub Actions'
    own workflow validation. It deliberately catches the current failure mode
    where a file starts directly with a job body instead of name/on/jobs.
    """
    keys: set[str] = set()
    pattern = re.compile(r"""^(?P<key>["']?[A-Za-z_][A-Za-z0-9_-]*["']?)\s*:(?:\s+.*)?$""")
    for raw in text.splitlines():
        if not raw or raw[0].isspace() or raw.lstrip().startswith("#"):
            continue
        match = pattern.match(raw)
        if match:
            keys.add(match.group("key").strip("\"'"))
    return keys


def check_quality_workflow_structure(path: Path = QUALITY_WORKFLOW) -> CheckResult:
    if not path.is_file():
        return CheckResult("quality-workflow", False, f"missing: {path}")

    text = path.read_text(encoding="utf-8")
    keys = _top_level_yaml_keys(text)
    required = {"name", "on", "jobs"}
    missing = sorted(required - keys)
    if missing:
        return CheckResult(
            "quality-workflow",
            False,
            "missing top-level key(s): " + ", ".join(missing),
        )

    # Make sure jobs is not merely an empty declaration.
    lines = text.splitlines()
    try:
        jobs_index = next(
            i for i, line in enumerate(lines)
            if re.match(r"^jobs\s*:\s*(?:#.*)?$", line)
        )
    except StopIteration:
        return CheckResult("quality-workflow", False, "jobs mapping not found")

    has_job = any(
        re.match(r"^  [A-Za-z0-9_-]+\s*:\s*(?:#.*)?$", line)
        for line in lines[jobs_index + 1 :]
    )
    if not has_job:
        return CheckResult("quality-workflow", False, "jobs mapping is empty")

    return CheckResult("quality-workflow", True, "required workflow structure present")


def run_command_check(name: str, command: Sequence[str], cwd: Path = ROOT) -> CheckResult:
    completed = subprocess.run(
        list(command),
        cwd=str(cwd),
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        check=False,
    )
    output = completed.stdout.rstrip()
    if output:
        print(f"\n--- {name} output ---")
        print(output)
        print(f"--- end {name} output ---")
    return CheckResult(
        name=name,
        ok=completed.returncode == 0,
        detail=f"exit={completed.returncode}",
    )


def build_checks(*, strict: bool) -> list[tuple[str, list[str]]]:
    python = sys.executable

    architecture = [
        python,
        str(ROOT / "tools" / "check_architecture_contracts.py"),
    ]
    component_status = [
        python,
        str(ROOT / "scripts" / "generate_component_status.py"),
        "--check",
    ]

    if strict:
        architecture.append("--strict")
        component_status.append("--strict")

    return [
        ("architecture-contracts", architecture),
        ("component-registry-generated-files", component_status),
    ]


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Run QtMaterial3 repository health gates.")
    parser.add_argument(
        "--strict",
        action="store_true",
        help=(
            "require zero architecture debt and strict component-registry validation; "
            "intended for the architecture-zero-debt gate, not the current brownfield baseline"
        ),
    )
    args = parser.parse_args(argv)

    results: list[CheckResult] = [check_quality_workflow_structure()]

    for name, command in build_checks(strict=args.strict):
        results.append(run_command_check(name, command))

    print("\nRepository health summary")
    print("=========================")
    for result in results:
        state = "PASS" if result.ok else "FAIL"
        suffix = f" - {result.detail}" if result.detail else ""
        print(f"{state:4}  {result.name}{suffix}")

    failed = [result for result in results if not result.ok]
    if failed:
        print(f"\nRepository health FAILED: {len(failed)} check(s) failed.")
        return 1

    mode = "strict" if args.strict else "brownfield/baseline-aware"
    print(f"\nRepository health PASSED ({mode} mode).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
