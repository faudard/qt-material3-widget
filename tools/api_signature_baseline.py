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

CLASS_SECTIONS = {
    "public-type",
    "public-func",
    "public-static-func",
    "public-attrib",
    "public-static-attrib",
    "public-slot",
    "protected-type",
    "protected-func",
    "protected-static-func",
    "protected-attrib",
    "protected-static-attrib",
    "protected-slot",
    "signal",
    "property",
    "event",
}

NAMESPACE_SECTIONS = {
    "typedef",
    "enum",
    "func",
    "var",
}

SUPPORTED_COMPOUND_KINDS = {
    "class",
    "struct",
    "union",
    "namespace",
    "file",
}


def _text(node: ET.Element | None) -> str:
    if node is None:
        return ""
    return " ".join("".join(node.itertext()).split())


def _normalize(value: str) -> str:
    value = re.sub(r"\s+", " ", value).strip()
    value = re.sub(r"\s*([(),<>&*=])\s*", r"\1", value)
    return value


def _location_file(node: ET.Element) -> str:
    location = node.find("location")
    if location is None:
        return ""
    file_name = (location.get("file") or "").replace("\\", "/")
    if file_name.startswith("include/"):
        return file_name
    marker = "/include/"
    if marker in file_name:
        return file_name[file_name.rfind(marker) + 1 :]
    return file_name


def _qualified_name(compound: str, member: ET.Element) -> str:
    qualified = _normalize(_text(member.find("qualifiedname")))
    if qualified:
        return qualified
    name = _normalize(_text(member.find("name")))
    if not compound:
        return name
    return f"{compound}::{name}"


def _enum_values(member: ET.Element) -> str:
    values = []
    for value in member.findall("enumvalue"):
        name = _normalize(_text(value.find("name")))
        initializer = _normalize(_text(value.find("initializer")))
        values.append(f"{name}{initializer}")
    return ",".join(values)


def _member_signature(compound: str, section: str, member: ET.Element) -> str:
    kind = member.get("kind", "")
    qualified_name = _qualified_name(compound, member)
    type_text = _normalize(_text(member.find("type")))
    args = _normalize(_text(member.find("argsstring")))
    attrs = ",".join(
        f"{key}={member.get(key)}"
        for key in ("prot", "static", "const", "explicit", "virt")
        if member.get(key) is not None
    )
    template = _normalize(_text(member.find("templateparamlist")))
    pieces = [
        qualified_name,
        section,
        kind,
        type_text,
        args,
        attrs,
        "template=" + template,
        "enum=" + _enum_values(member),
        "file=" + _location_file(member),
    ]
    return "|".join(pieces)


def _compound_signature(compound_def: ET.Element, compound_name: str) -> str:
    kind = compound_def.get("kind", "")
    bases = sorted(
        f"{base.get('prot', '')}:{base.get('virt', '')}:{_normalize(_text(base))}"
        for base in compound_def.findall("basecompoundref")
    )
    template = _normalize(_text(compound_def.find("templateparamlist")))
    return "|".join(
        [
            compound_name,
            "compound",
            kind,
            "bases=" + ",".join(bases),
            "template=" + template,
            "file=" + _location_file(compound_def),
        ]
    )


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

        compound_kind = compound_def.get("kind", "")
        if compound_kind not in SUPPORTED_COMPOUND_KINDS:
            continue

        compound_name = _normalize(_text(compound_def.find("compoundname")))
        if compound_kind in {"class", "struct", "union"}:
            signatures.add(_compound_signature(compound_def, compound_name))

        for section in compound_def.findall("sectiondef"):
            section_kind = section.get("kind", "")
            if compound_kind in {"class", "struct", "union"}:
                if section_kind not in CLASS_SECTIONS:
                    continue
            else:
                if section_kind not in NAMESPACE_SECTIONS:
                    continue

            for member in section.findall("memberdef"):
                if compound_kind in {"class", "struct", "union"}:
                    prot = member.get("prot", "")
                    if prot not in {"", "public", "protected"}:
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


def is_source_compatible(actual: Iterable[str], expected: Iterable[str]) -> bool:
    removed, _ = compare(actual, expected)
    return not removed


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

    if expected.get("baselineMajor") != args.baseline_major:
        print(
            "API baseline major does not match the requested baseline major",
            file=sys.stderr,
        )
        return 2

    removed, added = compare(actual["signatures"], expected["signatures"])
    if removed:
        print("Breaking stable API baseline drift detected:")
        for item in removed:
            print(f" - removed/changed: {item}")
        for item in added:
            print(f" + replacement/addition: {item}")
        return 1

    if added:
        print(f"Stable API baseline OK; {len(added)} additive signature(s) detected.")
        for item in added:
            print(f" + additive: {item}")
        return 0

    print(f"Stable API baseline OK ({len(actual['signatures'])} signatures)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
