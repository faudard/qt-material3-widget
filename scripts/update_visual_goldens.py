#!/usr/bin/env python3
"""Cross-platform updater for QtMaterial3 visual regression goldens."""

from __future__ import annotations

import argparse
import os
import subprocess
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str], *, cwd: Path, env: dict[str, str]) -> None:
    print("+", " ".join(command), flush=True)
    subprocess.run(command, cwd=cwd, env=env, check=True)


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("build_dir", nargs="?", default="build")
    parser.add_argument("--root", type=Path, default=ROOT)
    args = parser.parse_args(argv)

    root = args.root.resolve()
    build_dir = Path(args.build_dir)
    if not build_dir.is_absolute():
        build_dir = root / build_dir

    env = os.environ.copy()
    try:
        run(
            [
                "cmake",
                "-S",
                str(root),
                "-B",
                str(build_dir),
                "-DQTMATERIAL3_BUILD_TESTS=ON",
                "-DQTMATERIAL3_BUILD_EXAMPLES=ON",
                "-DQTMATERIAL3_BUILD_VISUAL_REGRESSION_TESTS=ON",
            ],
            cwd=root,
            env=env,
        )
        run(
            ["cmake", "--build", str(build_dir)],
            cwd=root,
            env=env,
        )
        update_env = env.copy()
        update_env["QTMATERIAL3_UPDATE_VISUAL_GOLDENS"] = "1"
        run(
            [
                "ctest",
                "--test-dir",
                str(build_dir),
                "--output-on-failure",
                "-R",
                "tst_theme_visual_regression",
            ],
            cwd=root,
            env=update_env,
        )
    except subprocess.CalledProcessError as exc:
        return exc.returncode or 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
