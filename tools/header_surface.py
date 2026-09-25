from __future__ import annotations

from pathlib import Path

MANIFEST = Path("cmake/QtMaterial3HeaderSurfaceManifest.cmake")
HEADER_SUFFIXES = {".h", ".hh", ".hpp", ".hxx"}

# Source-tree headers that are implementation-only even though their historical
# paths do not use a private/ directory or _p suffix. They are never installed.
INTERNAL_HEADER_PATHS = frozenset({
    "qtmaterial/specs/qtmaterialactionbuttonspecresolver.h",
    "qtmaterial/specs/qtmaterialappbarspecresolver.h",
    "qtmaterial/specs/qtmaterialautocompletepopupspecresolver.h",
    "qtmaterial/specs/qtmaterialautocompletespecresolver.h",
    "qtmaterial/specs/qtmaterialbuttonspecresolver.h",
    "qtmaterial/specs/qtmaterialchipspecresolver.h",
    "qtmaterial/specs/qtmaterialdataspecresolver.h",
    "qtmaterial/specs/qtmaterialdatefieldspecresolver.h",
    "qtmaterial/specs/qtmaterialdialogspecresolver.h",
    "qtmaterial/specs/qtmaterialmenuspecresolver.h",
    "qtmaterial/specs/qtmaterialnavigationrailspecresolver.h",
    "qtmaterial/specs/qtmaterialoverlaysurfacespecresolver.h",
    "qtmaterial/specs/qtmaterialprogressspecresolver.h",
    "qtmaterial/specs/qtmaterialsegmentedbuttonspecresolver.h",
    "qtmaterial/specs/qtmaterialselectionspecresolver.h",
    "qtmaterial/specs/qtmaterialsurfacespecresolver.h",
    "qtmaterial/specs/qtmaterialtabsspecresolver.h",
    "qtmaterial/specs/qtmaterialtextfieldspecresolver.h",
    "qtmaterial/specs/qtmaterialspeccontext.h",
    "qtmaterial/specs/qtmaterialspecvalidation.h",
})


def is_private_header(rel: Path) -> bool:
    if rel.as_posix() in INTERNAL_HEADER_PATHS:
        return True
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


def render_manifest(public: list[str], private: list[str]) -> str:
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

    return (
        parse_block("QTMATERIAL3_PUBLIC_HEADERS"),
        parse_block("QTMATERIAL3_PRIVATE_HEADERS"),
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
        errors.append(
            "headers classified both public and private: " + ", ".join(overlap)
        )

    for rel in public:
        if is_private_header(Path(rel)):
            errors.append(f"private-looking header classified public: {rel}")

    if public != expected_public:
        missing = sorted(set(expected_public) - set(public))
        stale = sorted(set(public) - set(expected_public))
        if missing:
            errors.append(
                "unclassified/new public headers: " + ", ".join(missing)
            )
        if stale:
            errors.append(
                "stale public manifest entries: " + ", ".join(stale)
            )

    if private != expected_private:
        missing = sorted(set(expected_private) - set(private))
        stale = sorted(set(private) - set(expected_private))
        if missing:
            errors.append(
                "unclassified/new private headers: " + ", ".join(missing)
            )
        if stale:
            errors.append(
                "stale private manifest entries: " + ", ".join(stale)
            )

    return errors
