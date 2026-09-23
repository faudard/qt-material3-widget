#!/usr/bin/env python3
"""Validate the QtMaterial3 public API/header surface."""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path
from typing import Sequence

TOOLS = Path(__file__).resolve().parent
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))

import header_surface

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = header_surface.MANIFEST

INCLUDE_RE = re.compile(
    r'^\s*#\s*include\s*[<"]([^>"]+)[>"]',
    re.MULTILINE,
)
PRIVATE_NAME_RE = re.compile(
    r"(?:^|/)private/|_p\.(?:h|hh|hpp|hxx)$",
    re.IGNORECASE,
)


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
        "QtMaterial3ApiChecks.cmake",
        "qtmaterial3_install_public_headers()",
        "qtmaterial3_add_api_checks()",
    ):
        if token not in text:
            errors.append(
                f"root CMake missing API/header token: {token}"
            )

    if not api_checks.is_file():
        errors.append("missing cmake/QtMaterial3ApiChecks.cmake")
    else:
        checks_text = api_checks.read_text(
            encoding="utf-8", errors="replace"
        )
        for token in (
            "QTMATERIAL3_PUBLIC_HEADERS",
            "qtmaterial3_install_public_headers",
            "install(",
            "FILES",
        ):
            if token not in checks_text:
                errors.append(
                    f"API CMake module missing {token}"
                )
        if re.search(
            r"foreach\s*\([^\)]*QTMATERIAL3_PRIVATE_HEADERS[^\)]*\)"
            r"(?:(?!endforeach\s*\().)*install\s*\(",
            checks_text,
            re.IGNORECASE | re.DOTALL,
        ):
            errors.append(
                "private header list must never be installed"
            )
        if "GLOB_RECURSE" in checks_text:
            errors.append(
                "public-header checks must not glob include/"
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
    manifest = root / MANIFEST
    errors = header_surface.validate_manifest(root, manifest)
    if errors:
        return errors
    public, private = header_surface.parse_manifest(manifest)
    errors.extend(check_public_includes(root, public, private))
    if not source_only:
        errors.extend(check_install_contract(root))
    return errors


def validate_installed(root: Path, prefix: Path) -> list[str]:
    manifest = root / MANIFEST
    manifest_errors = header_surface.validate_manifest(root, manifest)
    if manifest_errors:
        return [
            "source manifest invalid: " + error
            for error in manifest_errors
        ]

    public, private = header_surface.parse_manifest(manifest)
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
