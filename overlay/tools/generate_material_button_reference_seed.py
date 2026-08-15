#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Sequence

COMPONENTS = (
    "button.text",
    "button.filled",
    "button.filled-tonal",
    "button.outlined",
    "button.elevated",
)
STATES = ("enabled", "disabled", "hovered", "focused", "pressed")
THEMES = ("light", "dark")


def generate() -> dict:
    cases = [
        {
            "id": f"classic.buttons.{component}.{state}.{theme}",
            "componentId": component,
            "state": state,
            "theme": theme,
            "classification": "MUST_MATCH",
            "assertions": [],
        }
        for component in COMPONENTS
        for state in STATES
        for theme in THEMES
    ]
    return {
        "schemaVersion": 1,
        "id": "classic.buttons.poc",
        "profileId": "m3-classic.compose-material3-1.4.0",
        "status": "seeded",
        "family": "Buttons",
        "anchorComponentId": "button.filled",
        "sourceRefs": [
            "m3-buttons",
            "m3-design-tokens",
            "m3-states",
            "androidx-material3-1.4.0",
        ],
        "caseDimensions": {
            "components": list(COMPONENTS),
            "states": list(STATES),
            "themes": list(THEMES),
        },
        "cases": cases,
    }


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args(argv)

    expected = json.dumps(generate(), indent=2) + "\n"
    if args.check:
        if not args.output.is_file():
            print("Missing generated suite:", args.output)
            return 1
        if args.output.read_text(encoding="utf-8") != expected:
            print("Generated button reference suite is stale:", args.output)
            return 1
        print("Button reference seed OK: 5 x 5 x 2 = 50 deterministic cases")
        return 0

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(expected, encoding="utf-8")
    print("Generated", args.output, "with 50 cases")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
