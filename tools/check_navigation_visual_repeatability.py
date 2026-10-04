#!/usr/bin/env python3
"""Validate two independent pinned CI renders before Navigation golden review."""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import struct
from pathlib import Path

PREFIXES = ("navigation_primary_matrix", "navigation_desktop_matrix", "navigation_split_matrix",
            *("navigation_focus_" + component for component in ("tabs", "rail", "menu", "breadcrumb", "palette", "split")))
VARIANTS = ("light_standard", "dark_standard", "light_high")
CASES = tuple(prefix + "_" + variant for prefix in PREFIXES for variant in VARIANTS)
PROFILE = {"qtVersion": "6.4.0", "style": "fusion", "platform": "xcb", "scaleFactor": "1", "fontDpi": "96"}


def records(directory: Path) -> dict[str, dict]:
    entries = json.loads((directory / "artifacts/manifest.json").read_text(encoding="utf-8"))
    if not isinstance(entries, list):
        raise ValueError("Render manifest must be an array")
    result = {}
    for entry in entries:
        if not isinstance(entry, dict):
            raise ValueError("Render records must be objects")
        name = entry.get("name")
        if name not in CASES:
            continue
        if name in result:
            raise ValueError("Duplicate render record: " + name)
        for key, expected in PROFILE.items():
            if str(entry.get(key, "")).lower() != expected.lower():
                raise ValueError(f"{name}: expected {key}={expected}, got {entry.get(key)!r}")
        if not re.fullmatch(r"[0-9a-f]{40}", entry.get("sourceCommit", "")):
            raise ValueError(name + ": missing source commit provenance")
        if not re.fullmatch(r"[0-9a-f]{64}", entry.get("sha256", "")):
            raise ValueError(name + ": missing pixel hash provenance")
        result[name] = entry
    missing = set(CASES) - result.keys()
    if missing:
        raise ValueError("Missing render records: " + ", ".join(sorted(missing)))
    return result


def png(directory: Path, name: str) -> tuple[bytes, tuple[int, int]]:
    data = (directory / "goldens" / (name + ".png")).read_bytes()
    if len(data) < 45 or data[:8] != b"\x89PNG\r\n\x1a\n" or data[12:16] != b"IHDR" or data[-8:-4] != b"IEND":
        raise ValueError(name + ": invalid PNG")
    size = struct.unpack(">II", data[16:24])
    if not all(size):
        raise ValueError(name + ": empty PNG")
    return data, size


def compare(first: Path, second: Path) -> dict:
    a, b = records(first), records(second)
    commits = {record["sourceCommit"] for record in (*a.values(), *b.values())}
    if len(commits) != 1:
        raise ValueError("Render passes refer to different source commits")
    images = []
    for name in CASES:
        left, size = png(first, name)
        right, other_size = png(second, name)
        if left != right or size != other_size or a[name]["sha256"] != b[name]["sha256"]:
            raise ValueError(name + ": render passes differ")
        if size != (a[name]["width"], a[name]["height"]) or size != (b[name]["width"], b[name]["height"]):
            raise ValueError(name + ": PNG dimensions disagree with runtime metadata")
        images.append({"name": name, "width": size[0], "height": size[1],
                       "pngSha256": hashlib.sha256(left).hexdigest(), "pixelSha256": a[name]["sha256"]})
    return {"schemaVersion": 1, "sourceCommit": commits.pop(), "renderer": PROFILE,
            "repeatable": True, "visualReview": "pending", "images": images}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("first", type=Path)
    parser.add_argument("second", type=Path)
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()
    try:
        report = compare(args.first, args.second)
    except (ValueError, KeyError, TypeError, OSError) as error:
        parser.exit(1, "Navigation visual evidence rejected: " + str(error) + "\n")
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(f"Navigation visual evidence: {len(report['images'])} identical PNGs; visual review remains pending")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
