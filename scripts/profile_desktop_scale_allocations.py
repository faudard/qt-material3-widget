#!/usr/bin/env python3
"""Run a focused Massif heap profile for Desktop Scale 2.0 lifecycle churn."""

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
from pathlib import Path


def find_executable(build_dir: Path, name: str) -> Path | None:
    candidates: list[Path] = []
    for path in build_dir.rglob("*"):
        if not path.is_file():
            continue
        if path.name == name or path.name == f"{name}.exe":
            candidates.append(path)
    return sorted(candidates)[0] if candidates else None


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("build_dir", type=Path)
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--cycles", type=int, default=40)
    args = parser.parse_args()

    if args.cycles <= 0:
        parser.error("--cycles must be positive")

    valgrind = shutil.which("valgrind")
    ms_print = shutil.which("ms_print")
    if not valgrind:
        raise SystemExit("valgrind is required for allocation profiling")

    build_dir = args.build_dir.resolve()
    executable = find_executable(build_dir, "tst_desktop_scale_2_contracts")
    if executable is None:
        raise SystemExit(
            f"tst_desktop_scale_2_contracts was not found under {build_dir}"
        )

    output_dir = args.output_dir.resolve()
    output_dir.mkdir(parents=True, exist_ok=True)
    massif = output_dir / "desktop-scale-2.massif"
    qtest_log = output_dir / "desktop-scale-2-massif-qtest.txt"
    report = output_dir / "desktop-scale-2-massif.txt"

    env = os.environ.copy()
    env.setdefault("QT_QPA_PLATFORM", "offscreen")
    env["QTMATERIAL3_RUN_DESKTOP_SCALE_2"] = "1"
    env["QTMATERIAL3_ENFORCE_PERF_BUDGETS"] = "0"
    env["QTMATERIAL3_DESKTOP_SCALE_LONG_CYCLES"] = str(args.cycles)

    command = [
        valgrind,
        "--tool=massif",
        "--stacks=yes",
        "--time-unit=B",
        "--detailed-freq=10",
        f"--massif-out-file={massif}",
        str(executable),
        "longRunMemoryCycles",
        "-o",
        f"{qtest_log},txt",
    ]
    print("+", " ".join(command), flush=True)
    completed = subprocess.run(command, env=env, check=False)
    if completed.returncode != 0:
        return completed.returncode or 1

    if ms_print:
        with report.open("w", encoding="utf-8") as handle:
            printed = subprocess.run(
                [ms_print, str(massif)],
                stdout=handle,
                stderr=subprocess.STDOUT,
                text=True,
                check=False,
            )
        if printed.returncode != 0:
            return printed.returncode or 1
    else:
        report.write_text(
            "ms_print is unavailable; inspect desktop-scale-2.massif directly.\n",
            encoding="utf-8",
        )

    print(f"Massif profile written to {output_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
