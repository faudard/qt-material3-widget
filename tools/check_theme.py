#!/usr/bin/env python3
"""Unified entry point for Theme contract checks."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Callable, Sequence

TOOLS = Path(__file__).resolve().parent
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))

from theme_contracts import io, model, runtime, targets, tokens

ROOT = Path(__file__).resolve().parents[1]

VALIDATORS: dict[str, Callable[[Path], list[str]]] = {
    "model": model.validate_contract,
    "io": io.validate,
    "runtime": runtime.validate,
    "targets": targets.validate_contract,
    "tokens": tokens.validate,
}


def normalize_scopes(scopes: Sequence[str] | None) -> tuple[str, ...]:
    if not scopes or "all" in scopes:
        return tuple(VALIDATORS)
    return tuple(dict.fromkeys(scopes))


def validate(
    root: Path = ROOT,
    scopes: Sequence[str] | None = None,
) -> dict[str, list[str]]:
    selected = normalize_scopes(scopes)
    return {
        scope: VALIDATORS[scope](root.resolve())
        for scope in selected
    }


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument(
        "--scope",
        action="append",
        choices=("all", *VALIDATORS.keys()),
        default=[],
    )
    args = parser.parse_args(argv)

    results = validate(args.root, args.scope)
    failed = False
    for scope, errors in results.items():
        if errors:
            failed = True
            print(f"Theme {scope} contract FAILED:")
            for error in errors:
                print(" -", error)
        else:
            print(f"Theme {scope} contract OK")
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
