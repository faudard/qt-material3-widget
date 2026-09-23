#!/usr/bin/env python3
"""Validate the QtMaterial3 public API/header surface."""

from __future__ import annotations

import argparse
import importlib.util
import re
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = Path("cmake/QtMaterial3HeaderSurfaceManifest.cmake")

INCLUDE_RE = re.compile(
    r'^\s*#\s*include\s*[<"]([^>"]+)[>"]',
    re.MULTILINE,
)
PRIVATE_NAME_RE = re.compile(
    r"(?:^|/)private/|_p\.(?:h|hh|hpp|hxx)$",
    re.IGNORECASE,
)


def load_manifest_helper(root: Path):
    script = root / "tools/update_header_surface_manifest.py"
    spec = importlib.util.spec_from_file_location(
        "qtm3_header_manifest", script
    )
    if spec is None or spec.loader is None:
        raise RuntimeError("cannot load header manifest helper")
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
                errors.append(
                    f"{rel} includes private header {normalized}"
                )
                continue
            if PRIVATE_NAME_RE.search(normalized):
                errors.append(
                    f"{rel} includes private-looking header {normalized}"
                )
            if (
                normalized.startswith("src/")
                or "/src/" in normalized
            ):
                errors.append(
                    f"{rel} includes source-tree implementation path "
                    f"{normalized}"
                )
            if (
                normalized.startswith("../")
                and "src/" in normalized
            ):
                errors.append(
                    f"{rel} reaches into source implementation via "
                    f"{normalized}"
                )
    return errors


def check_install_contract(root: Path) -> list[str]:
    errors: list[str] = []
    root_cmake = root / "CMakeLists.txt"
    install_helper = root / "cmake/QtMaterial3HeaderSurface.cmake"
    api_checks = root / "cmake/QtMaterial3ApiChecks.cmake"

    if not root_cmake.is_file():
        return ["missing root CMakeLists.txt"]

    text = root_cmake.read_text(encoding="utf-8")
    if re.search(
        r"install\s*\(\s*DIRECTORY\s+[^\)]*include/?[\"']?",
        text,
        re.IGNORECASE | re.DOTALL,
    ):
        errors.append(
            "root install must not install include/ as a whole directory"
        )

    for token in (
        "QtMaterial3HeaderSurface.cmake",
        "qtmaterial3_install_public_headers()",
        "QtMaterial3ApiChecks.cmake",
        "qtmaterial3_add_api_checks()",
    ):
        if token not in text:
            errors.append(
                f"root CMake missing API/header token: {token}"
            )

    if not install_helper.is_file():
        errors.append("missing cmake/QtMaterial3HeaderSurface.cmake")
    else:
        helper_text = install_helper.read_text(encoding="utf-8")
        for token in (
            "QTMATERIAL3_PUBLIC_HEADERS",
            "install(",
            "FILES",
        ):
            if token not in helper_text:
                errors.append(
                    f"header install helper missing {token}"
                )
        if re.search(
            r"foreach\s*\([^\)]*QTMATERIAL3_PRIVATE_HEADERS[^\)]*\)"
            r"(?:(?!endforeach\s*\().)*install\s*\(",
            helper_text,
            re.IGNORECASE | re.DOTALL,
        ):
            errors.append(
                "private header list must never be installed"
            )

    if not api_checks.is_file():
        errors.append("missing cmake/QtMaterial3ApiChecks.cmake")
    else:
        checks_text = api_checks.read_text(
            encoding="utf-8", errors="replace"
        )
        if "QTMATERIAL3_PUBLIC_HEADERS" not in checks_text:
            errors.append(
                "public-header compile tests must be manifest-driven"
            )
        if "GLOB_RECURSE" in checks_text:
            errors.append(
                "public-header compile tests must not glob include/"
            )

    consumer_runner = root / "scripts/ci/run-consumer-matrix.py"
    if not consumer_runner.is_file():
        errors.append(
            "consumer matrix runner missing installed API verification"
        )
    else:
        runner_text = consumer_runner.read_text(
            encoding="utf-8", errors="replace"
        )
        for token in (
            "check_api_surface.py",
            "--scope",
            "installed",
            "--prefix",
        ):
            if token not in runner_text:
                errors.append(
                    "consumer matrix runner missing installed "
                    f"API verification token: {token}"
                )
    return errors


def installed_headers(prefix: Path) -> list[str]:
    base = prefix / "include" / "qtmaterial"
    if not base.exists():
        return []
    result = []
    for path in base.rglob("*"):
        if (
            path.is_file()
            and path.suffix.lower() in {
                ".h", ".hh", ".hpp", ".hxx"
            }
        ):
            result.append(
                path.relative_to(prefix / "include").as_posix()
            )
    return sorted(result)


def validate_source(
    root: Path,
    *,
    source_only: bool = False,
) -> list[str]:
    helper = load_manifest_helper(root)
    manifest = root / MANIFEST
    errors = helper.validate_manifest(root, manifest)
    if errors:
        return errors
    public, private = helper.parse_manifest(manifest)
    errors.extend(check_public_includes(root, public, private))
    if not source_only:
        errors.extend(check_install_contract(root))
    return errors


def validate_installed(root: Path, prefix: Path) -> list[str]:
    helper = load_manifest_helper(root)
    manifest = root / MANIFEST
    manifest_errors = helper.validate_manifest(root, manifest)
    if manifest_errors:
        return [
            "source manifest invalid: " + error
            for error in manifest_errors
        ]

    public, private = helper.parse_manifest(manifest)
    actual = installed_headers(prefix)
    errors: list[str] = []
    missing = sorted(set(public) - set(actual))
    extra = sorted(set(actual) - set(public))
    leaked_private = sorted(set(actual) & set(private))

    if missing:
        errors.append(
            "missing installed public headers: "
            + ", ".join(missing)
        )
    if extra:
        errors.append(
            "unexpected installed headers: " + ", ".join(extra)
        )
    if leaked_private:
        errors.append(
            "private headers leaked into installation: "
            + ", ".join(leaked_private)
        )
    return errors


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument(
        "--scope",
        choices=("source", "installed"),
        default="source",
    )
    parser.add_argument("--source-only", action="store_true")
    parser.add_argument("--prefix", type=Path)
    args = parser.parse_args(argv)

    root = args.root.resolve()
    if args.scope == "source":
        errors = validate_source(
            root,
            source_only=args.source_only,
        )
        success = "API surface OK"
    else:
        if args.prefix is None:
            parser.error("--prefix is required for --scope installed")
        errors = validate_installed(
            root,
            args.prefix.resolve(),
        )
        success = (
            "Installed API surface OK: public manifest "
            "exactly matches package"
        )

    if errors:
        print("API surface FAILED:")
        for error in errors:
            print(" -", error)
        return 1

    print(success)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
