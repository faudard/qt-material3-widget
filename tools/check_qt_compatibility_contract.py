#!/usr/bin/env python3
from __future__ import annotations
import argparse, json, sys
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "docs/compatibility/qt-support.json"

def validate_manifest(data: dict[str, Any]) -> list[str]:
    errors = []
    if data.get("schemaVersion") != 1:
        errors.append("qt-support schemaVersion must be 1")
    if data.get("languageStandard") != "C++17":
        errors.append("languageStandard must be C++17")
    qt = data.get("qt", {})
    qt5 = qt.get("5", {})
    qt6 = qt.get("6", {})
    if qt5.get("minimum") != "5.14.2":
        errors.append("Qt5 minimum must be exactly 5.14.2")
    if qt5.get("advertisedFloor") != "5.14.2":
        errors.append("Qt5 advertised floor must be 5.14.2")
    cert = qt5.get("primaryCertification", {})
    if cert.get("qtVersion") != "5.14.2":
        errors.append("Qt5 certification must use Qt 5.14.2 exactly")
    if cert.get("qtArchive") != "win64_msvc2017_64":
        errors.append("Qt5 Windows archive must be win64_msvc2017_64")
    refs = set(cert.get("referenceMsvcToolsets", []))
    if not {"14.28", "14.29"}.issubset(refs):
        errors.append("Qt5 reference toolsets must include MSVC 14.28 and 14.29")
    if qt6.get("minimum") != "6.4.0":
        errors.append("Qt6 minimum must be 6.4.0")
    return errors

def require_text(path: Path, tokens: list[str]) -> list[str]:
    if not path.is_file():
        return [f"missing file: {path.relative_to(ROOT)}"]
    text = path.read_text(encoding="utf-8")
    return [f"{path.relative_to(ROOT)} missing: {t}" for t in tokens if t not in text]

def validate_repository_contract() -> list[str]:
    errors = []
    errors += require_text(ROOT/"CMakeLists.txt",
        ["QtMaterial3QtCompatibility.cmake",
         "qtmaterial3_validate_qt_compatibility()"])
    errors += require_text(ROOT/"tests/CMakeLists.txt",
        ["tst_qtcompat", "compat/tst_qtcompat.cpp"])
    errors += require_text(ROOT/".github/workflows/ci.yml",
        ["windows-qt5.14.2-v142", 'qt-version: "5.14.2"',
         "win64_msvc2017_64", 'msvc-toolset: "14.29"',
         "check-qt-toolchain.py"])
    for rel in [
        "cmake/QtMaterial3QtCompatibility.cmake",
        "include/qtmaterial/core/qtmaterialeventcompat.h",
        "tests/compat/tst_qtcompat.cpp",
        "scripts/ci/check-qt-toolchain.py",
    ]:
        if not (ROOT/rel).is_file():
            errors.append(f"missing file: {rel}")
    return errors

def main(argv: Sequence[str] | None = None) -> int:
    argparse.ArgumentParser().parse_args(argv)
    try:
        data = json.loads(MANIFEST.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        print(f"manifest load failed: {exc}", file=sys.stderr)
        return 2
    errors = validate_manifest(data) + validate_repository_contract()
    if errors:
        print("Qt compatibility contract FAILED:")
        for error in errors:
            print(" -", error)
        return 1
    print("Qt compatibility contract OK: Qt 5.14.2 + Qt 6.4+")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
