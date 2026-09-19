#!/usr/bin/env python3
"""Validate QtMaterial3 public/private header boundaries."""

from __future__ import annotations

import argparse
import re
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = Path("cmake/QtMaterial3HeaderSurfaceManifest.cmake")

INCLUDE_RE = re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]', re.MULTILINE)
PRIVATE_NAME_RE = re.compile(r"(?:^|/)private/|_p\.(?:h|hh|hpp|hxx)$", re.IGNORECASE)


def load_manifest_module(root: Path):
    import importlib.util
    import sys
    script = root / "tools/update_header_surface_manifest.py"
    spec = importlib.util.spec_from_file_location("qtm3_header_manifest", script)
    if spec is None or spec.loader is None:
        raise RuntimeError("cannot load manifest helper")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def check_public_includes(
    root: Path,
    public: list[str],
    private: list[str],
) -> list[str]:
    errors: list[str] = []
    private_set = set(private)

    for rel in public:
        path = root / "include" / rel
        if not path.is_file():
            errors.append(f"public header missing: {rel}")
            continue

        text = path.read_text(encoding="utf-8", errors="replace")
        for include in INCLUDE_RE.findall(text):
            normalized = include.replace("\\", "/")

            if normalized in private_set:
                errors.append(f"{rel} includes private header {normalized}")
                continue

            if PRIVATE_NAME_RE.search(normalized):
                errors.append(f"{rel} includes private-looking header {normalized}")

            if normalized.startswith("src/") or "/src/" in normalized:
                errors.append(f"{rel} includes source-tree implementation path {normalized}")

            if normalized.startswith("../") and "src/" in normalized:
                errors.append(f"{rel} reaches into source implementation via {normalized}")

    return errors


def check_install_contract(root: Path) -> list[str]:
    errors: list[str] = []
    root_cmake = root / "CMakeLists.txt"
    helper = root / "cmake/QtMaterial3HeaderSurface.cmake"
    hygiene = root / "cmake/QtMaterial3PublicHeaderHygiene.cmake"

    if not root_cmake.is_file():
        return ["missing root CMakeLists.txt"]

    text = root_cmake.read_text(encoding="utf-8")
    if re.search(
        r"install\s*\(\s*DIRECTORY\s+[^\)]*include/?[\"']?",
        text,
        re.IGNORECASE | re.DOTALL,
    ):
        errors.append("root install must not install include/ as a whole directory")

    for token in (
        "QtMaterial3HeaderSurface.cmake",
        "qtmaterial3_install_public_headers()",
    ):
        if token not in text:
            errors.append(f"root CMake missing explicit header install token: {token}")

    if not helper.is_file():
        errors.append("missing cmake/QtMaterial3HeaderSurface.cmake")
    else:
        helper_text = helper.read_text(encoding="utf-8")
        for token in (
            "QTMATERIAL3_PUBLIC_HEADERS",
            "install(",
            "FILES",
        ):
            if token not in helper_text:
                errors.append(f"header install helper missing {token}")
        # A private-header loop may validate source files, but an install()
        # command inside that loop is forbidden.
        if re.search(
            r"foreach\s*\([^\)]*QTMATERIAL3_PRIVATE_HEADERS[^\)]*\)"
            r"(?:(?!endforeach\s*\().)*install\s*\(",
            helper_text,
            re.IGNORECASE | re.DOTALL,
        ):
            errors.append("private header list must never be installed")

    consumer_runner = root / "scripts/ci/run-consumer-matrix.py"
    if not consumer_runner.is_file():
        errors.append("consumer matrix runner missing installed header-surface verification")
    else:
        runner_text = consumer_runner.read_text(encoding="utf-8", errors="replace")
        if (
            "check_installed_header_surface.py" not in runner_text
            or "--prefix" not in runner_text
        ):
            errors.append("consumer matrix runner missing installed header-surface verification")

    if not hygiene.is_file():
        errors.append("missing QtMaterial3PublicHeaderHygiene.cmake")
    else:
        hygiene_text = hygiene.read_text(encoding="utf-8")
        if "QTMATERIAL3_PUBLIC_HEADERS" not in hygiene_text:
            errors.append("public-header compile tests must be driven by the manifest")
        if "GLOB_RECURSE" in hygiene_text:
            errors.append("public-header compile tests must not glob the include tree")

    return errors


def validate(root: Path, *, source_only: bool = False) -> list[str]:
    helper = load_manifest_module(root)
    manifest = root / MANIFEST

    errors = helper.validate_manifest(root, manifest)
    if errors:
        return errors

    public, private = helper.parse_manifest(manifest)
    errors.extend(check_public_includes(root, public, private))

    if not source_only:
        errors.extend(check_install_contract(root))

    return errors


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument("--source-only", action="store_true")
    args = parser.parse_args(argv)

    root = args.root.resolve()
    errors = validate(root, source_only=args.source_only)
    if errors:
        print("Public/private header contract FAILED:")
        for error in errors:
            print(" -", error)
        return 1

    print("Public/private header contract OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
