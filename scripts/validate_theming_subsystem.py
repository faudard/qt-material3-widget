#!/usr/bin/env python3
"""Cross-platform focused validation for the theming subsystem."""

from __future__ import annotations

import argparse
import os
import platform
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
TEST_PATTERN = (
    "theme|serializer|seed|backend|componenttoken|runtime|systemtheme|"
    "accessibility|keyboard|visual|public"
)


def run(command: list[str], *, cwd: Path, env: dict[str, str]) -> None:
    print("+", " ".join(command), flush=True)
    subprocess.run(command, cwd=cwd, env=env, check=True)


def validate_build(
    root: Path,
    *,
    name: str,
    use_mcu: str,
    generator: str,
    build_root: Path,
    env: dict[str, str],
) -> None:
    build_dir = build_root / name
    print(f"==> Configuring {name} (QTMATERIAL3_USE_MCU={use_mcu})")
    run(
        [
            "cmake",
            "-S",
            str(root),
            "-B",
            str(build_dir),
            "-G",
            generator,
            "-DCMAKE_BUILD_TYPE=Debug",
            "-DQTMATERIAL3_BUILD_TESTS=ON",
            "-DQTMATERIAL3_BUILD_EXAMPLES=ON",
            "-DQTMATERIAL3_BUILD_BENCHMARKS=OFF",
            "-DQTMATERIAL3_BUILD_VISUAL_REGRESSION_TESTS=ON",
            f"-DQTMATERIAL3_USE_MCU={use_mcu}",
        ],
        cwd=root,
        env=env,
    )

    print(f"==> Building {name}")
    run(
        ["cmake", "--build", str(build_dir), "--config", "Debug"],
        cwd=root,
        env=env,
    )

    print(f"==> Running focused theming tests for {name}")
    ctest = [
        "ctest",
        "--test-dir",
        str(build_dir),
        "--build-config",
        "Debug",
        "--output-on-failure",
        "-R",
        TEST_PATTERN,
    ]
    if platform.system() == "Linux" and shutil.which("xvfb-run"):
        ctest = ["xvfb-run", "-a", *ctest]
    run(ctest, cwd=root, env=env)


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument(
        "--generator",
        default=os.environ.get("QTM3_GENERATOR", "Ninja"),
    )
    parser.add_argument("--build-root", type=Path)
    args = parser.parse_args(argv)

    root = args.root.resolve()
    build_root = (
        args.build_root.resolve()
        if args.build_root
        else root / "build"
    )

    env = os.environ.copy()
    env.setdefault("CMAKE_BUILD_PARALLEL_LEVEL", "2")
    if platform.system() == "Linux":
        env.setdefault("QT_QPA_PLATFORM", "offscreen")

    try:
        validate_build(
            root,
            name="theming-fallback",
            use_mcu="OFF",
            generator=args.generator,
            build_root=build_root,
            env=env,
        )
        validate_build(
            root,
            name="theming-mcu-requested",
            use_mcu="ON",
            generator=args.generator,
            build_root=build_root,
            env=env,
        )
    except subprocess.CalledProcessError as exc:
        return exc.returncode or 1

    print("==> Theming subsystem validation completed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
