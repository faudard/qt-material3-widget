#!/usr/bin/env python3
from __future__ import annotations

import json
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REGISTRY = ROOT / "docs" / "components" / "component-registry.json"
STATUS = ROOT / "STATUS.md"
DOC_STATUS = ROOT / "docs" / "component-status.md"

MATURITY_ORDER = {"complete": 0, "usable": 1, "partial": 2, "skeleton": 3, "planned": 4}


def marker(path: Path, text: str) -> str:
    return "yes" if (ROOT / path).exists() and text else "missing"


def main() -> int:
    components = json.loads(REGISTRY.read_text(encoding="utf-8"))
    components.sort(key=lambda item: (item.get("family", ""), MATURITY_ORDER.get(item.get("maturity", "planned"), 99), item.get("id", "")))

    by_family = defaultdict(list)
    for item in components:
        by_family[item.get("family", "Other")].append(item)

    lines = [
        "# Component status",
        "",
        "Generated from `docs/components/component-registry.json`.",
        "",
        "Maturity levels: `complete`, `usable`, `partial`, `skeleton`, `planned`.",
        "",
    ]

    for family in sorted(by_family):
        lines.append(f"## {family}")
        lines.append("")
        lines.append("| ID | Component | Maturity | Header | Spec | Test | Docs | Gallery |")
        lines.append("|---|---|---:|---|---|---|---|---|")
        for item in by_family[family]:
            lines.append(
                "| {id} | {name} | `{maturity}` | `{publicHeader}` | `{specType}` | `{testTarget}` | `{docsPath}` | `{galleryRoute}` |".format(
                    id=item.get("id", ""),
                    name=item.get("name", ""),
                    maturity=item.get("maturity", "planned"),
                    publicHeader=item.get("publicHeader", ""),
                    specType=item.get("specType", ""),
                    testTarget=item.get("testTarget", ""),
                    docsPath=item.get("docsPath", ""),
                    galleryRoute=item.get("galleryRoute", ""),
                )
            )
        lines.append("")

    blockers = [item for item in components if item.get("maturity") in {"skeleton", "planned"}]
    lines.append("## Release blockers")
    lines.append("")
    if blockers:
        for item in blockers:
            lines.append(f"- `{item['id']}` — {item['name']} is `{item.get('maturity', 'planned')}`.")
    else:
        lines.append("No registry-level maturity blockers.")
    lines.append("")

    output = "\n".join(lines).rstrip() + "\n"
    STATUS.write_text(output, encoding="utf-8")
    DOC_STATUS.parent.mkdir(parents=True, exist_ok=True)
    DOC_STATUS.write_text(output, encoding="utf-8")
    print(f"wrote {STATUS.relative_to(ROOT)}")
    print(f"wrote {DOC_STATUS.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
