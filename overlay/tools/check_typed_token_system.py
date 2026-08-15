#!/usr/bin/env python3
"""Validate stable typed token identity and ThemeIO text ownership."""

from __future__ import annotations

import argparse
import re
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]

EXPECTED_CATEGORIES = {
    "Invalid": "0x00",
    "Color": "0x01",
    "Typography": "0x02",
    "Shape": "0x03",
    "Elevation": "0x04",
    "Motion": "0x05",
    "StateLayer": "0x06",
    "Density": "0x07",
    "IconSize": "0x08",
    "Accessibility": "0x09",
    "Interaction": "0x0A",
}

FORBIDDEN_SERIALIZER_TABLES = (
    "kColorRoles",
    "kTypeRoles",
    "kShapeRoles",
    "kElevationRoles",
    "kMotionTokens",
    "kDensityRoles",
    "kIconSizeRoles",
)

def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")

def validate(root: Path = ROOT) -> list[str]:
    errors: list[str] = []

    token_h = root/"include/qtmaterial/foundation/qtmaterialtokenid.h"
    if not token_h.is_file():
        errors.append("missing foundation TokenId")
    else:
        data = read(token_h)
        if "#include <Q" in data or "#include <Qt" in data:
            errors.append("foundation TokenId must have no Qt dependency")
        for name, value in EXPECTED_CATEGORIES.items():
            pattern = rf"\b{name}\s*=\s*{re.escape(value)}"
            if not re.search(pattern, data):
                errors.append(f"TokenCategory::{name} must stay {value}")
        for token in (
            "EncodingVersion = 1",
            "LocalMask = 0x00FFFFFFu",
            "static_assert(sizeof(TokenId) == sizeof(std::uint32_t)",
        ):
            if token not in data:
                errors.append(f"TokenId missing stability token: {token}")

    component_h = root/"include/qtmaterial/foundation/qtmaterialcomponentid.h"
    if not component_h.is_file():
        errors.append("ComponentId must live in Foundation")
    else:
        data = read(component_h)
        if "#include <Q" in data or "#include <Qt" in data:
            errors.append("foundation ComponentId must have no Qt dependency")
        for token in (
            "Button = 0x00000100u",
            "ButtonFilled = 0x00000102u",
            "TextFieldOutlined = 0x00000E11u",
            "Data = 0x00001700u",
            "Navigation = 0x00001800u",
            "Chip = 0x00001A00u",
            "Menu = 0x00001B00u",
            "SegmentedButton = 0x00001C00u",
            "Custom = 0xFFFF0000u",
        ):
            if token not in data:
                errors.append(f"ComponentId stability anchor missing: {token}")

    component_tokens = root/"include/qtmaterial/theme/qtmaterialcomponenttokens.h"
    if component_tokens.is_file():
        data = read(component_tokens)
        if "enum class ComponentId" in data:
            errors.append("ThemeModel must not redefine ComponentId")
        if "qtmaterial/foundation/qtmaterialcomponentid.h" not in data:
            errors.append("Component tokens must consume Foundation ComponentId")

    ids_h = root/"include/qtmaterial/theme/qtmaterialtokenids.h"
    if not ids_h.is_file():
        errors.append("missing ThemeModel token-id mapping")
    else:
        data = read(ids_h)
        for category in (
            "Color","Typography","Shape","Elevation","Motion",
            "StateLayer","Density","IconSize","Accessibility","Interaction",
        ):
            if f"TokenCategory::{category}" not in data:
                errors.append(f"token mapping missing category {category}")
        if "QString" in data or "QLatin1String" in data or "QStringLiteral" in data:
            errors.append("ThemeModel token-id mapping must contain no textual names")

    codec_h = root/"include/qtmaterial/theme/qtmaterialthemetextcodec.h"
    codec_cpp = root/"src/theme/qtmaterialthemetextcodec.cpp"
    for path in (codec_h, codec_cpp):
        if not path.is_file():
            errors.append(f"missing ThemeTextCodec file: {path.relative_to(root)}")
    if codec_h.is_file():
        data = read(codec_h)
        for token in ("tokenIdToString(TokenId id)",
                      "tokenIdFromString(",
                      "TokenCategory category"):
            if token not in data:
                errors.append(f"ThemeTextCodec missing {token}")

    serializer = root/"src/theme/qtmaterialthemeserializer.cpp"
    if serializer.is_file():
        data = read(serializer)
        for table in FORBIDDEN_SERIALIZER_TABLES:
            if table in data:
                errors.append(f"ThemeSerializer duplicates token-name table {table}")
        for token in (
            "qtmaterialthemetextcodec.h",
            "qtmaterialtokenids.h",
            "ThemeTextCodec::tokenIdToString",
            "allColorRoles()",
            "allTypeRoles()",
            "allShapeRoles()",
            "allElevationRoles()",
            "allMotionTokens()",
            "allDensityRoles()",
            "allIconSizeRoles()",
        ):
            if token not in data:
                errors.append(f"ThemeSerializer missing typed-token migration token: {token}")

    # Universal TokenId must not become a QVariant-based hot-path API.
    for base_rel in ("include/qtmaterial/theme", "include/qtmaterial/specs", "src/specs", "src/core"):
        base = root/base_rel
        if not base.exists():
            continue
        for path in base.rglob("*"):
            if not path.is_file() or path.suffix.lower() not in {".h",".hpp",".cpp",".cc",".cxx"}:
                continue
            data = read(path)
            if re.search(r"\bQVariant\s+[A-Za-z0-9_]*(?:token|Token)[A-Za-z0-9_]*\s*\(\s*TokenId\b", data):
                errors.append(
                    f"{path.relative_to(root)} introduces QVariant(TokenId) universal lookup"
                )

    return errors

def main(argv: Sequence[str] | None = None) -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--root", type=Path, default=ROOT)
    args = p.parse_args(argv)
    errors = validate(args.root.resolve())
    if errors:
        print("Typed token system FAILED:")
        for error in errors:
            print(" -", error)
        return 1
    print("Typed token system OK: stable numeric identity + ThemeIO-only text names")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
