#!/usr/bin/env python3
"""Generate or verify the stable QtMaterial3 C++ API signature baseline from Doxygen XML."""

from __future__ import annotations

import argparse
import json
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path
from typing import Iterable

SCHEMA_VERSION = 1
SUPPORTED_SECTIONS = {
    "public-type",
    "public-func",
    "public-attrib",
    "public-static-attrib",
    "public-slot",
    "protected-type",
    "protected-func",
    "protected-attrib",
    "protected-static-attrib",
    "protected-slot",
    "signal",
}


def _text(node: ET.Element | None) -> str:
    if node is None:
        return ""
    return " ".join("".join(node.itertext()).split())


def _normalize(value: str) -> str:
    value = re.sub(r"\s+", " ", value).strip()
    value = re.sub(r"\s*([(),<>&*=])\s*", r"\1", value)
    return value


def _member_signature(compound: str, section: str, member: ET.Element) -> str:
    kind = member.get("kind", "")
    name = _text(member.find("name"))
    type_text = _normalize(_text(member.find("type")))
    args = _normalize(_text(member.find("argsstring")))
    attrs = ",".join(
        f"{key}={member.get(key)}"
        for key in ("prot", "static", "const", "explicit", "virt")
        if member.get(key) is not None
    )
    enum_values = ",".join(
        _normalize(_text(value.find("name")))
        for value in member.findall("enumvalue")
    )
    pieces = [compound, section, kind, name, type_text, args, attrs, enum_values]
    return "|".join(pieces)


def extract_signatures(xml_dir: Path) -> list[str]:
    index_path = xml_dir / "index.xml"
    if not index_path.is_file():
        raise FileNotFoundError(f"missing Doxygen index: {index_path}")

    root = ET.parse(index_path).getroot()
    signatures: set[str] = set()
    for compound_index in root.findall("compound"):
        refid = compound_index.get("refid")
        if not refid:
            continue
        compound_path = xml_dir / f"{refid}.xml"
        if not compound_path.is_file():
            continue
        compound_root = ET.parse(compound_path).getroot()
        compound_def = compound_root.find("compounddef")
        if compound_def is None:
            continue
        compound_name = _normalize(_text(compound_def.find("compoundname")))
        for section in compound_def.findall("sectiondef"):
            section_kind = section.get("kind", "")
            if section_kind not in SUPPORTED_SECTIONS:
                continue
            for member in section.findall("memberdef"):
                prot = member.get("prot", "")
                if prot not in {"public", "protected"} and section_kind != "signal":
                    continue
                signatures.add(_member_signature(compound_name, section_kind, member))
    return sorted(signatures)


def make_baseline(xml_dir: Path, baseline_major: int) -> dict[str, object]:
    signatures = extract_signatures(xml_dir)
    return {
        "schemaVersion": SCHEMA_VERSION,
        "baselineMajor": baseline_major,
        "generator": "tools/api_signature_baseline.py",
        "signatures": signatures,
    }


def load_baseline(path: Path) -> dict[str, object]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if data.get("schemaVersion") != SCHEMA_VERSION:
        raise ValueError(f"unsupported API baseline schema in {path}")
    signatures = data.get("signatures")
    if not isinstance(signatures, list) or not all(isinstance(item, str) for item in signatures):
        raise ValueError(f"invalid signatures array in {path}")
    return data


def compare(actual: Iterable[str], expected: Iterable[str]) -> tuple[list[str], list[str]]:
    actual_set = set(actual)
    expected_set = set(expected)
    return sorted(expected_set - actual_set), sorted(actual_set - expected_set)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--xml-dir", type=Path, required=True)
    parser.add_argument("--baseline", type=Path, required=True)
    parser.add_argument("--baseline-major", type=int, default=1)
    parser.add_argument("--write", action="store_true")
    args = parser.parse_args()

    actual = make_baseline(args.xml_dir, args.baseline_major)
    if args.write:
        args.baseline.parent.mkdir(parents=True, exist_ok=True)
        args.baseline.write_text(
            json.dumps(actual, indent=2, sort_keys=True) + "\n",
            encoding="utf-8",
        )
        print(f"Wrote API baseline: {args.baseline}")
        return 0

    if not args.baseline.is_file():
        print(f"API baseline missing: {args.baseline}", file=sys.stderr)
        return 2

    try:
        expected = load_baseline(args.baseline)
    except (OSError, ValueError, json.JSONDecodeError) as exc:
        print(str(exc), file=sys.stderr)
        return 2

    removed, added = compare(actual["signatures"], expected["signatures"])
    if removed or added:
        print("Stable API baseline drift detected:")
        for item in removed:
            print(f" - removed: {item}")
        for item in added:
            print(f" + added: {item}")
        return 1

    print(f"Stable API baseline OK ({len(actual['signatures'])} signatures)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
