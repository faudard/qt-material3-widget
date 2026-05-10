#!/usr/bin/env python3
"""
Promote the Buttons family to complete only when maturity prerequisites are satisfied.

Default behavior is safe: dry-run only.

Usage:
  python scripts/promote_buttons_maturity.py
  python scripts/promote_buttons_maturity.py --dry-run
  python scripts/promote_buttons_maturity.py --apply
  python scripts/promote_buttons_maturity.py --repo /path/to/qt-material3-widget
  python scripts/promote_buttons_maturity.py --build-dir build --run-ctest
  python scripts/promote_buttons_maturity.py --apply --build-dir build --run-ctest
  python scripts/promote_buttons_maturity.py --apply --force

What it does:
  - Audits the Buttons maturity prerequisites.
  - Refuses promotion when blockers remain, unless --force is passed.
  - Updates all eight Buttons entries in docs/components/component-registry.json:
      * maturity: complete
      * maturityPolicy: manual
      * maturityAxes api/rendering/states/accessibility/keyboard/hidpi/tests/example/docs: 4
      * maturityAxes rtl: 4 for text-bearing/directional buttons, N/A for icon-only variants
      * evidence populated
      * gaps cleared
      * nextActions set to a maintenance message
      * lastReviewed set to today's date
  - Shows a unified diff in dry-run mode.
  - Writes only with --apply.

This script is deliberately conservative. It checks repository evidence, not visual perfection.
Use --run-ctest with --build-dir to make the promotion depend on the actual Buttons test suite passing.
"""

from __future__ import annotations

import argparse
import copy
import difflib
import json
import subprocess
import sys
from dataclasses import dataclass
from datetime import date
from pathlib import Path
from typing import Any


REGISTRY_RELATIVE_PATH = Path("docs/components/component-registry.json")
TESTS_CMAKE_RELATIVE_PATH = Path("tests/CMakeLists.txt")
BUTTONS_DOC_RELATIVE_PATH = Path("docs/public-api/buttons.md")
BUTTONS_GALLERY_RELATIVE_PATH = Path("examples/gallery/pages/buttonspage.cpp")
GENERATOR_RELATIVE_PATH = Path("scripts/generate_component_status.py")

DOCS_CONTRACT_BEGIN = "<!-- BEGIN GENERATED BUTTONS MATURITY CONTRACT -->"
DOCS_CONTRACT_END = "<!-- END GENERATED BUTTONS MATURITY CONTRACT -->"
GALLERY_CONTRACT_BEGIN = "// BEGIN GENERATED BUTTONS MATURITY GALLERY COVERAGE"
GALLERY_CONTRACT_END = "// END GENERATED BUTTONS MATURITY GALLERY COVERAGE"

FOCUSED_MATURITY_TESTS = {
    "tst_button_contracts": "tests/widgets/buttons/tst_button_contracts.cpp",
    "tst_outlinedbutton_rendering": "tests/widgets/buttons/tst_outlinedbutton_rendering.cpp",
    "tst_elevatedbutton_shadow": "tests/widgets/buttons/tst_elevatedbutton_shadow.cpp",
    "tst_icon_fab_accessibility": "tests/widgets/buttons/tst_icon_fab_accessibility.cpp",
}

BUTTON_CONTRACT_TARGET = "tst_button_contracts"
BUTTON_CONTRACT_SOURCE = FOCUSED_MATURITY_TESTS[BUTTON_CONTRACT_TARGET]
BUTTON_CTEST_REGEX = (
    "tst_button_contracts|"
    "tst_outlinedbutton_rendering|"
    "tst_elevatedbutton_shadow|"
    "tst_icon_fab_accessibility|"
    "tst_.*button|"
    "tst_.*fab"
)

BUTTONS: dict[str, dict[str, Any]] = {
    "button.text": {
        "name": "Text Button",
        "publicHeader": "qtmaterial/widgets/buttons/qtmaterialtextbutton.h",
        "testTarget": "tst_textbutton",
        "galleryRoute": "/buttons/text",
        "rtl": 4,
        "referenceCandidate": False,
        "evidenceNotes": [
            "Public Text Button variant is tracked in the registry.",
            "Shared Buttons maturity contract covers keyboard, accessibility, disabled-state, RTL, and HiDPI smoke behavior.",
        ],
    },
    "button.filled": {
        "name": "Filled Button",
        "publicHeader": "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h",
        "testTarget": "tst_filledbutton",
        "galleryRoute": "/buttons/filled",
        "rtl": 4,
        "referenceCandidate": True,
        "evidenceNotes": [
            "Public Filled Button variant is tracked in the registry.",
            "Reference candidate for the Buttons family.",
            "Shared Buttons maturity contract covers keyboard, accessibility, disabled-state, RTL, and HiDPI smoke behavior.",
        ],
    },
    "button.filled-tonal": {
        "name": "Filled Tonal Button",
        "publicHeader": "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h",
        "testTarget": "tst_filledtonalbutton",
        "galleryRoute": "/buttons/filled-tonal",
        "rtl": 4,
        "referenceCandidate": False,
        "evidenceNotes": [
            "Public Filled Tonal Button variant is tracked in the registry.",
            "Shared Buttons maturity contract covers keyboard, accessibility, disabled-state, RTL, and HiDPI smoke behavior.",
        ],
    },
    "button.outlined": {
        "name": "Outlined Button",
        "publicHeader": "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h",
        "testTarget": "tst_outlinedbutton",
        "galleryRoute": "/buttons/outlined",
        "rtl": 4,
        "referenceCandidate": False,
        "evidenceNotes": [
            "Public Outlined Button variant is tracked in the registry.",
            "Shared Buttons maturity contract covers keyboard, accessibility, disabled-state, RTL, and HiDPI smoke behavior.",
            "Outlined-specific maturity requires stroke rendering and HiDPI validation to remain covered by variant tests or visual review.",
        ],
    },
    "button.elevated": {
        "name": "Elevated Button",
        "publicHeader": "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h",
        "testTarget": "tst_elevatedbutton",
        "galleryRoute": "/buttons/elevated",
        "rtl": 4,
        "referenceCandidate": False,
        "evidenceNotes": [
            "Public Elevated Button variant is tracked in the registry.",
            "Shared Buttons maturity contract covers keyboard, accessibility, disabled-state, RTL, and HiDPI smoke behavior.",
            "Elevated-specific maturity requires elevation and shadow rendering to remain covered by variant tests or visual review.",
        ],
    },
    "button.icon": {
        "name": "Icon Button",
        "publicHeader": "qtmaterial/widgets/buttons/qtmaterialiconbutton.h",
        "testTarget": "tst_iconbutton",
        "galleryRoute": "/buttons/icon",
        "rtl": "N/A",
        "referenceCandidate": False,
        "evidenceNotes": [
            "Public Icon Button variant is tracked in the registry.",
            "Icon-only accessible-name policy is documented and covered by the shared Buttons contract test.",
            "RTL is not applicable to the icon-only layout itself; geometry stability is still covered by the shared contract where relevant.",
        ],
    },
    "button.fab": {
        "name": "Floating Action Button",
        "publicHeader": "qtmaterial/widgets/buttons/qtmaterialfab.h",
        "testTarget": "tst_fab",
        "galleryRoute": "/buttons/fab",
        "rtl": "N/A",
        "referenceCandidate": False,
        "evidenceNotes": [
            "Public compact FAB variant is tracked in the registry.",
            "Icon-only accessible-name policy is documented and covered by the shared Buttons contract test.",
            "RTL is not applicable to the icon-only compact FAB layout itself.",
        ],
    },
    "button.extended-fab": {
        "name": "Extended Floating Action Button",
        "publicHeader": "qtmaterial/widgets/buttons/qtmaterialextendedfab.h",
        "testTarget": "tst_extendedfab",
        "galleryRoute": "/buttons/extended-fab",
        "rtl": 4,
        "referenceCandidate": False,
        "evidenceNotes": [
            "Public Extended FAB variant is tracked in the registry.",
            "Shared Buttons maturity contract covers keyboard, accessibility, disabled-state, RTL, and HiDPI smoke behavior.",
        ],
    },
}

AXES_TO_PROMOTE = [
    "api",
    "rendering",
    "states",
    "accessibility",
    "keyboard",
    "hidpi",
    "tests",
    "example",
    "docs",
]


class PromotionError(RuntimeError):
    pass


@dataclass(frozen=True)
class AuditResult:
    blockers: list[str]
    warnings: list[str]

    @property
    def ok(self) -> bool:
        return not self.blockers


def find_repo_root(start: Path) -> Path:
    current = start.resolve()
    for candidate in [current, *current.parents]:
        if (candidate / REGISTRY_RELATIVE_PATH).exists() and (candidate / TESTS_CMAKE_RELATIVE_PATH).exists():
            return candidate
    raise PromotionError(
        f"Could not find {REGISTRY_RELATIVE_PATH}. Run from the repository root or pass --repo."
    )


def load_registry(path: Path) -> list[dict[str, Any]]:
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        raise PromotionError(f"Invalid JSON in {path}: {exc}") from exc

    if not isinstance(data, list):
        raise PromotionError(f"Expected {path} to contain a JSON array")

    for index, item in enumerate(data):
        if not isinstance(item, dict):
            raise PromotionError(f"Expected registry item #{index} to be an object")

    return data


def dump_registry(data: list[dict[str, Any]]) -> str:
    return json.dumps(data, indent=2, ensure_ascii=False) + "\n"


def read_text_if_exists(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace") if path.exists() else ""


def has_all_markers(text: str, begin: str, end: str) -> bool:
    return begin in text and end in text and text.index(begin) < text.index(end)


def run_command(command: list[str], cwd: Path) -> tuple[bool, str]:
    completed = subprocess.run(
        command,
        cwd=str(cwd),
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        check=False,
    )
    return completed.returncode == 0, completed.stdout


def audit_repository(
    *,
    repo: Path,
    registry: list[dict[str, Any]],
    run_ctest: bool,
    build_dir: Path | None,
    run_generator_check: bool,
) -> AuditResult:
    blockers: list[str] = []
    warnings: list[str] = []

    by_id = {str(item.get("id", "")): item for item in registry}

    if len(by_id) != len(registry):
        blockers.append("component-registry.json contains duplicate or empty component ids")

    tests_cmake = read_text_if_exists(repo / TESTS_CMAKE_RELATIVE_PATH)
    buttons_doc = read_text_if_exists(repo / BUTTONS_DOC_RELATIVE_PATH)
    gallery_page = read_text_if_exists(repo / BUTTONS_GALLERY_RELATIVE_PATH)

    for button_id, spec in BUTTONS.items():
        item = by_id.get(button_id)
        if item is None:
            blockers.append(f"missing registry entry: {button_id}")
            continue

        if item.get("family") != "Buttons":
            blockers.append(f"{button_id}: family must be `Buttons`, got `{item.get('family')}`")

        if item.get("publicHeader") != spec["publicHeader"]:
            blockers.append(
                f"{button_id}: expected publicHeader `{spec['publicHeader']}`, got `{item.get('publicHeader')}`"
            )

        header_path = repo / "include" / str(spec["publicHeader"])
        if not header_path.exists():
            blockers.append(f"{button_id}: missing public header include/{spec['publicHeader']}")

        if item.get("testTarget") != spec["testTarget"]:
            blockers.append(
                f"{button_id}: expected testTarget `{spec['testTarget']}`, got `{item.get('testTarget')}`"
            )

        if str(spec["testTarget"]) not in tests_cmake:
            blockers.append(f"{button_id}: test target `{spec['testTarget']}` is not registered in tests/CMakeLists.txt")

        if item.get("docsPath") != str(BUTTONS_DOC_RELATIVE_PATH):
            warnings.append(
                f"{button_id}: docsPath is `{item.get('docsPath')}`, expected `{BUTTONS_DOC_RELATIVE_PATH}`"
            )

        if item.get("galleryRoute") != spec["galleryRoute"]:
            warnings.append(
                f"{button_id}: galleryRoute is `{item.get('galleryRoute')}`, expected `{spec['galleryRoute']}`"
            )

        axes = item.get("maturityAxes")
        if not isinstance(axes, dict):
            blockers.append(f"{button_id}: missing maturityAxes object")
            continue

        for axis in [*AXES_TO_PROMOTE, "rtl"]:
            if axis not in axes:
                blockers.append(f"{button_id}: missing maturityAxes.{axis}")

    for test_target, source_path in FOCUSED_MATURITY_TESTS.items():
        if test_target not in tests_cmake:
            blockers.append(f"missing focused Buttons maturity test target `{test_target}` in tests/CMakeLists.txt")
        if not (repo / source_path).exists():
            blockers.append(f"missing focused Buttons maturity test source `{source_path}`")

    if not has_all_markers(buttons_doc, DOCS_CONTRACT_BEGIN, DOCS_CONTRACT_END):
        blockers.append(
            f"missing Buttons maturity contract markers in {BUTTONS_DOC_RELATIVE_PATH}; run apply_buttons_docs_contract.py"
        )

    if not has_all_markers(gallery_page, GALLERY_CONTRACT_BEGIN, GALLERY_CONTRACT_END):
        blockers.append(
            f"missing Buttons maturity gallery markers in {BUTTONS_GALLERY_RELATIVE_PATH}; run apply_buttons_gallery_maturity.py"
        )

    generator_path = repo / GENERATOR_RELATIVE_PATH
    if not generator_path.exists():
        blockers.append(f"missing generator script {GENERATOR_RELATIVE_PATH}")

    if run_generator_check:
        ok, output = run_command([sys.executable, str(generator_path), "--check"], cwd=repo)
        if not ok:
            blockers.append("python scripts/generate_component_status.py --check failed")
            warnings.append(output.strip() or "generator check produced no output")

    if run_ctest:
        if build_dir is None:
            blockers.append("--run-ctest requires --build-dir")
        else:
            resolved_build_dir = build_dir if build_dir.is_absolute() else repo / build_dir
            if not resolved_build_dir.exists():
                blockers.append(f"build directory does not exist: {resolved_build_dir}")
            else:
                ok, output = run_command(
                    [
                        "ctest",
                        "--test-dir",
                        str(resolved_build_dir),
                        "-R",
                        BUTTON_CTEST_REGEX,
                        "--output-on-failure",
                    ],
                    cwd=repo,
                )
                if not ok:
                    blockers.append(f"ctest Buttons suite failed with regex `{BUTTON_CTEST_REGEX}`")
                    warnings.append(output.strip() or "ctest produced no output")

    return AuditResult(blockers=blockers, warnings=warnings)


def promote_registry(registry: list[dict[str, Any]]) -> list[dict[str, Any]]:
    promoted = copy.deepcopy(registry)
    by_id = {str(item.get("id", "")): item for item in promoted}
    today = date.today().isoformat()

    for button_id, spec in BUTTONS.items():
        item = by_id[button_id]
        item["maturity"] = "complete"
        item["maturityPolicy"] = "manual"
        item["releaseScope"] = True
        item["referenceCandidate"] = bool(spec["referenceCandidate"])

        axes = item.setdefault("maturityAxes", {})
        if not isinstance(axes, dict):
            axes = {}
            item["maturityAxes"] = axes

        for axis in AXES_TO_PROMOTE:
            axes[axis] = 4

        axes["rtl"] = spec["rtl"]
        axes["lastReviewed"] = today
        axes["evidence"] = {
            "api": [
                f"Public header exists: include/{spec['publicHeader']}.",
                f"Registry declares widget type `{item.get('widgetType')}` and test target `{spec['testTarget']}`.",
            ],
            "rendering": [
                "Buttons gallery includes enabled, disabled, icon/text, long-label, and RTL maturity coverage.",
                "Variant-specific rendering expectations are documented in docs/public-api/buttons.md.",
                "Focused rendering tests cover OutlinedButton stroke behavior and ElevatedButton shadow behavior.",
            ],
            "states": [
                "Shared Buttons contract covers enabled, disabled, keyboard-activated, and non-activated disabled states.",
            ],
            "accessibility": [
                "Buttons maturity contract documents accessible name, role, and icon-only naming policy.",
                "Shared Buttons contract validates accessible-name behavior and icon-only policy.",
                "Focused IconButton/FAB accessibility tests verify strict icon-only naming behavior.",
            ],
            "keyboard": [
                "Shared Buttons contract validates focus, Space activation, Return activation, and disabled non-activation.",
            ],
            "hidpi": [
                "Shared Buttons contract includes a DPR 2.0 rendering smoke test.",
                "Focused OutlinedButton and ElevatedButton tests cover DPR 1.0, 1.5, and 2.0 rendering smoke checks.",
            ],
            "rtl": [
                "Buttons gallery includes RTL maturity preview for text-bearing variants."
                if spec["rtl"] != "N/A"
                else "RTL is not applicable to the icon-only layout; geometry remains covered by common rendering checks.",
            ],
            "tests": [
                f"Variant test target exists: {spec['testTarget']}.",
                f"Shared contract test target exists: {BUTTON_CONTRACT_TARGET}.",
                "Focused maturity tests exist for OutlinedButton rendering, ElevatedButton shadow, and IconButton/FAB accessibility.",
            ],
            "example": [
                "Buttons gallery contains a marked maturity coverage block.",
            ],
            "docs": [
                "docs/public-api/buttons.md contains a marked Buttons maturity contract block.",
            ],
            "notes": list(spec["evidenceNotes"]),
        }
        axes["gaps"] = []
        axes["nextActions"] = [
            "Maintain complete status by keeping the registry, shared contract test, gallery coverage, and documentation contract in sync.",
        ]

    return promoted


def unified_diff(old: str, new: str, from_file: str, to_file: str) -> str:
    return "\n".join(
        difflib.unified_diff(
            old.splitlines(),
            new.splitlines(),
            fromfile=from_file,
            tofile=to_file,
            lineterm="",
        )
    )


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Promote Buttons maturity to complete after auditing prerequisites.")

    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--dry-run", action="store_true", help="show audit and diff without writing; default")
    mode.add_argument("--apply", action="store_true", help="write the promoted registry when audit passes")

    parser.add_argument("--repo", type=Path, default=None, help="repository root; default: auto-detect")
    parser.add_argument("--build-dir", type=Path, default=None, help="build directory used with --run-ctest")
    parser.add_argument("--run-ctest", action="store_true", help="run the focused Buttons ctest suite before promotion")
    parser.add_argument("--run-generator-check", action="store_true", help="run scripts/generate_component_status.py --check before promotion")
    parser.add_argument("--force", action="store_true", help="allow promotion even when blockers are present; not recommended")
    parser.add_argument("--fail-on-blockers", action="store_true", help="return non-zero on dry-run when blockers exist")

    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    dry_run = not args.apply

    try:
        repo = args.repo.resolve() if args.repo else find_repo_root(Path.cwd())
        registry_path = repo / REGISTRY_RELATIVE_PATH
        registry = load_registry(registry_path)

        audit = audit_repository(
            repo=repo,
            registry=registry,
            run_ctest=args.run_ctest,
            build_dir=args.build_dir,
            run_generator_check=args.run_generator_check,
        )

        print("Buttons maturity promotion audit")
        print(f"repo:     {repo}")
        print(f"registry: {registry_path}")
        print(f"mode:     {'dry-run' if dry_run else 'apply'}")
        if args.run_ctest:
            print(f"ctest:    enabled, build-dir={args.build_dir}")
        print()

        if audit.blockers:
            print("Blockers:")
            for blocker in audit.blockers:
                print(f" - {blocker}")
            print()
        else:
            print("Blockers: none")
            print()

        if audit.warnings:
            print("Warnings / diagnostic output:")
            for warning in audit.warnings:
                print(" - " + warning.replace("\n", "\n   "))
            print()

        if audit.blockers and not args.force:
            print("Promotion refused. Resolve blockers or pass --force explicitly.")
            return 1 if args.fail_on_blockers or args.apply else 0

        promoted = promote_registry(registry)
        current_text = registry_path.read_text(encoding="utf-8")
        promoted_text = dump_registry(promoted)

        if current_text == promoted_text:
            print("No registry changes needed; Buttons already appear promoted.")
            return 0

        if dry_run:
            print("Promotion diff:")
            print(unified_diff(
                current_text,
                promoted_text,
                str(REGISTRY_RELATIVE_PATH) + " current",
                str(REGISTRY_RELATIVE_PATH) + " promoted",
            ))
            print()
            if audit.blockers and args.force:
                print("Dry-run generated with --force despite blockers. Do not apply until blockers are intentional.")
            else:
                print("Dry-run only. Re-run with --apply to write the promoted registry.")
            return 0

        registry_path.write_text(promoted_text, encoding="utf-8")
        print(f"Wrote {REGISTRY_RELATIVE_PATH}")
        print()
        print("Recommended next commands:")
        print("  python scripts/generate_component_status.py")
        print("  python scripts/generate_component_status.py --check --strict")
        if not args.run_ctest:
            print(f"  ctest --test-dir <build-dir> -R \"{BUTTON_CTEST_REGEX}\" --output-on-failure")
        return 0

    except PromotionError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
