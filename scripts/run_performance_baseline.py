#!/usr/bin/env python3
"""Cross-platform QtMaterial3 performance baseline runner."""

from __future__ import annotations

import argparse
import os
import subprocess
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
BASE_BENCHMARKS = (
    "benchmark_theme_pipeline",
    "benchmark_theme_manager_fanout",
    "benchmark_spec_resolution_pipeline",
    "benchmark_shadow_cache_pipeline",
    "benchmark_theme_switch",
    "benchmark_ripple",
    "benchmark_widget_creation",
)

SCALE_BENCHMARKS = (
    "benchmark_data_views_scale",
    "benchmark_widget_scale",
    "benchmark_command_palette_scale",
    "benchmark_expressive_motion_scale",
)


def run(
    command: list[str],
    *,
    cwd: Path,
    env: dict[str, str],
    check: bool = True,
) -> subprocess.CompletedProcess[str]:
    print("+", " ".join(command), flush=True)
    return subprocess.run(
        command,
        cwd=cwd,
        env=env,
        text=True,
        check=check,
    )


def find_executable(build_dir: Path, name: str) -> Path | None:
    candidates = []
    for path in build_dir.rglob("*"):
        if not path.is_file():
            continue
        if path.name == name or path.name == f"{name}.exe":
            candidates.append(path)
    return sorted(candidates)[0] if candidates else None


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("build_dir", nargs="?", default="build-perf")
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument(
        "--scale",
        action="store_true",
        help="Run the opt-in 1.14 scale contracts in addition to the normal baseline.",
    )
    parser.add_argument(
        "--enforce",
        action="store_true",
        help="Fail when configured performance budgets are exceeded.",
    )
    args = parser.parse_args(argv)

    root = args.root.resolve()
    build_dir = Path(args.build_dir)
    if not build_dir.is_absolute():
        build_dir = root / build_dir

    env = os.environ.copy()
    if args.scale:
        env["QTMATERIAL3_RUN_SCALE_PERF"] = "1"
    if args.enforce:
        env["QTMATERIAL3_ENFORCE_PERF_BUDGETS"] = "1"
    use_mcu = env.get("QTMATERIAL3_USE_MCU", "OFF")

    try:
        run(
            [
                "cmake",
                "-S",
                str(root),
                "-B",
                str(build_dir),
                "-DQTMATERIAL3_BUILD_TESTS=ON",
                "-DQTMATERIAL3_BUILD_EXAMPLES=OFF",
                "-DQTMATERIAL3_BUILD_BENCHMARKS=ON",
                f"-DQTMATERIAL3_USE_MCU={use_mcu}",
            ],
            cwd=root,
            env=env,
        )
        run(
            ["cmake", "--build", str(build_dir)],
            cwd=root,
            env=env,
        )
        run(
            [
                "ctest",
                "--test-dir",
                str(build_dir),
                "--output-on-failure",
                "-R",
                "performance|theme_performance",
            ],
            cwd=root,
            env=env,
        )
    except subprocess.CalledProcessError as exc:
        return exc.returncode or 1

    results_dir = build_dir / "performance-results"
    results_dir.mkdir(parents=True, exist_ok=True)

    benchmarks = BASE_BENCHMARKS + (SCALE_BENCHMARKS if args.scale else ())
    for name in benchmarks:
        executable = find_executable(build_dir, name)
        if executable is None:
            print(
                f"warning: benchmark executable not found: {name}",
                flush=True,
            )
            continue

        print(f"==> {name}")
        output = results_dir / f"{name}.txt"
        completed = run(
            [str(executable), "-o", f"{output},txt"],
            cwd=root,
            env=env,
            check=False,
        )
        if completed.returncode != 0:
            print(
                f"warning: {name} returned {completed.returncode}",
                flush=True,
            )
        if output.is_file():
            print(output.read_text(encoding="utf-8", errors="replace"))

    print(f"Performance results written to {results_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
