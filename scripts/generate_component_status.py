#!/usr/bin/env python3
from __future__ import annotations

import argparse
import difflib
import json
import sys
from collections import Counter, defaultdict
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]

REGISTRY = ROOT / "docs" / "components" / "component-registry.json"
ROOT_STATUS = ROOT / "STATUS.md"
DOC_STATUS = ROOT / "docs" / "component-status.md"
MATURITY_DOC = ROOT / "docs" / "components" / "maturity.md"
TESTS_CMAKE = ROOT / "tests" / "CMakeLists.txt"

MATURITY_ORDER = {
    "complete": 0,
    "usable": 1,
    "partial": 2,
    "skeleton": 3,
    "planned": 4,
}

ALLOWED_MATURITY = set(MATURITY_ORDER)
ALLOWED_POLICIES = {"manual", "derived"}

AXES = [
    "api",
    "rendering",
    "states",
    "accessibility",
    "keyboard",
    "hidpi",
    "rtl",
    "tests",
    "example",
    "docs",
]

AXIS_LABELS = {
    "api": "API",
    "rendering": "Render",
    "states": "States",
    "accessibility": "A11y",
    "keyboard": "Keys",
    "hidpi": "HiDPI",
    "rtl": "RTL",
    "tests": "Tests",
    "example": "Example",
    "docs": "Docs",
}

OPTIONAL_AXIS_FIELDS = {
    "lastReviewed",
    "evidence",
    "gaps",
    "nextActions",
}

ALLOWED_AXIS_FIELDS = set(AXES) | OPTIONAL_AXIS_FIELDS

INTERACTIVE_FAMILIES = {
    "Buttons",
    "Inputs",
    "Selection",
    "Navigation",
    "Surfaces",
    "Compact controls",
}

DIRECTIONAL_FAMILIES = {
    "Buttons",
    "Inputs",
    "Selection",
    "Navigation",
    "Surfaces",
    "Data",
    "Data display",
}

CUSTOM_PAINTED_FAMILIES = {
    "Buttons",
    "Inputs",
    "Selection",
    "Navigation",
    "Surfaces",
    "Progress",
    "Data",
    "Data display",
}

BUTTON_IDS = {
    "button.text",
    "button.filled",
    "button.filled-tonal",
    "button.outlined",
    "button.elevated",
    "button.icon",
    "button.fab",
    "button.extended-fab",
}

BUTTON_EXPECTED_TEST_TARGETS = {
    "button.text": "tst_textbutton",
    "button.filled": "tst_filledbutton",
    "button.filled-tonal": "tst_filledtonalbutton",
    "button.outlined": "tst_outlinedbutton",
    "button.elevated": "tst_elevatedbutton",
    "button.icon": "tst_iconbutton",
    "button.fab": "tst_fab",
    "button.extended-fab": "tst_extendedfab",
}


class RegistryError(RuntimeError):
    pass


def load_registry() -> list[dict[str, Any]]:
    try:
        data = json.loads(REGISTRY.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        raise RegistryError(f"Invalid JSON in {REGISTRY}: {exc}") from exc

    if not isinstance(data, list):
        raise RegistryError("component-registry.json must contain a JSON array")

    for index, item in enumerate(data):
        if not isinstance(item, dict):
            raise RegistryError(f"component-registry.json item #{index} must be an object")

    return data


def sort_key(item: dict[str, Any]) -> tuple[str, int, str]:
    return (
        str(item.get("family", "Other")),
        MATURITY_ORDER.get(str(item.get("maturity", "planned")), 99),
        str(item.get("id", "")),
    )


def score(axes: dict[str, Any], axis: str) -> int | None:
    value = axes.get(axis)
    return value if isinstance(value, int) else None


def axis_md(value: Any) -> str:
    if value is None:
        return "?"
    if value == "N/A":
        return "N/A"
    return str(value)


def derived_maturity(axes: dict[str, Any]) -> str:
    api = score(axes, "api")
    rendering = score(axes, "rendering")
    states = score(axes, "states")
    accessibility = score(axes, "accessibility")
    keyboard = score(axes, "keyboard")
    hidpi = score(axes, "hidpi")
    tests = score(axes, "tests")
    example = score(axes, "example")
    docs = score(axes, "docs")

    if api is None or api < 1 or rendering is None or rendering == 0:
        return "planned"

    if (
        api >= 4
        and rendering is not None and rendering >= 4
        and states is not None and states >= 4
        and accessibility is not None and accessibility >= 3
        and keyboard is not None and keyboard >= 4
        and hidpi is not None and hidpi >= 3
        and tests is not None and tests >= 4
        and example is not None and example >= 4
        and docs is not None and docs >= 4
    ):
        return "complete"

    if (
        api >= 2
        and rendering is not None and rendering >= 2
        and states is not None and states >= 2
        and tests is not None and tests >= 1
        and example is not None and example >= 1
        and docs is not None and docs >= 1
    ):
        return "usable"

    if api >= 1 or rendering is not None or states is not None:
        return "partial"

    return "skeleton"


def effective_maturity(item: dict[str, Any]) -> str:
    policy = item.get("maturityPolicy", "manual")
    axes = item.get("maturityAxes")

    if policy == "derived" and isinstance(axes, dict):
        return derived_maturity(axes)

    maturity = item.get("maturity", "planned")
    return str(maturity) if maturity in ALLOWED_MATURITY else "planned"


def read_tests_cmake() -> str:
    if not TESTS_CMAKE.exists():
        return ""
    return TESTS_CMAKE.read_text(encoding="utf-8", errors="replace")


def validate_registry(components: list[dict[str, Any]], *, strict: bool) -> tuple[list[str], list[str]]:
    errors: list[str] = []
    warnings: list[str] = []
    tests_cmake = read_tests_cmake()
    seen_ids: set[str] = set()

    for index, item in enumerate(components):
        cid = str(item.get("id", ""))
        context = cid or f"item #{index}"

        if not cid:
            errors.append(f"{context}: missing required field `id`")
            continue

        if cid in seen_ids:
            errors.append(f"{cid}: duplicate component id")
        seen_ids.add(cid)

        for field in ["name", "family", "maturity", "publicHeader", "specType", "widgetType", "galleryRoute", "docsPath"]:
            if not item.get(field):
                errors.append(f"{cid}: missing required field `{field}`")

        declared_maturity = item.get("maturity")
        if declared_maturity not in ALLOWED_MATURITY:
            errors.append(f"{cid}: invalid maturity `{declared_maturity}`")

        policy = item.get("maturityPolicy", "manual")
        if policy not in ALLOWED_POLICIES:
            errors.append(f"{cid}: invalid maturityPolicy `{policy}`")

        gallery_route = item.get("galleryRoute")
        if isinstance(gallery_route, str) and gallery_route and not gallery_route.startswith("/"):
            errors.append(f"{cid}: galleryRoute must start with `/`")

        public_header = item.get("publicHeader")
        if isinstance(public_header, str) and public_header:
            header_path = ROOT / "include" / public_header
            if not header_path.exists():
                errors.append(f"{cid}: missing public header include/{public_header}")

        docs_path_value = item.get("docsPath")
        if isinstance(docs_path_value, str) and docs_path_value:
            docs_path = ROOT / docs_path_value
            if not docs_path.exists():
                errors.append(f"{cid}: missing docsPath {docs_path_value}")
            elif len(docs_path.read_text(encoding="utf-8", errors="replace").strip()) < 80:
                warnings.append(f"{cid}: docsPath {docs_path_value} looks very small")

        release_scope = bool(item.get("releaseScope", True))
        test_target = item.get("testTarget")

        if release_scope and not test_target:
            errors.append(f"{cid}: release-scope component must declare testTarget")

        if isinstance(test_target, str) and test_target and test_target not in tests_cmake:
            errors.append(f"{cid}: testTarget `{test_target}` is not registered in tests/CMakeLists.txt")

        if cid in BUTTON_EXPECTED_TEST_TARGETS:
            expected = BUTTON_EXPECTED_TEST_TARGETS[cid]
            if test_target != expected:
                warnings.append(f"{cid}: expected testTarget `{expected}`, got `{test_target}`")

        axes = item.get("maturityAxes")
        if axes is None:
            warnings.append(f"{cid}: missing maturityAxes")
            continue

        if not isinstance(axes, dict):
            errors.append(f"{cid}: maturityAxes must be an object")
            continue

        unknown_fields = sorted(set(axes) - ALLOWED_AXIS_FIELDS)
        for field in unknown_fields:
            errors.append(f"{cid}: unknown maturityAxes field `{field}`")

        for axis in AXES:
            value = axes.get(axis)
            if value is None or value == "N/A":
                continue
            if not isinstance(value, int) or value < 0 or value > 4:
                errors.append(f"{cid}: maturityAxes.{axis} must be 0..4, null, or " + '"N/A"')

        declared = str(item.get("maturity", "planned"))
        derived = derived_maturity(axes)
        if policy == "manual" and declared != derived:
            warnings.append(f"{cid}: declared maturity `{declared}` differs from derived maturity `{derived}`")

        if release_scope and effective_maturity(item) in {"skeleton", "planned"}:
            errors.append(f"{cid}: release-scope component cannot be `{effective_maturity(item)}`")

        family = str(item.get("family", ""))

        if family in INTERACTIVE_FAMILIES:
            if axes.get("accessibility") is None:
                warnings.append(f"{cid}: accessibility is not evaluated")
            if axes.get("keyboard") is None:
                warnings.append(f"{cid}: keyboard is not evaluated")

        if family in CUSTOM_PAINTED_FAMILIES and axes.get("hidpi") is None:
            warnings.append(f"{cid}: HiDPI is not evaluated")

        if family in DIRECTIONAL_FAMILIES and axes.get("rtl") is None:
            warnings.append(f"{cid}: RTL is not evaluated")

        if cid in {"button.icon", "button.fab"}:
            accessibility = score(axes, "accessibility")
            if effective_maturity(item) == "complete" and (accessibility is None or accessibility < 4):
                errors.append(f"{cid}: icon-only button cannot be complete without accessibility=4")

        if cid == "button.outlined":
            hidpi = score(axes, "hidpi")
            if effective_maturity(item) == "complete" and (hidpi is None or hidpi < 4):
                errors.append("button.outlined: cannot be complete without HiDPI stroke validation")

        if cid == "button.elevated":
            rendering = score(axes, "rendering")
            hidpi = score(axes, "hidpi")
            if effective_maturity(item) == "complete" and (
                rendering is None or rendering < 4 or hidpi is None or hidpi < 4
            ):
                errors.append("button.elevated: cannot be complete without rendering=4 and hidpi=4")

    missing_button_ids = sorted(BUTTON_IDS - seen_ids)
    if missing_button_ids:
        warnings.append("Buttons registry is incomplete; missing: " + ", ".join(missing_button_ids))

    if strict:
        errors.extend(warnings)
        warnings = []

    return errors, warnings


def group_by_family(components: list[dict[str, Any]]) -> dict[str, list[dict[str, Any]]]:
    grouped: dict[str, list[dict[str, Any]]] = defaultdict(list)
    for item in sorted(components, key=sort_key):
        grouped[str(item.get("family", "Other"))].append(item)
    return grouped


def render_root_status(components: list[dict[str, Any]]) -> str:
    counts = Counter(effective_maturity(item) for item in components)
    total = len(components)

    lines = [
        "# Project Status",
        "",
        "> Generated from `docs/components/component-registry.json` by `scripts/generate_component_status.py`.",
        "> Do not edit this file directly.",
        "",
        "## Component maturity summary",
        "",
        f"Total tracked components: **{total}**.",
        "",
        "| Maturity | Count |",
        "|---|---:|",
    ]

    for maturity in MATURITY_ORDER:
        lines.append(f"| `{maturity}` | {counts[maturity]} |")

    lines.extend([
        "",
        "## Release blockers",
        "",
    ])

    blockers = [
        item for item in components
        if bool(item.get("releaseScope", True)) and effective_maturity(item) in {"skeleton", "planned"}
    ]

    if blockers:
        for item in sorted(blockers, key=sort_key):
            lines.append(f"- `{item.get('id')}` — {item.get('name')} is `{effective_maturity(item)}`.")
    else:
        lines.append("No registry-level maturity blockers.")

    lines.extend([
        "",
        "## Generated documents",
        "",
        "- `docs/component-status.md` — compact component registry status.",
        "- `docs/components/maturity.md` — detailed maturity matrix.",
        "",
    ])

    return "\n".join(lines).rstrip() + "\n"


def render_component_status(components: list[dict[str, Any]]) -> str:
    grouped = group_by_family(components)

    lines = [
        "# Component status",
        "",
        "> Generated from `docs/components/component-registry.json` by `scripts/generate_component_status.py`.",
        "> Do not edit this file directly.",
        "",
        "Maturity levels: `complete`, `usable`, `partial`, `skeleton`, `planned`.",
        "",
    ]

    for family in sorted(grouped):
        lines.append(f"## {family}")
        lines.append("")
        lines.append("| ID | Component | Maturity | Header | Spec | Test | Docs | Gallery |")
        lines.append("|---|---|---:|---|---|---|---|---|")

        for item in grouped[family]:
            lines.append(
                "| `{id}` | {name} | `{maturity}` | `{header}` | `{spec}` | `{test}` | `{docs}` | `{gallery}` |".format(
                    id=item.get("id", ""),
                    name=item.get("name", ""),
                    maturity=effective_maturity(item),
                    header=item.get("publicHeader", ""),
                    spec=item.get("specType", ""),
                    test=item.get("testTarget", ""),
                    docs=item.get("docsPath", ""),
                    gallery=item.get("galleryRoute", ""),
                )
            )

        lines.append("")

    return "\n".join(lines).rstrip() + "\n"


def first_note(item: dict[str, Any]) -> str:
    axes = item.get("maturityAxes")
    if not isinstance(axes, dict):
        return ""

    gaps = axes.get("gaps")
    if isinstance(gaps, list) and gaps:
        return str(gaps[0])

    actions = axes.get("nextActions")
    if isinstance(actions, list) and actions:
        return str(actions[0])

    return ""


def render_not_evaluated_summary(components: list[dict[str, Any]]) -> list[str]:
    counts: Counter[str] = Counter()

    for item in components:
        axes = item.get("maturityAxes")
        if not isinstance(axes, dict):
            for axis in AXES:
                counts[axis] += 1
            continue

        for axis in AXES:
            if axes.get(axis) is None:
                counts[axis] += 1

    lines = [
        "## Not evaluated axes",
        "",
        "| Axis | Count |",
        "|---|---:|",
    ]

    for axis in AXES:
        lines.append(f"| {AXIS_LABELS[axis]} | {counts[axis]} |")

    lines.append("")
    return lines


def render_next_actions(components: list[dict[str, Any]]) -> list[str]:
    rows: list[tuple[str, str]] = []

    for item in sorted(components, key=sort_key):
        axes = item.get("maturityAxes")
        if not isinstance(axes, dict):
            continue

        actions = axes.get("nextActions")
        if not isinstance(actions, list):
            continue

        for action in actions:
            rows.append((str(item.get("name", "")), str(action)))

    lines = [
        "## Next actions",
        "",
        "| Component | Action |",
        "|---|---|",
    ]

    if rows:
        for component, action in rows:
            lines.append(f"| {component} | {action} |")
    else:
        lines.append("| — | No next actions declared. |")

    lines.append("")
    return lines


def render_button_completion_gate(components: list[dict[str, Any]]) -> list[str]:
    by_id = {str(item.get("id", "")): item for item in components}

    lines = [
        "## Buttons completion gate",
        "",
        "The Buttons family can be considered `complete` only when all eight public variants are tracked and every required axis is evaluated.",
        "",
        "| Button | Tracked | Status | Missing axes |",
        "|---|---:|---:|---|",
    ]

    for cid in sorted(BUTTON_IDS):
        item = by_id.get(cid)
        if item is None:
            lines.append(f"| `{cid}` | no | — | all |")
            continue

        axes = item.get("maturityAxes")
        missing = []
        if not isinstance(axes, dict):
            missing = AXES[:]
        else:
            for axis in AXES:
                if axes.get(axis) is None:
                    missing.append(axis)

        lines.append(
            f"| `{cid}` | yes | `{effective_maturity(item)}` | {', '.join(missing) if missing else '—'} |"
        )

    lines.append("")
    return lines


def render_maturity_doc(components: list[dict[str, Any]]) -> str:
    grouped = group_by_family(components)

    lines = [
        "# Component Maturity Matrix",
        "",
        "> Generated from `docs/components/component-registry.json` by `scripts/generate_component_status.py`.",
        "> Do not edit this file directly.",
        "",
        "## Legend",
        "",
        "| Value | Meaning |",
        "|---|---|",
        "| `0` | Not started |",
        "| `1` | Prototype, scaffold, or incomplete implementation |",
        "| `2` | Usable in simple cases, with known gaps |",
        "| `3` | Functional, coherent, and reasonably covered |",
        "| `4` | Robust, documented, tested, and consistent |",
        "| `?` | Not evaluated yet |",
        "| `N/A` | Not applicable |",
        "",
        "## Matrix",
        "",
    ]

    header = "| ID | Component | Status | " + " | ".join(AXIS_LABELS[axis] for axis in AXES) + " | Notes |"
    separator = "|---|---|---:|" + "|".join("---:" for _ in AXES) + "|---|"

    for family in sorted(grouped):
        lines.append(f"### {family}")
        lines.append("")
        lines.append(header)
        lines.append(separator)

        for item in grouped[family]:
            axes = item.get("maturityAxes")
            if not isinstance(axes, dict):
                axes = {}

            axis_cells = " | ".join(axis_md(axes.get(axis)) for axis in AXES)
            lines.append(
                f"| `{item.get('id', '')}` | {item.get('name', '')} | `{effective_maturity(item)}` | {axis_cells} | {first_note(item)} |"
            )

        lines.append("")

    lines.extend(render_button_completion_gate(components))
    lines.extend(render_not_evaluated_summary(components))
    lines.extend(render_next_actions(components))

    return "\n".join(lines).rstrip() + "\n"


def write_or_check(path: Path, content: str, *, check: bool) -> bool:
    if check:
        current = path.read_text(encoding="utf-8") if path.exists() else ""
        if current == content:
            return True

        rel = path.relative_to(ROOT)
        print(f"{rel} is out of date")
        for line in difflib.unified_diff(
            current.splitlines(),
            content.splitlines(),
            fromfile=f"{rel} current",
            tofile=f"{rel} generated",
            lineterm="",
        ):
            print(line)
        return False

    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="utf-8")
    print(f"wrote {path.relative_to(ROOT)}")
    return True


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate component status and maturity documents.")
    parser.add_argument("--check", action="store_true", help="fail if generated files are out of date")
    parser.add_argument("--strict", action="store_true", help="treat registry warnings as errors")
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)

    try:
        components = load_registry()
    except RegistryError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    errors, warnings = validate_registry(components, strict=args.strict)

    if warnings:
        print("component registry warnings:")
        for warning in warnings:
            print(f" - {warning}")

    if errors:
        print("component registry validation failed:")
        for error in errors:
            print(f" - {error}")
        return 1

    outputs = {
        ROOT_STATUS: render_root_status(components),
        DOC_STATUS: render_component_status(components),
        MATURITY_DOC: render_maturity_doc(components),
    }

    ok = True
    for path, content in outputs.items():
        ok &= write_or_check(path, content, check=args.check)

    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
