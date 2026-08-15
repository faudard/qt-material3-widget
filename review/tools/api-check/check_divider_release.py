#!/usr/bin/env python3
from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HEADER = ROOT / "include/qtmaterial/widgets/data/qtmaterialdivider.h"
SOURCE = ROOT / "src/widgets/data/qtmaterialdivider.cpp"
TEST = ROOT / "tests/widgets/data/tst_divider.cpp"

REQUIRED_HEADER_TOKENS = [
    "Q_PROPERTY(bool decorative",
    "Q_PROPERTY(QString accessibilityLabel",
    "bool isDecorative() const noexcept;",
    "void setDecorative(bool decorative);",
    "QString accessibilityLabel() const;",
    "void setAccessibilityLabel(const QString &label);",
    "QString accessibilitySummary() const;",
    "QRect lineRect() const;",
    "void decorativeChanged(bool decorative);",
    "void accessibilitySummaryChanged(const QString &summary);",
]

REQUIRED_SOURCE_TOKENS = [
    "QtMaterialDivider::lineRect() const",
    "layoutDirection() == Qt::RightToLeft",
    "QtMaterialDivider::syncAccessibility()",
    "setAccessibleName",
    "setAccessibleDescription",
]

REQUIRED_TEST_TOKENS = [
    "lineRectHorizontalRtl",
    "accessibilityDecorativeByDefault",
    "accessibilityOptInSummary",
]


def require(path: Path, tokens: list[str]) -> None:
    if not path.exists():
        raise SystemExit(f"missing file: {path}")

    text = path.read_text(encoding="utf-8")
    missing = [token for token in tokens if token not in text]
    if missing:
        raise SystemExit(f"{path}: missing tokens: {missing}")


def main() -> int:
    require(HEADER, REQUIRED_HEADER_TOKENS)
    require(SOURCE, REQUIRED_SOURCE_TOKENS)
    require(TEST, REQUIRED_TEST_TOKENS)
    print("Divider release API check passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
