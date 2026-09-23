#!/usr/bin/env python3
from __future__ import annotations

import argparse
import re
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[2]
TOOLS = ROOT / "tools"
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))

import qt_support


def capture(command: list[str]) -> tuple[int, str]:
    process = subprocess.run(
        command,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        check=False,
    )
    return process.returncode, process.stdout.strip()


def detect_qt_version() -> str:
    for name in ("qmake", "qmake6", "qmake-qt5"):
        exe = shutil.which(name)
        if exe:
            rc, out = capture([exe, "-query", "QT_VERSION"])
            if rc == 0 and out:
                return out.splitlines()[-1].strip()
    raise RuntimeError("qmake/qmake6/qmake-qt5 not found or unusable")


def detect_msvc_version() -> str:
    exe = shutil.which("cl")
    if not exe:
        raise RuntimeError("cl.exe not found")
    _rc, out = capture([exe])
    match = re.search(
        r"Version\s+([0-9]+\.[0-9]+(?:\.[0-9]+)?)",
        out,
    )
    if not match:
        raise RuntimeError(f"unable to parse MSVC version: {out}")
    return match.group(1)


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--profile", required=True)
    args = parser.parse_args(argv)

    try:
        data = qt_support.load_manifest(ROOT)
        profile = qt_support.certification_profile(data, args.profile)
    except qt_support.QtSupportError as exc:
        print(str(exc), file=sys.stderr)
        return 2

    qt = detect_qt_version()
    print("Qt version:", qt)
    expected_qt = profile.get("qtVersion")
    if expected_qt and qt != expected_qt:
        print(
            f"profile {args.profile} expects Qt {expected_qt}, got {qt}",
            file=sys.stderr,
        )
        return 1

    expected_msvc = profile.get("hostedCompilerPrefix")
    if expected_msvc:
        msvc = detect_msvc_version()
        print("MSVC compiler:", msvc)
        if not msvc.startswith(str(expected_msvc)):
            print(
                f"profile {args.profile} expects MSVC prefix "
                f"{expected_msvc}, got {msvc}",
                file=sys.stderr,
            )
            return 1

    print(f"Toolchain matches qt-support.json profile: {args.profile}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
