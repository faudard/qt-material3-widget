#!/usr/bin/env python3
"""Validate the permanent Interaction & Effects release contract."""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace")


def project_version(text: str) -> str | None:
    match = re.search(
        r"project\s*\(\s*qt-material3-widgets\s+VERSION\s+([0-9]+\.[0-9]+\.[0-9]+)",
        text,
        re.IGNORECASE | re.MULTILINE,
    )
    return match.group(1) if match else None


def require_markers(
    root: Path,
    relative: str,
    markers: tuple[str, ...],
    errors: list[str],
) -> None:
    path = root / relative
    if not path.is_file():
        errors.append(f"missing interaction release artifact: {relative}")
        return
    content = read(path)
    for marker in markers:
        if marker not in content:
            errors.append(f"{relative} missing contract marker: {marker}")


def validate(root: Path, expected_version: str | None = None) -> list[str]:
    errors: list[str] = []

    version = project_version(read(root / "CMakeLists.txt"))
    if not version:
        return ["cannot resolve project version"]
    if expected_version and version != expected_version:
        errors.append(f"project version is {version}, expected {expected_version}")

    contracts: dict[str, tuple[str, ...]] = {
        "include/qtmaterial/foundation/qtmaterialinteractionstate.h": (
            "isHovered()",
            "isFocused()",
            "isPressed()",
            "isChecked()",
            "isSelected()",
            "isIndeterminate()",
            "isDragged()",
            "hasError()",
            "isReadOnly()",
            "isBusy()",
            "isExpanded()",
            "isInteractive()",
            "hasTransientState()",
        ),
        "include/qtmaterial/effects/qtmaterialripplecontroller.h": (
            "enum class BoundsMode",
            "enum class OriginMode",
            "setReducedMotion",
            "setEnabled",
        ),
        "include/qtmaterial/effects/qtmaterialstatelayerpainter.h": (
            "opacityForState",
            "paintPathForState",
        ),
        "include/qtmaterial/effects/qtmaterialfocusindicator.h": (
            "shouldShow",
            "FocusRingTokens",
        ),
        "include/qtmaterial/effects/qtmaterialtransitioncontroller.h": (
            "setReducedMotion",
            "isRunning",
            "startTo",
            "finish()",
        ),
        "include/qtmaterial/effects/qtmaterialshadowcache.h": (
            "devicePixelRatio",
        ),
        "include/qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h": (
            "focusNextPrevChild",
            "focusableSheetChildren",
            "restoreFocusOnClose",
        ),
        "src/widgets/buttons/qtmaterialtextbutton.cpp": (
            "focusReason()",
            "accessibility().reducedMotion",
        ),
        "src/widgets/selection/qtmaterialcheckbox.cpp": (
            "setIndeterminate",
            "accessibility().reducedMotion",
            "focusReason()",
        ),
        "tests/core/tst_interactionstate_contracts.cpp": (
            "readOnlySuppressesPressAndDrag",
            "persistentStatesSurviveDisabled",
        ),
        "tests/effects/tst_ripplecontroller.cpp": (
            "disabledSuppressesRipple",
            "reducedMotionSuppressesAnimatedRipple",
            "boundedAndUnboundedPaint",
        ),
        "tests/effects/tst_statelayerpainter.cpp": (
            "interactionPriority",
            "policyCanSuppressPointerFeedback",
        ),
        "tests/effects/tst_focusindicator.cpp": (
            "keyboardVisibilityPolicy",
            "tokenizedRingPaints",
        ),
        "tests/effects/tst_transitioncontroller.cpp": (
            "reducedMotionCompletesImmediately",
            "retargetingEndsAtLatestTarget",
            "themeMotionTokenAppliesReducedMotion",
        ),
        "tests/widgets/surfaces/tst_bottomsheet.cpp": (
            "modalTabFocusStaysInsideSheet",
            "initialFocusAndRestoreFocus",
        ),
        "tests/widgets/surfaces/tst_dialog.cpp": (
            "tabFocusStaysInsideDialog",
            "openFocusesInitialWidgetAndRestoresPreviousFocusOnClose",
        ),
        "benchmarks/effects/benchmark_shadow_cache_pipeline.cpp": (
            "dpr-1x",
            "dpr-2x",
            "dprProducesDistinctCacheIdentity",
        ),
        "benchmarks/effects/benchmark_ripple.cpp": (
            "animated",
            "reduced-motion",
        ),
        "docs/public-api/interaction-effects.md": (
            "Interaction state",
            "Reduced motion",
            "Transient focus",
            "Device-pixel-ratio",
        ),
    }

    for relative, markers in contracts.items():
        require_markers(root, relative, markers, errors)

    theme_cmake = read(root / "src/theme/CMakeLists.txt")
    serializer = read(root / "src/theme/qtmaterialthemeserializer.cpp")
    if 'QTMATERIAL3_VERSION_STRING="${PROJECT_VERSION}"' not in theme_cmake:
        errors.append("ThemeIO does not source serializer version from PROJECT_VERSION")
    if "QTMATERIAL3_VERSION_STRING" not in serializer:
        errors.append("theme serializer does not use the project version definition")

    return errors


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", default=".")
    parser.add_argument("--expected-version")
    args = parser.parse_args()

    errors = validate(Path(args.root).resolve(), args.expected_version)
    if errors:
        print("Interaction release contract FAILED:")
        for error in errors:
            print(f"  - {error}")
        return 1

    print("Interaction release contract OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())
