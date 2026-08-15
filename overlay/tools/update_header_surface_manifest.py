#!/usr/bin/env python3
"""Generate/check the explicit QtMaterial3 header-surface manifest.

The manifest is version-controlled and is the ONLY install source for public
headers. Classification bootstrap rules are intentionally conservative:

private:
  - any path component named "private";
  - any basename ending in _p.h/_p.hpp/_p.hh/_p.hxx.

public:
  - every other C/C++ header under include/qtmaterial/.

After bootstrap, the manifest is authoritative. Any source-tree drift is a CI
error until the manifest is deliberately regenerated and reviewed.
"""

from __future__ import annotations

import argparse
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = Path("cmake/QtMaterial3HeaderSurfaceManifest.cmake")
HEADER_SUFFIXES = {".h", ".hh", ".hpp", ".hxx"}


def is_private_header(rel: Path) -> bool:
    parts = tuple(part.lower() for part in rel.parts)
    if "private" in parts:
        return True
    stem = rel.name.lower()
    return (
        stem.endswith("_p.h")
        or stem.endswith("_p.hh")
        or stem.endswith("_p.hpp")
        or stem.endswith("_p.hxx")
    )


def scan(root: Path) -> tuple[list[str], list[str]]:
    base = root / "include" / "qtmaterial"
    public: list[str] = []
    private: list[str] = []
    if not base.exists():
        return public, private

    for path in sorted(base.rglob("*")):
        if not path.is_file() or path.suffix.lower() not in HEADER_SUFFIXES:
            continue
        rel = path.relative_to(root / "include")
        rel_text = rel.as_posix()
        (private if is_private_header(rel) else public).append(rel_text)

    return public, private


def render(public: list[str], private: list[str]) -> str:
    def block(name: str, values: list[str]) -> str:
        lines = [f"set({name}"]
        lines.extend(f'    "{value}"' for value in values)
        lines.append(")")
        return "\n".join(lines)

    return (
        "# Generated/reviewed QtMaterial3 header-surface manifest.\n"
        "# Paths are relative to the repository include/ directory.\n"
        "# Do not add private headers to QTMATERIAL3_PUBLIC_HEADERS.\n\n"
        + block("QTMATERIAL3_PUBLIC_HEADERS", public)
        + "\n\n"
        + block("QTMATERIAL3_PRIVATE_HEADERS", private)
        + "\n"
    )


def parse_manifest(path: Path) -> tuple[list[str], list[str]]:
    if not path.is_file():
        raise FileNotFoundError(path)
    text = path.read_text(encoding="utf-8")

    def parse_block(name: str) -> list[str]:
        start = text.find(f"set({name}")
        if start < 0:
            raise ValueError(f"manifest missing {name}")
        end = text.find("\n)", start)
        if end < 0:
            raise ValueError(f"unterminated {name}")
        body = text[start:end].splitlines()[1:]
        result: list[str] = []
        for line in body:
            line = line.strip()
            if not line:
                continue
            if not (line.startswith('"') and line.endswith('"')):
                raise ValueError(f"non-canonical manifest entry: {line}")
            result.append(line[1:-1])
        return result

    return parse_block("QTMATERIAL3_PUBLIC_HEADERS"), parse_block(
        "QTMATERIAL3_PRIVATE_HEADERS"
    )


def validate_manifest(root: Path, manifest_path: Path) -> list[str]:
    errors: list[str] = []
    expected_public, expected_private = scan(root)

    try:
        public, private = parse_manifest(manifest_path)
    except (OSError, ValueError) as exc:
        return [f"manifest load failed: {exc}"]

    if public != sorted(set(public)):
        errors.append("public header manifest must be sorted and duplicate-free")
    if private != sorted(set(private)):
        errors.append("private header manifest must be sorted and duplicate-free")

    overlap = sorted(set(public) & set(private))
    if overlap:
        errors.append("headers classified both public and private: " + ", ".join(overlap))

    for rel in public:
        if is_private_header(Path(rel)):
            errors.append(f"private-looking header classified public: {rel}")

    if public != expected_public:
        missing = sorted(set(expected_public) - set(public))
        stale = sorted(set(public) - set(expected_public))
        if missing:
            errors.append("unclassified/new public headers: " + ", ".join(missing))
        if stale:
            errors.append("stale public manifest entries: " + ", ".join(stale))

    if private != expected_private:
        missing = sorted(set(expected_private) - set(private))
        stale = sorted(set(private) - set(expected_private))
        if missing:
            errors.append("unclassified/new private headers: " + ", ".join(missing))
        if stale:
            errors.append("stale private manifest entries: " + ", ".join(stale))

    return errors


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
