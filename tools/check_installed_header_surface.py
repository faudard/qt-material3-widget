#!/usr/bin/env python3
"""Compare an installed prefix against the declared public header surface."""

from __future__ import annotations

import argparse
import importlib.util
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]


def load_manifest_helper(root: Path):
    script = root / "tools/update_header_surface_manifest.py"
    spec = importlib.util.spec_from_file_location("qtm3_header_manifest", script)
    if spec is None or spec.loader is None:
        raise RuntimeError("cannot load header manifest helper")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def installed_headers(prefix: Path) -> list[str]:
    base = prefix / "include" / "qtmaterial"
    if not base.exists():
        return []
    result = []
    for path in base.rglob("*"):
        if path.is_file() and path.suffix.lower() in {".h", ".hh", ".hpp", ".hxx"}:
            result.append(path.relative_to(prefix / "include").as_posix())
    return sorted(result)


def validate(root: Path, prefix: Path) -> list[str]:
    helper = load_manifest_helper(root)
    manifest = root / "cmake/QtMaterial3HeaderSurfaceManifest.cmake"
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
        errors.append("missing installed public headers: " + ", ".join(missing))
    if extra:
        errors.append("unexpected installed headers: " + ", ".join(extra))
    if leaked_private:
        errors.append("private headers leaked into installation: " + ", ".join(leaked_private))

    return errors


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument("--prefix", type=Path, required=True)
    args = parser.parse_args(argv)

    root = args.root.resolve()
    prefix = args.prefix.resolve()
    errors = validate(root, prefix)
    if errors:
        print("Installed header surface FAILED:")
        for error in errors:
            print(" -", error)
        return 1

    print("Installed header surface OK: public manifest exactly matches package")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
