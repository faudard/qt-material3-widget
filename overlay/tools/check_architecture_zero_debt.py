#!/usr/bin/env python3
"""Authoritative zero-debt architecture gate for QtMaterial3.

This gate deliberately runs BOTH architecture engines until their rule sets are
consolidated in a later change. A repository is architecture-clean only when:

1. tools/check_architecture_contracts.py --strict passes;
2. scripts/architecture/check_architecture.py passes;
3. the legacy baseline contains zero active entries;
4. no architecture-suppression markers are present;
5. the post-0.5 target graph contract is complete.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, Sequence

ROOT = Path(__file__).resolve().parents[1]
BASELINE = Path("tools/architecture_contracts_baseline.txt")
CONFIG = Path("tools/architecture_contracts.json")
LEGACY_CHECKER = Path("tools/check_architecture_contracts.py")
LAYER_CHECKER = Path("scripts/architecture/check_architecture.py")
LAYER_RULES = Path("scripts/architecture/architecture_rules.json")

REQUIRED_TARGETS = {
    "qtmaterial3_foundation",
    "qtmaterial3_theme_model",
    "qtmaterial3_theme_io",
    "qtmaterial3_theme_runtime",
    "qtmaterial3_specs",
    "qtmaterial3_core",
    "qtmaterial3_widgets",
}

SUPPRESSION_RE = re.compile(
    r"(?:ARCH(?:ITECTURE)?[-_ ]?(?:IGNORE|SUPPRESS|EXCEPTION)|"
    r"NOLINT\s*\(\s*ARCH|"
    r"architecture[_ -]waiver|"
    r"architecture[_ -]exception)",
    re.IGNORECASE,
)

SUPPRESSION_SCAN_ROOTS = (
    "include",
    "src",
    "cmake",
    "scripts",
)

# These files must be able to discuss suppression policy without triggering it.
SUPPRESSION_SCAN_EXCLUDE = {
    "tools/check_architecture_zero_debt.py",
    "docs/architecture/zero-debt.md",
    "specs/011-architecture-zero-debt/spec.md",
    "specs/011-architecture-zero-debt/plan.md",
    "specs/011-architecture-zero-debt/tasks.md",
    "specs/011-architecture-zero-debt/contracts/zero-debt-contract.md",
}


@dataclass(frozen=True)
class Result:
    name: str
    returncode: int
    output: str


def active_baseline_entries(path: Path) -> list[str]:
    if not path.is_file():
        return []
    return [
        line.strip()
        for line in path.read_text(encoding="utf-8").splitlines()
        if line.strip() and not line.lstrip().startswith("#")
    ]


def load_config(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def check_config(config: dict) -> list[str]:
    errors: list[str] = []
    required = set(config.get("required_targets", []))
    missing = sorted(REQUIRED_TARGETS - required)
    if missing:
        errors.append("required_targets missing: " + ", ".join(missing))

    forbidden = config.get("target_forbidden_links", {})
    expectations = {
        "qtmaterial3_theme_model": {
            "qtmaterial3_theme_io",
            "qtmaterial3_theme_runtime",
            "qtmaterial3_specs",
            "qtmaterial3_core",
            "qtmaterial3_widgets",
        },
        "qtmaterial3_theme_io": {
            "qtmaterial3_theme_runtime",
            "qtmaterial3_specs",
            "qtmaterial3_core",
            "qtmaterial3_widgets",
        },
        "qtmaterial3_theme_runtime": {
            "qtmaterial3_theme_io",
            "qtmaterial3_specs",
            "qtmaterial3_core",
            "qtmaterial3_widgets",
        },
        "qtmaterial3_specs": {
            "qtmaterial3_theme_io",
            "qtmaterial3_theme_runtime",
            "qtmaterial3_core",
            "qtmaterial3_widgets",
        },
        "qtmaterial3_core": {
            "qtmaterial3_theme_io",
            "qtmaterial3_widgets",
        },
    }
    for target, expected in expectations.items():
        actual = set(forbidden.get(target, []))
        missing_edges = sorted(expected - actual)
        if missing_edges:
            errors.append(
                f"{target} missing forbidden edges: {', '.join(missing_edges)}"
            )
    return errors


def iter_text_files(root: Path) -> Iterable[Path]:
    suffixes = {
        ".h", ".hh", ".hpp", ".hxx",
        ".c", ".cc", ".cpp", ".cxx",
        ".cmake", ".py", ".json", ".md",
    }
    for base_name in SUPPRESSION_SCAN_ROOTS:
        base = root / base_name
        if not base.exists():
            continue
        for path in base.rglob("*"):
            if not path.is_file():
                continue
            rel = path.relative_to(root).as_posix()
            if rel in SUPPRESSION_SCAN_EXCLUDE:
                continue
            if any(
                part in {".git", "__pycache__", "third_party", "_deps"}
                or part.startswith("build")
                for part in path.relative_to(root).parts
            ):
                continue
            if path.name == "CMakeLists.txt" or path.suffix.lower() in suffixes:
                yield path


def find_suppressions(root: Path) -> list[str]:
    errors: list[str] = []
    for path in iter_text_files(root):
        try:
            text = path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            text = path.read_text(encoding="utf-8", errors="replace")
        for number, line in enumerate(text.splitlines(), 1):
            if SUPPRESSION_RE.search(line):
                errors.append(
                    f"{path.relative_to(root).as_posix()}:{number}: "
                    "architecture suppression markers are forbidden"
                )
    return errors


def run_command(
    root: Path,
    name: str,
    command: list[str],
) -> Result:
    completed = subprocess.run(
        command,
        cwd=root,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        check=False,
        env={**os.environ, "PYTHONDONTWRITEBYTECODE": "1"},
    )
    return Result(name, completed.returncode, completed.stdout.rstrip())


def run_zero_debt(root: Path, python: str = sys.executable) -> tuple[list[str], list[Result]]:
    errors: list[str] = []
    results: list[Result] = []

    baseline_path = root / BASELINE
    baseline = active_baseline_entries(baseline_path)
    if baseline:
        errors.append(
            f"architecture baseline contains {len(baseline)} active debt entr"
            f"{'y' if len(baseline) == 1 else 'ies'}"
        )

    config_path = root / CONFIG
    if not config_path.is_file():
        errors.append(f"missing architecture config: {CONFIG}")
    else:
        try:
            errors.extend(check_config(load_config(config_path)))
        except (OSError, json.JSONDecodeError) as exc:
            errors.append(f"invalid architecture config: {exc}")

    errors.extend(find_suppressions(root))

    legacy = root / LEGACY_CHECKER
    if not legacy.is_file():
        errors.append(f"missing checker: {LEGACY_CHECKER}")
    else:
        results.append(
            run_command(
                root,
                "architecture-contracts-strict",
                [
                    python,
                    str(legacy),
                    "--root", str(root),
                    "--config", str(config_path),
                    "--baseline", str(baseline_path),
                    "--strict",
                ],
            )
        )

    layer = root / LAYER_CHECKER
    rules = root / LAYER_RULES
    if not layer.is_file():
        errors.append(f"missing checker: {LAYER_CHECKER}")
    elif not rules.is_file():
        errors.append(f"missing rules: {LAYER_RULES}")
    else:
        results.append(
            run_command(
                root,
                "architecture-layers",
                [
                    python,
                    str(layer),
                    "--root", str(root),
                    "--rules", str(rules),
                ],
            )
        )

    for result in results:
        if result.returncode != 0:
            errors.append(
                f"{result.name} failed with exit code {result.returncode}"
            )
    return errors, results


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument(
        "--github",
        action="store_true",
        help="emit child-checker output verbatim for GitHub Actions logs",
    )
    args = parser.parse_args(argv)

    root = args.root.resolve()
    errors, results = run_zero_debt(root)

    for result in results:
        if result.output:
            print(f"--- {result.name} ---")
            print(result.output)

    if errors:
        print("Architecture zero-debt gate FAILED:")
        for error in errors:
            print(" -", error)
        return 1

    print(
        "Architecture zero-debt gate OK: "
        "0 baseline entries, 0 strict contract violations, "
        "0 layer-rule violations, 0 suppressions"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
