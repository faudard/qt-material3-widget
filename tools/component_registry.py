from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
REGISTRY = ROOT / "docs" / "components" / "component-registry.json"
TESTS_CMAKE = ROOT / "tests" / "CMakeLists.txt"

MATURITY_ORDER = {
    "complete": 0,
    "usable": 1,
    "partial": 2,
    "skeleton": 3,
    "planned": 4,
}
ALLOWED_MATURITY = set(MATURITY_ORDER)
CPP_MATURITY = {
    "complete": "Complete",
    "usable": "Usable",
    "partial": "Partial",
    "skeleton": "Skeleton",
    "planned": "Planned",
}
ALLOWED_POLICIES = {"manual", "derived"}
AXES = [
    "api", "rendering", "states", "accessibility", "keyboard",
    "hidpi", "rtl", "tests", "example", "docs",
]
AXIS_LABELS = {
    "api": "API", "rendering": "Render", "states": "States",
    "accessibility": "A11y", "keyboard": "Keys", "hidpi": "HiDPI",
    "rtl": "RTL", "tests": "Tests", "example": "Example", "docs": "Docs",
}
OPTIONAL_AXIS_FIELDS = {"lastReviewed", "evidence", "gaps", "nextActions"}
ALLOWED_AXIS_FIELDS = set(AXES) | OPTIONAL_AXIS_FIELDS
INTERACTIVE_FAMILIES = {
    "Buttons", "Inputs", "Selection", "Navigation", "Surfaces",
    "Compact controls",
}
DIRECTIONAL_FAMILIES = {
    "Buttons", "Inputs", "Selection", "Navigation", "Surfaces",
    "Data", "Data display",
}
CUSTOM_PAINTED_FAMILIES = {
    "Buttons", "Inputs", "Selection", "Navigation", "Surfaces",
    "Progress", "Data", "Data display",
}
BUTTON_IDS = {
    "button.text", "button.filled", "button.filled-tonal", "button.outlined",
    "button.elevated", "button.icon", "button.fab", "button.extended-fab",
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


def load_registry(root: Path = ROOT) -> list[dict[str, Any]]:
    registry = root / "docs" / "components" / "component-registry.json"
    try:
        data = json.loads(registry.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        raise RegistryError(f"Invalid JSON in {registry}: {exc}") from exc
    if not isinstance(data, list):
        raise RegistryError("component-registry.json must contain a JSON array")
    for index, item in enumerate(data):
        if not isinstance(item, dict):
            raise RegistryError(
                f"component-registry.json item #{index} must be an object"
            )
    return data


def sort_key(item: dict[str, Any]) -> tuple[str, int, str]:
    return (
        str(item.get("family", "Other")),
        MATURITY_ORDER.get(str(item.get("maturity", "planned")), 99),
        str(item.get("id", "")),
    )


def score(axes: dict[str, Any], axis: str) -> int | None:
    value = axes.get(axis)
    if value == "N/A":
        return 4
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
        and rendering >= 4
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
        and rendering >= 2
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


def read_tests_cmake(root: Path = ROOT) -> str:
    path = root / "tests" / "CMakeLists.txt"
    if not path.exists():
        return ""
    return path.read_text(encoding="utf-8", errors="replace")


def validate_registry(
    components: list[dict[str, Any]],
    *,
    strict: bool,
    root: Path = ROOT,
) -> tuple[list[str], list[str]]:
    errors: list[str] = []
    warnings: list[str] = []
    tests_cmake = read_tests_cmake(root)
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

        for field in [
            "name", "family", "maturity", "publicHeader", "specType",
            "widgetType", "galleryRoute", "docsPath",
        ]:
            if not item.get(field):
                errors.append(f"{cid}: missing required field `{field}`")

        declared_maturity = item.get("maturity")
        if declared_maturity not in ALLOWED_MATURITY:
            errors.append(f"{cid}: invalid maturity `{declared_maturity}`")

        policy = item.get("maturityPolicy", "manual")
        if policy not in ALLOWED_POLICIES:
            errors.append(f"{cid}: invalid maturityPolicy `{policy}`")

        gallery_route = item.get("galleryRoute")
        if (
            isinstance(gallery_route, str)
            and gallery_route
            and not gallery_route.startswith("/")
        ):
            errors.append(f"{cid}: galleryRoute must start with `/`")

        public_header = item.get("publicHeader")
        if isinstance(public_header, str) and public_header:
            header_path = root / "include" / public_header
            if not header_path.exists():
                errors.append(
                    f"{cid}: missing public header include/{public_header}"
                )

        docs_path_value = item.get("docsPath")
        if isinstance(docs_path_value, str) and docs_path_value:
            docs_path = root / docs_path_value
            if not docs_path.exists():
                errors.append(f"{cid}: missing docsPath {docs_path_value}")
            elif len(
                docs_path.read_text(
                    encoding="utf-8", errors="replace"
                ).strip()
            ) < 80:
                warnings.append(
                    f"{cid}: docsPath {docs_path_value} looks very small"
                )

        release_scope = bool(item.get("releaseScope", True))
        test_target = item.get("testTarget")
        if release_scope and not test_target:
            errors.append(
                f"{cid}: release-scope component must declare testTarget"
            )
        if (
            isinstance(test_target, str)
            and test_target
            and test_target not in tests_cmake
        ):
            errors.append(
                f"{cid}: testTarget `{test_target}` is not registered "
                "in tests/CMakeLists.txt"
            )
        if cid in BUTTON_EXPECTED_TEST_TARGETS:
            expected = BUTTON_EXPECTED_TEST_TARGETS[cid]
            if test_target != expected:
                warnings.append(
                    f"{cid}: expected testTarget `{expected}`, "
                    f"got `{test_target}`"
                )

        axes = item.get("maturityAxes")
        if axes is None:
            warnings.append(f"{cid}: missing maturityAxes")
            continue
        if not isinstance(axes, dict):
            errors.append(f"{cid}: maturityAxes must be an object")
            continue

        for field in sorted(set(axes) - ALLOWED_AXIS_FIELDS):
            errors.append(
                f"{cid}: unknown maturityAxes field `{field}`"
            )
        for axis in AXES:
            value = axes.get(axis)
            if value is None or value == "N/A":
                continue
            if not isinstance(value, int) or value < 0 or value > 4:
                errors.append(
                    f"{cid}: maturityAxes.{axis} must be 0..4, null, "
                    'or "N/A"'
                )

        declared = str(item.get("maturity", "planned"))
        derived = derived_maturity(axes)
        if policy == "manual" and declared != derived:
            warnings.append(
                f"{cid}: declared maturity `{declared}` differs from "
                f"derived maturity `{derived}`"
            )
        if release_scope and effective_maturity(item) in {
            "skeleton", "planned"
        }:
            errors.append(
                f"{cid}: release-scope component cannot be "
                f"`{effective_maturity(item)}`"
            )

        family = str(item.get("family", ""))
        if family in INTERACTIVE_FAMILIES:
            if axes.get("accessibility") is None:
                warnings.append(f"{cid}: accessibility is not evaluated")
            if axes.get("keyboard") is None:
                warnings.append(f"{cid}: keyboard is not evaluated")
        if (
            family in CUSTOM_PAINTED_FAMILIES
            and axes.get("hidpi") is None
        ):
            warnings.append(f"{cid}: HiDPI is not evaluated")
        if (
            family in DIRECTIONAL_FAMILIES
            and axes.get("rtl") is None
        ):
            warnings.append(f"{cid}: RTL is not evaluated")

        if cid in {"button.icon", "button.fab"}:
            accessibility = score(axes, "accessibility")
            if (
                effective_maturity(item) == "complete"
                and (accessibility is None or accessibility < 4)
            ):
                errors.append(
                    f"{cid}: icon-only button cannot be complete "
                    "without accessibility=4"
                )
        if cid == "button.outlined":
            hidpi = score(axes, "hidpi")
            if (
                effective_maturity(item) == "complete"
                and (hidpi is None or hidpi < 4)
            ):
                errors.append(
                    "button.outlined: cannot be complete without "
                    "HiDPI stroke validation"
                )
        if cid == "button.elevated":
            rendering = score(axes, "rendering")
            hidpi = score(axes, "hidpi")
            if (
                effective_maturity(item) == "complete"
                and (
                    rendering is None
                    or rendering < 4
                    or hidpi is None
                    or hidpi < 4
                )
            ):
                errors.append(
                    "button.elevated: cannot be complete without "
                    "rendering=4 and hidpi=4"
                )

    missing_button_ids = sorted(BUTTON_IDS - seen_ids)
    if missing_button_ids:
        warnings.append(
            "Buttons registry is incomplete; missing: "
            + ", ".join(missing_button_ids)
        )
    if strict:
        errors.extend(warnings)
        warnings = []
    return errors, warnings
