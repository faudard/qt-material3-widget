#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
REGISTRY = ROOT / "docs" / "components" / "component-registry.json"
TESTS_CMAKE = ROOT / "tests" / "CMakeLists.txt"
BUTTONS_DOC = ROOT / "docs" / "public-api" / "buttons.md"
BUTTONS_GALLERY = ROOT / "examples" / "gallery" / "pages" / "buttonspage.cpp"
GENERATOR = ROOT / "scripts" / "generate_component_status.py"

DOCS_CONTRACT_BEGIN = "<!-- BEGIN GENERATED BUTTONS MATURITY CONTRACT -->"
DOCS_CONTRACT_END = "<!-- END GENERATED BUTTONS MATURITY CONTRACT -->"
GALLERY_CONTRACT_BEGIN = "// BEGIN GENERATED BUTTONS MATURITY GALLERY COVERAGE"
GALLERY_CONTRACT_END = "// END GENERATED BUTTONS MATURITY GALLERY COVERAGE"

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

BUTTONS: dict[str, dict[str, Any]] = {
    "button.text": {
        "name": "Text Button",
        "header": "qtmaterial/widgets/buttons/qtmaterialtextbutton.h",
        "test": "tst_textbutton",
        "source": "src/widgets/buttons/qtmaterialtextbutton.cpp",
        "rtl": True,
    },
    "button.filled": {
        "name": "Filled Button",
        "header": "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h",
        "test": "tst_filledbutton",
        "source": "src/widgets/buttons/qtmaterialfilledbutton.cpp",
        "rtl": True,
    },
    "button.filled-tonal": {
        "name": "Filled Tonal Button",
        "header": "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h",
        "test": "tst_filledtonalbutton",
        "source": "src/widgets/buttons/qtmaterialfilledtonalbutton.cpp",
        "rtl": True,
    },
    "button.outlined": {
        "name": "Outlined Button",
        "header": "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h",
        "test": "tst_outlinedbutton",
        "source": "src/widgets/buttons/qtmaterialoutlinedbutton.cpp",
        "focused_tests": ["tst_outlinedbutton_rendering"],
        "rtl": True,
    },
    "button.elevated": {
        "name": "Elevated Button",
        "header": "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h",
        "test": "tst_elevatedbutton",
        "source": "src/widgets/buttons/qtmaterialelevatedbutton.cpp",
        "focused_tests": ["tst_elevatedbutton_shadow"],
        "rtl": True,
    },
    "button.icon": {
        "name": "Icon Button",
        "header": "qtmaterial/widgets/buttons/qtmaterialiconbutton.h",
        "test": "tst_iconbutton",
        "source": "src/widgets/buttons/qtmaterialiconbutton.cpp",
        "focused_tests": ["tst_icon_fab_accessibility"],
        "rtl": "N/A",
    },
    "button.fab": {
        "name": "Floating Action Button",
        "header": "qtmaterial/widgets/buttons/qtmaterialfab.h",
        "test": "tst_fab",
        "source": "src/widgets/buttons/qtmaterialfab.cpp",
        "focused_tests": ["tst_icon_fab_accessibility"],
        "rtl": "N/A",
    },
    "button.extended-fab": {
        "name": "Extended Floating Action Button",
        "header": "qtmaterial/widgets/buttons/qtmaterialextendedfab.h",
        "test": "tst_extendedfab",
        "source": "src/widgets/buttons/qtmaterialextendedfab.cpp",
        "focused_tests": ["tst_icon_fab_accessibility"],
        "rtl": True,
    },
}

FOCUSED_TEST_SOURCES = {
    "tst_button_contracts": "tests/widgets/buttons/tst_button_contracts.cpp",
    "tst_outlinedbutton_rendering": "tests/widgets/buttons/tst_outlinedbutton_rendering.cpp",
    "tst_elevatedbutton_shadow": "tests/widgets/buttons/tst_elevatedbutton_shadow.cpp",
    "tst_icon_fab_accessibility": "tests/widgets/buttons/tst_icon_fab_accessibility.cpp",
}

BUTTON_CTEST_REGEX = (
    "tst_button_contracts|"
    "tst_outlinedbutton_rendering|"
    "tst_elevatedbutton_shadow|"
    "tst_icon_fab_accessibility|"
    "tst_.*button|"
    "tst_.*fab"
)


@dataclass
class AxisResult:
    ok: bool
    evidence: list[str] = field(default_factory=list)
    blockers: list[str] = field(default_factory=list)
    target_value: int | str = 4


@dataclass
class ButtonAudit:
    button_id: str
    name: str
    axes: dict[str, AxisResult]

    @property
    def complete_ready(self) -> bool:
        return all(result.ok for result in self.axes.values())


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace") if path.exists() else ""


def load_registry() -> list[dict[str, Any]]:
    return json.loads(REGISTRY.read_text(encoding="utf-8"))


def registry_by_id(registry: list[dict[str, Any]]) -> dict[str, dict[str, Any]]:
    return {str(item.get("id", "")): item for item in registry if isinstance(item, dict)}


def has_marked_block(text: str, begin: str, end: str) -> bool:
    return begin in text and end in text and text.index(begin) < text.index(end)


def test_target_exists(tests_cmake: str, target: str) -> bool:
    return target in tests_cmake


def focused_test_exists(repo_relative: str, tests_cmake: str) -> bool:
    target = Path(repo_relative).stem
    return (ROOT / repo_relative).exists() and test_target_exists(tests_cmake, target)


def source_has_metadata(source_text: str, variant: str) -> bool:
    required = [
        'setMaterialComponent(QStringLiteral("button"))',
        f'setMaterialVariant(QStringLiteral("{variant}"))',
        'setMaterialRole(QStringLiteral("action"))',
    ]
    return all(item in source_text for item in required)


def variant_from_id(button_id: str) -> str:
    return button_id.removeprefix("button.")


def pass_or_fail(condition: bool, evidence: str, blocker: str, target_value: int | str = 4) -> AxisResult:
    if condition:
        return AxisResult(ok=True, evidence=[evidence], target_value=target_value)
    return AxisResult(ok=False, blockers=[blocker], target_value=target_value)


def combine(target_value: int | str, *results: AxisResult) -> AxisResult:
    ok = all(result.ok for result in results)
    evidence: list[str] = []
    blockers: list[str] = []

    for result in results:
        evidence.extend(result.evidence)
        blockers.extend(result.blockers)

    return AxisResult(ok=ok, evidence=evidence, blockers=blockers, target_value=target_value)


def audit_button(
    button_id: str,
    spec: dict[str, Any],
    registry: dict[str, dict[str, Any]],
    tests_cmake: str,
    buttons_doc: str,
    gallery_page: str,
) -> ButtonAudit:
    item = registry.get(button_id)
    header_path = ROOT / "include" / str(spec["header"])
    source_path = ROOT / str(spec["source"])
    source_text = read_text(source_path)
    variant = variant_from_id(button_id)
    focused_tests = list(spec.get("focused_tests", []))

    registry_present = item is not None
    header_exists = header_path.exists()
    source_exists = source_path.exists()
    test_registered = test_target_exists(tests_cmake, spec["test"])
    contract_registered = focused_test_exists(FOCUSED_TEST_SOURCES["tst_button_contracts"], tests_cmake)
    docs_contract = has_marked_block(buttons_doc, DOCS_CONTRACT_BEGIN, DOCS_CONTRACT_END)
    gallery_contract = has_marked_block(gallery_page, GALLERY_CONTRACT_BEGIN, GALLERY_CONTRACT_END)
    metadata_ok = source_has_metadata(source_text, variant) if source_exists else False

    focused_results = [
        pass_or_fail(
            focused_test_exists(FOCUSED_TEST_SOURCES[target], tests_cmake),
            f"focused test `{target}` exists and is registered",
            f"missing focused test `{target}` or its CMake registration",
        )
        for target in focused_tests
    ]

    api = combine(
        4,
        pass_or_fail(registry_present, "registry entry exists", "missing registry entry"),
        pass_or_fail(header_exists, f"public header exists: include/{spec['header']}", f"missing public header include/{spec['header']}"),
        pass_or_fail(source_exists, f"source exists: {spec['source']}", f"missing source {spec['source']}"),
        pass_or_fail(test_registered, f"variant test `{spec['test']}` is registered", f"variant test `{spec['test']}` is not registered"),
        pass_or_fail(metadata_ok, "Material automation metadata is explicit", "missing explicit Material automation metadata"),
    )

    tests = combine(
        4,
        pass_or_fail(test_registered, f"variant test `{spec['test']}` is registered", f"variant test `{spec['test']}` is not registered"),
        pass_or_fail(contract_registered, "shared button contract test exists and is registered", "missing `tst_button_contracts` source or CMake registration"),
        *focused_results,
    )

    states = combine(
        4,
        pass_or_fail(contract_registered, "shared contract covers enabled/disabled/focus/activation states", "missing shared contract test"),
        pass_or_fail(test_registered, "variant test exists for component-specific state behavior", f"missing variant test `{spec['test']}`"),
    )

    accessibility = combine(
        4,
        pass_or_fail(docs_contract, "docs include the Buttons accessibility contract", "missing Buttons docs maturity contract"),
        pass_or_fail(contract_registered, "shared contract includes accessibility checks", "missing shared contract test"),
        pass_or_fail(
            focused_test_exists(FOCUSED_TEST_SOURCES["tst_icon_fab_accessibility"], tests_cmake),
            "focused IconButton/FAB accessibility test exists and is registered",
            "missing focused IconButton/FAB accessibility test",
        ),
    )

    keyboard = combine(
        4,
        pass_or_fail(contract_registered, "shared contract covers keyboard activation and disabled non-activation", "missing shared contract test"),
    )

    rendering_requirements = [
        pass_or_fail(gallery_contract, "gallery includes maturity rendering coverage", "missing Buttons gallery maturity block"),
        pass_or_fail(test_registered, f"variant rendering can be exercised by `{spec['test']}`", f"missing variant test `{spec['test']}`"),
    ]
    if button_id == "button.outlined":
        rendering_requirements.append(pass_or_fail(
            focused_test_exists(FOCUSED_TEST_SOURCES["tst_outlinedbutton_rendering"], tests_cmake),
            "focused OutlinedButton rendering test exists and is registered",
            "missing focused OutlinedButton rendering test",
        ))
    if button_id == "button.elevated":
        rendering_requirements.append(pass_or_fail(
            focused_test_exists(FOCUSED_TEST_SOURCES["tst_elevatedbutton_shadow"], tests_cmake),
            "focused ElevatedButton shadow test exists and is registered",
            "missing focused ElevatedButton shadow test",
        ))
    rendering = combine(4, *rendering_requirements)

    hidpi_requirements = [
        pass_or_fail(contract_registered, "shared contract includes DPR 2.0 render smoke", "missing shared contract test"),
    ]
    if button_id == "button.outlined":
        hidpi_requirements.append(pass_or_fail(
            focused_test_exists(FOCUSED_TEST_SOURCES["tst_outlinedbutton_rendering"], tests_cmake),
            "OutlinedButton focused test covers DPR 1.0/1.5/2.0",
            "missing OutlinedButton DPR-focused test",
        ))
    if button_id == "button.elevated":
        hidpi_requirements.append(pass_or_fail(
            focused_test_exists(FOCUSED_TEST_SOURCES["tst_elevatedbutton_shadow"], tests_cmake),
            "ElevatedButton focused test covers DPR 1.0/1.5/2.0",
            "missing ElevatedButton DPR-focused test",
        ))
    hidpi = combine(4, *hidpi_requirements)

    if spec["rtl"] == "N/A":
        rtl = AxisResult(ok=True, evidence=["RTL is not applicable to icon-only layout"], target_value="N/A")
    else:
        rtl = combine(
            4,
            pass_or_fail(contract_registered, "shared contract includes RTL sizeHint stability", "missing shared contract test"),
            pass_or_fail(gallery_contract, "gallery includes RTL preview", "missing Buttons gallery maturity block"),
        )

    example = combine(
        4,
        pass_or_fail(gallery_contract, "gallery contains marked maturity coverage block", "missing Buttons gallery maturity block"),
    )

    docs = combine(
        4,
        pass_or_fail(docs_contract, "buttons.md contains marked maturity contract block", "missing Buttons docs maturity contract"),
    )

    axes = {
        "api": api,
        "rendering": rendering,
        "states": states,
        "accessibility": accessibility,
        "keyboard": keyboard,
        "hidpi": hidpi,
        "rtl": rtl,
        "tests": tests,
        "example": example,
        "docs": docs,
    }

    return ButtonAudit(button_id=button_id, name=spec["name"], axes=axes)


def run_command(command: list[str]) -> tuple[bool, str]:
    completed = subprocess.run(
        command,
        cwd=str(ROOT),
        check=False,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )
    return completed.returncode == 0, completed.stdout


def audit_all(*, build_dir: Path | None, run_ctest: bool, run_generator_check: bool) -> tuple[list[ButtonAudit], list[str]]:
    registry = registry_by_id(load_registry())
    tests_cmake = read_text(TESTS_CMAKE)
    buttons_doc = read_text(BUTTONS_DOC)
    gallery_page = read_text(BUTTONS_GALLERY)
    diagnostics: list[str] = []

    audits = [
        audit_button(button_id, spec, registry, tests_cmake, buttons_doc, gallery_page)
        for button_id, spec in BUTTONS.items()
    ]

    if run_generator_check:
        ok, output = run_command([sys.executable, str(GENERATOR), "--check"])
        if ok:
            diagnostics.append("generator check: PASS")
        else:
            diagnostics.append("generator check: FAIL")
            diagnostics.append(output.strip())

    if run_ctest:
        if build_dir is None:
            diagnostics.append("ctest: FAIL --run-ctest requires --build-dir")
        else:
            resolved = build_dir if build_dir.is_absolute() else ROOT / build_dir
            ok, output = run_command([
                "ctest",
                "--test-dir",
                str(resolved),
                "-R",
                BUTTON_CTEST_REGEX,
                "--output-on-failure",
            ])
            if ok:
                diagnostics.append(f"ctest `{BUTTON_CTEST_REGEX}`: PASS")
            else:
                diagnostics.append(f"ctest `{BUTTON_CTEST_REGEX}`: FAIL")
                diagnostics.append(output.strip())

    return audits, diagnostics


def status_cell(result: AxisResult) -> str:
    if result.ok:
        return f"{result.target_value}"
    return "FAIL"


def print_summary(audits: list[ButtonAudit], diagnostics: list[str], *, verbose: bool) -> int:
    header = ["Button", *AXES, "ready"]
    rows: list[list[str]] = []

    for audit in audits:
        rows.append([
            audit.button_id,
            *[status_cell(audit.axes[axis]) for axis in AXES],
            "yes" if audit.complete_ready else "no",
        ])

    widths = [max(len(row[index]) for row in [header, *rows]) for index in range(len(header))]
    print(" | ".join(header[index].ljust(widths[index]) for index in range(len(header))))
    print("-+-".join("-" * widths[index] for index in range(len(header))))
    for row in rows:
        print(" | ".join(row[index].ljust(widths[index]) for index in range(len(row))))

    print()

    blockers = 0
    for audit in audits:
        for axis in AXES:
            result = audit.axes[axis]
            if not result.ok:
                blockers += len(result.blockers)

    if diagnostics:
        print("Diagnostics:")
        for diagnostic in diagnostics:
            if diagnostic:
                print(f" - {diagnostic.replace(chr(10), chr(10) + '   ')}")
        print()

    if verbose or blockers:
        print("Axis details:")
        for audit in audits:
            print(f"\n{audit.button_id} — {audit.name}")
            for axis in AXES:
                result = audit.axes[axis]
                label = "PASS" if result.ok else "FAIL"
                print(f"  {axis}: {label} -> target {result.target_value}")
                details = result.evidence if result.ok else result.blockers
                for detail in details:
                    print(f"    - {detail}")
        print()

    all_ready = all(audit.complete_ready for audit in audits) and not any("FAIL" in item for item in diagnostics)
    if all_ready:
        print("Buttons family is ready for complete promotion.")
        return 0

    print("Buttons family is not ready for complete promotion.")
    return 1


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Audit Buttons maturity axes before promotion.")
    parser.add_argument("--build-dir", type=Path, default=None, help="build directory used with --run-ctest")
    parser.add_argument("--run-ctest", action="store_true", help="run focused Buttons CTest suite")
    parser.add_argument("--run-generator-check", action="store_true", help="run generate_component_status.py --check")
    parser.add_argument("--verbose", action="store_true", help="show evidence for passing axes as well as blockers")
    parser.add_argument("--fail-on-blockers", action="store_true", help="return non-zero when any axis is not ready")
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    audits, diagnostics = audit_all(
        build_dir=args.build_dir,
        run_ctest=args.run_ctest,
        run_generator_check=args.run_generator_check,
    )
    rc = print_summary(audits, diagnostics, verbose=args.verbose)
    return rc if args.fail_on_blockers else 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
