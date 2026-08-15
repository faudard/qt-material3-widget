#!/usr/bin/env python3
from __future__ import annotations
import argparse, re, shutil, subprocess, sys
from typing import Sequence

def capture(command: list[str]) -> tuple[int, str]:
    p = subprocess.run(command, text=True, stdout=subprocess.PIPE,
                       stderr=subprocess.STDOUT, check=False)
    return p.returncode, p.stdout.strip()

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
    m = re.search(r"Version\s+([0-9]+\.[0-9]+(?:\.[0-9]+)?)", out)
    if not m:
        raise RuntimeError(f"unable to parse MSVC version: {out}")
    return m.group(1)

def main(argv: Sequence[str] | None = None) -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--expected-qt", required=True)
    p.add_argument("--allowed-msvc-prefix", action="append", default=[])
    args = p.parse_args(argv)

    qt = detect_qt_version()
    print("Qt version:", qt)
    if qt != args.expected_qt:
        print(f"expected Qt {args.expected_qt}, got {qt}", file=sys.stderr)
        return 1

    if args.allowed_msvc_prefix:
        msvc = detect_msvc_version()
        print("MSVC compiler:", msvc)
        if not any(msvc.startswith(prefix) for prefix in args.allowed_msvc_prefix):
            print(f"MSVC {msvc} not in allowed prefixes "
                  f"{args.allowed_msvc_prefix}", file=sys.stderr)
            return 1
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
