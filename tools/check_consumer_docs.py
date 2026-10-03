#!/usr/bin/env python3
"""Validate the release consumer-documentation contract."""
from __future__ import annotations
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
REGISTRY = ROOT / "docs/components/component-registry.json"
REQUIRED_METADATA = ("name", "family", "publicHeader", "widgetType", "testTarget", "galleryRoute", "docsPath")
FAMILY_TERMS = ("keyboard", "accessib", "rtl")
errors: list[str] = []

entries = json.loads(REGISTRY.read_text(encoding="utf-8"))
release = [entry for entry in entries if entry.get("releaseScope") is True]

if not release:
    errors.append("component registry has no release-scoped components")

seen_ids: set[str] = set()
for entry in release:
    cid = entry.get("id", "<missing-id>")
    if cid in seen_ids:
        errors.append(f"{cid}: duplicate registry id")
    seen_ids.add(cid)

    for key in REQUIRED_METADATA:
        if not entry.get(key):
            errors.append(f"{cid}: missing {key}")

    header = ROOT / "include" / str(entry.get("publicHeader", ""))
    if entry.get("publicHeader") and not header.is_file():
        errors.append(f"{cid}: installed public header not found: {header.relative_to(ROOT)}")

    docs_value = entry.get("docsPath")
    if docs_value:
        docs = ROOT / docs_value
        if not docs.is_file():
            errors.append(f"{cid}: docsPath not found: {docs_value}")
        else:
            text = docs.read_text(encoding="utf-8").lower()
            widget = str(entry.get("widgetType", "")).lower()
            if widget and widget not in text:
                errors.append(f"{cid}: {entry['widgetType']} is not named in {docs_value}")
            for term in FAMILY_TERMS:
                if term not in text:
                    errors.append(f"{cid}: {docs_value} does not document {term} behavior")

    route = str(entry.get("galleryRoute", ""))
    if route and not route.startswith("/"):
        errors.append(f"{cid}: galleryRoute must be absolute: {route}")

generated = ROOT / "docs/widgets/components"
for entry in release:
    page = generated / (entry["id"].lower().replace(".", "-") + ".md")
    if not page.is_file():
        errors.append(f"{entry.get('id')}: generated component page missing: {page.relative_to(ROOT)}")
    elif entry.get("widgetType") not in page.read_text(encoding="utf-8"):
        errors.append(f"{entry.get('id')}: generated page does not expose public API type")

    if entry.get("maturity") == "complete":
        axes = entry.get("maturityAxes", {})
        for axis in ("api", "rendering", "states", "accessibility", "keyboard", "hidpi", "rtl", "tests", "example", "docs"):
            value = axes.get(axis)
            if value != 4 and value != "N/A":
                errors.append(f"{entry.get('id')}: complete component has {axis}={value!r}")
        evidence = axes.get("evidence", {})
        for axis in ("rendering", "states", "accessibility", "keyboard", "tests", "example", "docs"):
            if not evidence.get(axis):
                errors.append(f"{entry.get('id')}: complete component lacks {axis} evidence")

catalog = ROOT / "docs/widgets/component-reference.md"
if not catalog.is_file():
    errors.append("missing docs/widgets/component-reference.md")
else:
    catalog_text = catalog.read_text(encoding="utf-8")
    for entry in release:
        for value in (entry.get("name"), entry.get("widgetType"), entry.get("publicHeader"), entry.get("galleryRoute")):
            if value and str(value) not in catalog_text:
                errors.append(f"{entry.get('id')}: component reference missing {value}")

if errors:
    print("Consumer documentation contract failed:", file=sys.stderr)
    for error in errors:
        print(f"  - {error}", file=sys.stderr)
    raise SystemExit(1)

print(f"Consumer documentation contract OK: {len(release)} release-scoped components")
