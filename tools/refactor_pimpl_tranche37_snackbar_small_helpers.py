#!/usr/bin/env python3
"""
Tranche 37: QtMaterialSnackbar small helper cleanup.

Target commit family: c5994b7ac67b41bb01bd0ed8d11b7cbc4ff1af99, after the
Snackbar d_ptr initialization fix.

This intentionally performs a conservative cleanup only:
  - remove currentDurationMs() and containerRect() from the public header's
    private section;
  - replace them with .cpp-local helpers;
  - do not move theme/UI/signal-heavy helpers yet.
"""
from __future__ import annotations

import argparse
import difflib
import re
import shutil
import sys
from pathlib import Path

HEADER = Path("include/qtmaterial/widgets/surfaces/qtmaterialsnackbar.h")
SOURCE = Path("src/widgets/surfaces/qtmaterialsnackbar.cpp")


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def write(path: Path, text: str) -> None:
    path.write_text(text, encoding="utf-8")


def unified(path: Path, old: str, new: str) -> str:
    return "".join(
        difflib.unified_diff(
            old.splitlines(keepends=True),
            new.splitlines(keepends=True),
            fromfile=str(path),
            tofile=str(path),
        )
    )


def remove_decl(text: str, decl_pattern: str) -> str:
    # Handles normal multi-line C++ headers and also resiliently removes the
    # declaration if whitespace was reformatted by a previous script.
    return re.sub(r"\n\s*" + decl_pattern + r"\s*;", "", text)


def transform_header(text: str) -> str:
    text = remove_decl(text, r"int\s+currentDurationMs\s*\(\s*\)\s*const")
    text = remove_decl(text, r"QRectF\s+containerRect\s*\(\s*\)\s*const")
    return text


def ensure_include(text: str, include: str) -> str:
    if include in text:
        return text
    # Keep Qt includes grouped near the other Qt includes.
    m = re.search(r"(#include\s+<Q[^>]+>\s*\n)(?!#include\s+<Q)", text)
    if m:
        return text[: m.end()] + include + "\n" + text[m.end():]
    return include + "\n" + text


def insert_helpers(text: str) -> str:
    if "snackbarDurationMs(" in text and "snackbarContainerRect(" in text:
        return text

    helper_block = r'''
namespace {

int snackbarDurationMs(const SnackbarRequest& request)
{
    switch (request.duration) {
    case SnackbarDuration::Short:
        return 4000;
    case SnackbarDuration::Long:
        return 10000;
    case SnackbarDuration::Indefinite:
        return 0;
    }
    return 0;
}

QRectF snackbarContainerRect(const QtMaterialSnackbar& snackbar)
{
    return QRectF(snackbar.rect());
}

} // namespace
'''

    # Insert after QtMaterialSnackbarPrivate's closing brace, before the widget
    # constructor. This is intentionally narrow to avoid reordering the file.
    pattern = re.compile(
        r"(class\s+QtMaterialSnackbarPrivate\s*\{.*?\n\};\s*\n)(\s*QtMaterialSnackbar::QtMaterialSnackbar\s*\()",
        re.DOTALL,
    )
    match = pattern.search(text)
    if not match:
        raise RuntimeError(
            "Could not locate QtMaterialSnackbarPrivate block before constructor. "
            "Apply tranche 36 first and verify the source still matches the expected layout."
        )
    return text[: match.end(1)] + helper_block + text[match.start(2):]


def remove_member_definition(text: str, signature_regex: str) -> str:
    # Removes a simple one-level member function body. The two functions handled
    # by this tranche have no nested braces except switch blocks for duration;
    # currentDurationMs has a switch, so use a specific non-greedy terminator.
    pattern = re.compile(signature_regex, re.DOTALL)
    return pattern.sub("", text)


def transform_source(text: str) -> str:
    text = ensure_include(text, "#include <QRectF>")
    text = insert_helpers(text)

    # Replace call sites first, then remove member definitions.
    text = text.replace("currentDurationMs()", "snackbarDurationMs(d_ptr->request)")
    text = text.replace("containerRect()", "snackbarContainerRect(*this)")

    # Remove the old member definitions. The call-site replacements may also
    # alter the definitions' signatures; handle both original and replaced forms.
    text = re.sub(
        r"\nint\s+QtMaterialSnackbar::(?:currentDurationMs|snackbarDurationMs\s*\(\s*d_ptr->request\s*\))\s*\([^)]*\)\s*const\s*\{\s*switch\s*\([^)]*\)\s*\{.*?\}\s*return\s+0\s*;\s*\}\s*",
        "\n",
        text,
        flags=re.DOTALL,
    )
    text = re.sub(
        r"\nQRectF\s+QtMaterialSnackbar::(?:containerRect|snackbarContainerRect\s*\(\s*\*this\s*\))\s*\([^)]*\)\s*const\s*\{\s*return\s+QRectF\s*\(\s*rect\s*\(\s*\)\s*\)\s*;\s*\}\s*",
        "\n",
        text,
        flags=re.DOTALL,
    )
    return text


def audit(root: Path) -> int:
    header = read(root / HEADER)
    source = read(root / SOURCE)
    problems: list[str] = []
    if re.search(r"int\s+currentDurationMs\s*\(\s*\)\s*const\s*;", header):
        problems.append("header still declares currentDurationMs()")
    if re.search(r"QRectF\s+containerRect\s*\(\s*\)\s*const\s*;", header):
        problems.append("header still declares containerRect()")
    if "QtMaterialSnackbar::currentDurationMs" in source:
        problems.append("source still defines QtMaterialSnackbar::currentDurationMs()")
    if "QtMaterialSnackbar::containerRect" in source:
        problems.append("source still defines QtMaterialSnackbar::containerRect()")
    if "snackbarDurationMs(d_ptr->request)" not in source:
        problems.append("source does not use snackbarDurationMs(d_ptr->request)")
    if "snackbarContainerRect(*this)" not in source:
        problems.append("source does not use snackbarContainerRect(*this)")

    if problems:
        print("Snackbar small-helper cleanup audit FAILED:")
        for p in problems:
            print(f"  - {p}")
        return 1
    print("Snackbar small-helper cleanup audit OK")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", default=".", help="Repository root")
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("--audit", action="store_true")
    args = parser.parse_args()

    root = Path(args.root).resolve()
    header_path = root / HEADER
    source_path = root / SOURCE
    if not header_path.exists() or not source_path.exists():
        print("Expected QtMaterialSnackbar header/source not found", file=sys.stderr)
        return 2

    if args.audit:
        return audit(root)

    old_header = read(header_path)
    old_source = read(source_path)
    new_header = transform_header(old_header)
    new_source = transform_source(old_source)

    changed = []
    if new_header != old_header:
        changed.append((header_path, old_header, new_header))
    if new_source != old_source:
        changed.append((source_path, old_source, new_source))

    if args.dry_run:
        if not changed:
            print("No changes needed.")
        for path, old, new in changed:
            print(unified(path.relative_to(root), old, new))
        return 0

    for path, old, new in changed:
        backup = path.with_suffix(path.suffix + ".bak")
        if not backup.exists():
            shutil.copy2(path, backup)
        write(path, new)
        print(f"updated {path.relative_to(root)}")
    if not changed:
        print("No changes needed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
