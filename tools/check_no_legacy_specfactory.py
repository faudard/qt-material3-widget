#!/usr/bin/env python3
"""Reject any reintroduction of the retired SpecFactory facade.

The active architecture is resolver-based. Historical/specification text may
mention the retired facade, but active code/public API/build files and current
top-level product documentation must not expose it as a supported mechanism.
"""

from __future__ import annotations

import argparse
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, Sequence

ROOT = Path(__file__).resolve().parents[1]

ACTIVE_ROOTS = (
    "include",
    "src",
    "tests",
    "examples",
    "cmake",
    "packaging",
)

CODE_SUFFIXES = {
    ".h", ".hpp", ".hh",
    ".c", ".cc", ".cpp", ".cxx",
    ".cmake", ".in",
    ".py", ".qml", ".ui",
}

ACTIVE_NAME_RE = re.compile(r"spec[\W_]*factory", re.IGNORECASE)
ACTIVE_TEXT_RES = (
    re.compile(r"\bSpecFactory\b"),
    re.compile(r"\bQtMaterialSpecFactory\b"),
    re.compile(r"\bqtmaterialspecfactory\b", re.IGNORECASE),
    re.compile(r"\bspec[\s_-]+factory\b", re.IGNORECASE),
)

CURRENT_DOC_RULES = {
    "README.md": (
        re.compile(r"\bspecs?\s+and\s+factory\b", re.IGNORECASE),
        re.compile(r"\bspec[\s_-]+factory\b", re.IGNORECASE),
        re.compile(r"\bfactory\s+path\b", re.IGNORECASE),
    ),
    "qt_material3_widget_roadmap_and_status.md": (
        re.compile(r"\bspec[\s_-]+factory\b", re.IGNORECASE),
        re.compile(r"\bfactory\s+path\b", re.IGNORECASE),
    ),
}

# The checker itself and the executable Spec Kit record are allowed to name the
# retired facade because they document/enforce its removal.
ALLOW_RELATIVE = {
    Path("tools/check_no_legacy_specfactory.py"),
}


@dataclass(frozen=True)
class Violation:
    path: Path
    line: int
    reason: str
    snippet: str = ""


def iter_active_files(root: Path) -> Iterable[Path]:
    for directory in ACTIVE_ROOTS:
        base = root / directory
        if not base.exists():
            continue
        for path in base.rglob("*"):
            if not path.is_file():
                continue
            rel = path.relative_to(root)
            if rel in ALLOW_RELATIVE:
                continue
            if any(part in {".git", "build", "_build", "__pycache__"} for part in rel.parts):
                continue
            if path.name == "CMakeLists.txt" or path.suffix.lower() in CODE_SUFFIXES:
                yield path


def scan_tree(root: Path = ROOT) -> list[Violation]:
    violations: list[Violation] = []

    for path in iter_active_files(root):
        rel = path.relative_to(root)

        if ACTIVE_NAME_RE.search(path.name):
            violations.append(
                Violation(rel, 0, "legacy SpecFactory filename is forbidden", path.name)
            )

        try:
            text = path.read_text(encoding="utf-8", errors="replace")
        except UnicodeDecodeError:
            continue

        for number, line in enumerate(text.splitlines(), start=1):
            for pattern in ACTIVE_TEXT_RES:
                if pattern.search(line):
                    violations.append(
                        Violation(
                            rel,
                            number,
                            f"legacy SpecFactory reference matches {pattern.pattern}",
                            line.strip(),
                        )
                    )
                    break

    for rel_text, patterns in CURRENT_DOC_RULES.items():
        path = root / rel_text
        if not path.is_file():
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        for number, line in enumerate(text.splitlines(), start=1):
            for pattern in patterns:
                if pattern.search(line):
                    violations.append(
                        Violation(
                            Path(rel_text),
                            number,
                            "current documentation still describes the retired factory path",
                            line.strip(),
                        )
                    )
                    break

    return violations


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Reject the retired QtMaterial3 SpecFactory facade."
    )
    parser.add_argument("--root", type=Path, default=ROOT)
    args = parser.parse_args(argv)

    violations = scan_tree(args.root.resolve())
    if violations:
        print("Legacy SpecFactory contract FAILED:")
        for item in violations:
            location = f"{item.path}:{item.line}" if item.line else str(item.path)
            print(f" - {location}: {item.reason}")
            if item.snippet:
                print(f"   {item.snippet}")
        return 1

    print(
        "Legacy SpecFactory contract OK: active code/public API/build/docs use dedicated resolvers"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
