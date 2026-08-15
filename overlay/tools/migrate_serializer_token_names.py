#!/usr/bin/env python3
"""Move ThemeSerializer token-name ownership to ThemeTextCodec.

Fail closed when the expected serializer shapes are not found.
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path
from typing import Sequence

TABLES = (
    "kColorRoles",
    "kTypeRoles",
    "kShapeRoles",
    "kElevationRoles",
    "kMotionTokens",
    "kDensityRoles",
    "kIconSizeRoles",
)

LOOPS = {
    "kColorRoles": ("ColorRole", "role", "allColorRoles"),
    "kTypeRoles": ("TypeRole", "role", "allTypeRoles"),
    "kShapeRoles": ("ShapeRole", "role", "allShapeRoles"),
    "kElevationRoles": ("ElevationRole", "role", "allElevationRoles"),
    "kMotionTokens": ("MotionToken", "token", "allMotionTokens"),
    "kDensityRoles": ("DensityRole", "role", "allDensityRoles"),
    "kIconSizeRoles": ("IconSizeRole", "role", "allIconSizeRoles"),
}

FUNCTION_VARIABLE = {
    "colorSchemeToJson": "role",
    "colorSchemeFromJson": "role",
    "typographyToJson": "role",
    "typographyFromJson": "role",
    "shapesToJson": "role",
    "shapesFromJson": "role",
    "elevationsToJson": "role",
    "elevationsFromJson": "role",
    "motionToJson": "token",
    "motionFromJson": "token",
    "densityToJson": "role",
    "densityFromJson": "role",
    "iconSizesToJson": "role",
    "iconSizesFromJson": "role",
}


def remove_array(text: str, name: str) -> tuple[str, bool]:
    pattern = re.compile(
        r"const\s+std::array\s+" + re.escape(name)
        + r"\s*=\s*\{.*?\n\};\s*\n",
        re.S,
    )
    new_text, count = pattern.subn("", text, count=1)
    return new_text, count == 1


def replace_in_function(text: str, function: str, old: str, new: str) -> tuple[str, bool]:
    match = re.search(r"\b" + re.escape(function) + r"\s*\(", text)
    if not match:
        return text, False
    brace = text.find("{", match.end())
    if brace < 0:
        return text, False
    depth = 0
    end = brace
    while end < len(text):
        if text[end] == "{":
            depth += 1
        elif text[end] == "}":
            depth -= 1
            if depth == 0:
                end += 1
                break
        end += 1
    body = text[brace:end]
    if old not in body:
        return text, False
    body = body.replace(old, new)
    return text[:brace] + body + text[end:], True


def migrate_text(text: str) -> tuple[str, list[str]]:
    errors: list[str] = []

    if 'qtmaterial/theme/qtmaterialtokenids.h' not in text:
        anchor = '#include "qtmaterial/theme/qtmaterialthemeserializer.h"'
        if anchor not in text:
            errors.append("serializer include anchor not found")
        else:
            text = text.replace(
                anchor,
                anchor
                + '\n#include "qtmaterial/theme/qtmaterialthemetextcodec.h"'
                + '\n#include "qtmaterial/theme/qtmaterialtokenids.h"',
                1,
            )

    for table in TABLES:
        if table in text:
            text, removed = remove_array(text, table)
            if not removed:
                errors.append(f"could not remove {table} definition")

    for table, (enum_type, var, all_fn) in LOOPS.items():
        old = f"for (const auto& [{var}, name] : {table})"
        new = f"for ({enum_type} {var} : {all_fn}())"
        if old in text:
            text = text.replace(old, new)

    # Existing component-override copy loops may use role,name but don't need textual names.
    text = text.replace("Q_UNUSED(name);\n", "")

    for function, var in FUNCTION_VARIABLE.items():
        old = "QString::fromLatin1(name)"
        new = f"ThemeTextCodec::tokenIdToString(tokenId({var}))"
        text, changed = replace_in_function(text, function, old, new)
        if not changed:
            # If already migrated this is fine.
            match = re.search(r"\b" + re.escape(function) + r"\s*\(", text)
            if match:
                brace = text.find("{", match.end())
                snippet = text[brace:text.find("\n}", brace)+2] if brace >= 0 else ""
                if "ThemeTextCodec::tokenIdToString" not in snippet:
                    errors.append(f"{function} did not migrate token naming")

    for table in TABLES:
        if table in text:
            errors.append(f"residual serializer token-name table {table}")

    # No role/token loop should retain a structured binding with textual name.
    if re.search(r"for\s*\(\s*const auto&\s*\[\s*(?:role|token)\s*,\s*name\s*\]", text):
        errors.append("residual role/token textual structured-binding loop")

    return text, errors


def main(argv: Sequence[str] | None = None) -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--file", type=Path, required=True)
    p.add_argument("--apply", action="store_true")
    args = p.parse_args()

    path = args.file.resolve()
    text = path.read_text(encoding="utf-8")
    migrated, errors = migrate_text(text)
    if errors:
        print("Serializer token-name migration FAILED:")
        for error in errors:
            print(" -", error)
        return 1

    if args.apply and migrated != text:
        path.write_text(migrated, encoding="utf-8")
        print("MIGRATED", path)
    elif migrated != text:
        print("WOULD MIGRATE", path)
    else:
        print("Serializer token-name ownership already migrated.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
