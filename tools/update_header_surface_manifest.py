#!/usr/bin/env python3
"""Generate/check the explicit QtMaterial3 header-surface manifest."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Sequence

TOOLS = Path(__file__).resolve().parent
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))

import header_surface

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = header_surface.MANIFEST

is_private_header = header_surface.is_private_header
scan = header_surface.scan
render = header_surface.render_manifest
parse_manifest = header_surface.parse_manifest
validate_manifest = header_surface.validate_manifest


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument("--manifest", type=Path)
    parser.add_argument("--write", action="store_true")
    args = parser.parse_args(argv)

    root = args.root.resolve()
    manifest = args.manifest or (root / MANIFEST)
    if not manifest.is_absolute():
        manifest = root / manifest

    public, private = scan(root)

    if args.write:
        manifest.parent.mkdir(parents=True, exist_ok=True)
        manifest.write_text(render(public, private), encoding="utf-8")
        print(
            f"Wrote {manifest}: "
            f"{len(public)} public, {len(private)} private header(s)"
        )
        return 0

    errors = validate_manifest(root, manifest)
    if errors:
        print("Header surface manifest FAILED:")
        for error in errors:
            print(" -", error)
        return 1

    print(
        f"Header surface manifest OK: "
        f"{len(public)} public, {len(private)} private header(s)"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
