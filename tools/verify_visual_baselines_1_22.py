#!/usr/bin/env python3
"""Verify that committed certification references are the exact pinned CI PNGs.

Integrity and repeatability are not human visual approval.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import struct
from pathlib import Path

import check_navigation_visual_repeatability as navigation
import check_missing_material3_visual_repeatability as material3
import check_adaptive_desktop_visual_repeatability as adaptive
import check_expressive_catalogue_visual_repeatability as expressive

ROOT = Path(__file__).resolve().parents[1]
FAMILIES = {
    "navigation-desktop": (navigation, "navigation-desktop-visual-repeatability-1.5.json"),
    "missing-material3": (material3, "material3-catalogue-visual-repeatability-1.7.json"),
    "adaptive-desktop": (adaptive, "adaptive-desktop-visual-repeatability-1.9.json"),
    "expressive-catalogue": (expressive, "expressive-catalogue-visual-repeatability-1.10-1.11.json"),
}


def verify_family(goldens, report, checker, source):
    if report.get("sourceCommit") != source or report.get("repeatable") is not True:
        raise ValueError("inconsistent source or unverified repeatability")
    if report.get("renderer") != checker.PROFILE:
        raise ValueError("renderer does not match the pinned profile")
    images = report.get("images")
    if not isinstance(images, list) or not all(isinstance(image, dict) for image in images):
        raise ValueError("invalid image records")
    names = [image.get("name") for image in images]
    if len(names) != len(checker.CASES) or set(names) != set(checker.CASES):
        raise ValueError("missing, duplicated or unexpected image records")
    for image in images:
        name = image["name"]
        data = (goldens / (name + ".png")).read_bytes()
        if len(data) < 45 or data[:8] != b"\x89PNG\r\n\x1a\n" or data[12:16] != b"IHDR" or data[-8:-4] != b"IEND":
            raise ValueError(f"{name}: invalid PNG")
        if hashlib.sha256(data).hexdigest() != image.get("pngSha256"):
            raise ValueError(f"{name}: PNG SHA256 mismatch")
        if struct.unpack(">II", data[16:24]) != (image.get("width"), image.get("height")):
            raise ValueError(f"{name}: unexpected PNG dimensions")
        if not re.fullmatch(r"[0-9a-f]{64}", str(image.get("pixelSha256", ""))):
            raise ValueError(f"{name}: missing runtime pixel hash")
    return len(images)


def verify(root=ROOT):
    provenance = json.loads((root / "docs/release/visual-baselines-1.22.json").read_text(encoding="utf-8"))
    source = provenance.get("sourceCommit")
    if not isinstance(source, str) or not re.fullmatch(r"[0-9a-f]{40}", source):
        raise ValueError("missing baseline source SHA")
    if type(provenance.get("runId")) is not int or provenance["runId"] <= 0:
        raise ValueError("missing baseline CI run ID")
    families = provenance["families"]
    if set(families) != set(FAMILIES):
        raise ValueError("baseline family set mismatch")
    counts = {}
    for name, (checker, report_file) in FAMILIES.items():
        canonical = families[name]
        standalone = json.loads((root / "docs/components" / report_file).read_text(encoding="utf-8"))
        if any(standalone.get(key) != value for key, value in canonical.items()):
            raise ValueError(f"{name}: family report differs from consolidated provenance")
        if standalone.get("sourceRunId") != provenance["runId"]:
            raise ValueError(f"{name}: source CI run differs")
        counts[name] = verify_family(root / "tests/visual/goldens", canonical, checker, source)
    return {"sourceCommit": source, "runId": provenance["runId"], "verifiedImages": counts}


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path)
    args = parser.parse_args(argv)
    try:
        report = verify()
        text = json.dumps(report, indent=2) + "\n"
        if args.output:
            args.output.parent.mkdir(parents=True, exist_ok=True)
            args.output.write_text(text, encoding="utf-8")
        print(text + "PNG integrity verified; visual sign-off remains a separate manual step.")
    except (OSError, ValueError, KeyError, TypeError) as error:
        parser.exit(1, f"Baseline verification failed: {error}\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
