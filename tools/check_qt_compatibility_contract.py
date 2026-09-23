#!/usr/bin/env python3
from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Any, Sequence

TOOLS = Path(__file__).resolve().parent
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))

import qt_support

ROOT = Path(__file__).resolve().parents[1]
validate_manifest = qt_support.validate_manifest


def require_text(
    root: Path,
    relative: str,
    tokens: Sequence[str],
) -> list[str]:
    path = root / relative
    if not path.is_file():
        return [f"missing file: {relative}"]
    text = path.read_text(encoding="utf-8", errors="replace")
    return [
        f"{relative} missing: {token}"
        for token in tokens
        if token not in text
    ]


def _quoted(value: Any) -> str:
    return f'"{value}"'


def validate_repository_contract(
    data: dict[str, Any],
    root: Path = ROOT,
) -> list[str]:
    errors: list[str] = []

    errors += require_text(
        root,
        "CMakeLists.txt",
        [
            "QtMaterial3QtCompatibility.cmake",
            "qtmaterial3_validate_qt_compatibility()",
        ],
    )
    errors += require_text(
        root,
        "cmake/QtMaterial3QtCompatibility.cmake",
        [
            "qt-support.json",
            "JSON QTMATERIAL3_QT5_MIN_VERSION",
            "JSON QTMATERIAL3_QT6_MIN_VERSION",
        ],
    )
    errors += require_text(
        root,
        "tests/CMakeLists.txt",
        ["tst_qtcompat", "compat/tst_qtcompat.cpp"],
    )

    ci_path = root / ".github/workflows/ci.yml"
    if not ci_path.is_file():
        errors.append("missing file: .github/workflows/ci.yml")
    else:
        ci = ci_path.read_text(encoding="utf-8", errors="replace")
        profiles = qt_support.certification_profiles(data)

        qt5 = data["qt"]["5"]["primaryCertification"]
        qt5_tokens = [
            f'compat-profile: {_quoted(qt5["profile"])}',
            f'qt-version: {_quoted(qt5["qtVersion"])}',
            f'qt-arch: {qt5["qtArchive"]}',
            f'msvc-toolset: {_quoted(qt5["hostedMsvcToolset"])}',
        ]
        for token in qt5_tokens:
            if token not in ci:
                errors.append(
                    "CI Qt5 certification drift from qt-support.json: "
                    + token
                )

        for profile_name in ("qt6-windows", "qt6-macos"):
            profile = profiles.get(profile_name)
            if not profile:
                errors.append(
                    f"missing certification profile: {profile_name}"
                )
                continue
            expected = [
                f'compat-profile: {_quoted(profile_name)}',
                f'qt-version: {_quoted(profile["qtVersion"])}',
                f'qt-arch: {profile["qtArchive"]}',
            ]
            if profile_name == "qt6-windows":
                expected.append(
                    f'msvc-toolset: '
                    f'{_quoted(profile["hostedMsvcToolset"])}'
                )
            for token in expected:
                if token not in ci:
                    errors.append(
                        "CI certification drift from qt-support.json: "
                        + token
                    )

        if 'compat-profile: "qt6-linux-system"' not in ci:
            errors.append(
                "CI must identify the system Qt6 certification profile"
            )
        if "scripts/ci/check-qt-toolchain.py" not in ci:
            errors.append("CI must run check-qt-toolchain.py")

    for relative in (
        "include/qtmaterial/core/qtmaterialeventcompat.h",
        "scripts/ci/check-qt-toolchain.py",
        "tools/qt_support.py",
    ):
        if not (root / relative).is_file():
            errors.append(f"missing file: {relative}")

    return errors


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=ROOT)
    args = parser.parse_args(argv)
    root = args.root.resolve()

    try:
        data = qt_support.load_manifest(root)
    except qt_support.QtSupportError as exc:
        print(f"manifest load failed: {exc}", file=sys.stderr)
        return 2

    errors = validate_manifest(data)
    errors += validate_repository_contract(data, root)
    if errors:
        print("Qt compatibility contract FAILED:")
        for error in errors:
            print(" -", error)
        return 1

    print("Qt compatibility contract OK: qt-support.json is authoritative")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
