#!/usr/bin/env python3
"""Generate homogeneous consumer component pages from the canonical registry."""
from __future__ import annotations

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "docs/widgets/components"
VISUALS = ROOT / "docs/_static/visuals"
REGISTRY = ROOT / "docs/components/component-registry.json"
REQUIRED = ("id", "name", "family", "publicHeader", "widgetType", "testTarget", "galleryRoute", "docsPath")

all_entries = json.loads(REGISTRY.read_text(encoding="utf-8"))
entries = [e for e in all_entries if all(e.get(key) for key in REQUIRED)]


def slug(entry):
    return re.sub(r"[^a-z0-9]+", "-", entry["id"].lower()).strip("-")


def bullets(values, fallback):
    return "\n".join("- " + str(value) for value in (values or [fallback]))


def visual_reference(entry):
    """Return the best reviewed visual asset currently prepared for this component."""
    if not VISUALS.is_dir():
        return None

    component_tokens = {
        token
        for token in re.split(r"[^a-z0-9]+", (entry["id"] + " " + entry["name"]).lower())
        if len(token) > 2
    }
    family_tokens = {
        token
        for token in re.split(r"[^a-z0-9]+", entry["family"].lower())
        if len(token) > 2
    }

    best = None
    best_score = 0
    for path in sorted(VISUALS.glob("*.png")):
        visual_tokens = {
            token
            for token in re.split(r"[^a-z0-9]+", path.stem.lower())
            if len(token) > 2
        }
        score = len(component_tokens & visual_tokens) * 4 + len(family_tokens & visual_tokens)
        if score > best_score:
            best = path
            best_score = score

    return best if best_score > 0 else None


OUT.mkdir(parents=True, exist_ok=True)
pages = []

for index, entry in enumerate(entries):
    axes = entry.get("maturityAxes", {})
    evidence = axes.get("evidence", {})
    guide = "../../" + Path(entry["docsPath"]).as_posix().removeprefix("docs/")
    visual = visual_reference(entry)

    screenshot = [
        "Open Gallery deep link " + entry["galleryRoute"] + " for the live component preview.",
        "",
    ]
    if visual is not None:
        screenshot += [
            "Reviewed deterministic visual used by the documentation pipeline:",
            "",
            "![Reviewed visual reference](../../_static/visuals/" + visual.name + ")",
            "",
        ]
    else:
        screenshot += [
            "No component-specific reviewed golden is currently available; the Gallery route remains the maintained live visual source.",
            "",
        ]
    screenshot += [
        "; ".join(evidence.get("rendering", []))
        or "Rendering follows the family contract and current theme tokens.",
        "",
    ]

    lines = [
        "# " + entry["name"],
        "",
        "**Public type:** " + entry["widgetType"],
        "**Header:** " + entry["publicHeader"],
        "**Family:** " + entry["family"],
        "**Maturity:** " + str(entry.get("maturity", "unknown")),
        "**Release scoped:** " + ("yes" if entry.get("releaseScope") is True else "no"),
        "**Gallery route:** " + entry["galleryRoute"],
        "",
        "## Screenshot / visual reference",
        "",
        *screenshot,
        "## When to use",
        "",
        "Use this component for the "
        + entry["name"]
        + " interaction in the "
        + entry["family"]
        + " family. See the [canonical family guide]("
        + guide
        + ") for behavioral detail and related components.",
        "",
        "## API",
        "",
        "Installed header: " + entry["publicHeader"],
        "",
        "Public type: "
        + entry["widgetType"]
        + ". See the [generated C++ API reference](../../api/index.md) for exact declarations.",
        "",
        "## States",
        "",
        bullets(
            evidence.get("states"),
            "Enabled/disabled, hover, focus, pressed, selected/checked and error states follow the component contract where applicable.",
        ),
        "",
        "Use the Gallery 2.0 state selector to exercise Default, Hover, Focus, Pressed, Selected, Disabled and Error without leaving the component route.",
        "",
        "## Keyboard",
        "",
        bullets(evidence.get("keyboard"), "Native Qt focus/navigation remains authoritative."),
        "",
        "## Accessibility",
        "",
        bullets(
            evidence.get("accessibility"),
            "Provide a visible label or accessible name and preserve meaningful state semantics.",
        ),
        "",
        "## RTL",
        "",
        bullets(
            evidence.get("rtl"),
            "Layout direction follows QWidget::layoutDirection() where directional geometry applies.",
        ),
        "",
        "## Example",
        "",
        "Open Gallery route "
        + entry["galleryRoute"]
        + ". Gallery 2.0 also exposes copyable C++ and .ui snippets plus live writable properties.",
        "",
        "Focused verification target: "
        + entry["testTarget"]
        + ". See the [examples guide](../../examples/index.md).",
        "",
        "## Maturity evidence",
        "",
        "- API: " + str(axes.get("api", "N/A")),
        "- Rendering: " + str(axes.get("rendering", "N/A")),
        "- States: " + str(axes.get("states", "N/A")),
        "- Accessibility: " + str(axes.get("accessibility", "N/A")),
        "- Keyboard: " + str(axes.get("keyboard", "N/A")),
        "- HiDPI: " + str(axes.get("hidpi", "N/A")),
        "- RTL: " + str(axes.get("rtl", "N/A")),
        "- Tests: " + str(axes.get("tests", "N/A")),
        "- Example: " + str(axes.get("example", "N/A")),
        "- Docs: " + str(axes.get("docs", "N/A")),
        "",
    ]

    if axes.get("gaps"):
        lines += ["Known maturity gaps:", "", bullets(axes["gaps"], "None"), ""]

    nav = []
    if index:
        nav.append("[← " + entries[index - 1]["name"] + "](" + slug(entries[index - 1]) + ".md)")
    nav.append("[All components](index.md)")
    if index + 1 < len(entries):
        nav.append("[" + entries[index + 1]["name"] + " →](" + slug(entries[index + 1]) + ".md)")
    lines += ["---", "", " · ".join(nav), ""]

    filename = slug(entry) + ".md"
    (OUT / filename).write_text("\n".join(lines), encoding="utf-8")
    pages.append((entry, filename))

index_lines = [
    "# Components",
    "",
    "Generated consumer pages for every public registry component. Do not hand-edit generated pages.",
    "",
    f"**Catalog size:** {len(pages)} components.",
    "",
]
for family in dict.fromkeys(entry["family"] for entry, _ in pages):
    index_lines += ["## " + family, ""]
    index_lines += [
        "- ["
        + entry["name"]
        + "]("
        + filename
        + ") — "
        + entry["widgetType"]
        + " · "
        + entry["galleryRoute"]
        for entry, filename in pages
        if entry["family"] == family
    ]
    index_lines.append("")

(OUT / "index.md").write_text("\n".join(index_lines) + "\n", encoding="utf-8")
print("Generated " + str(len(pages)) + " component pages")
