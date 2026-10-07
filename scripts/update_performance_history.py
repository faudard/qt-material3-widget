#!/usr/bin/env python3
"""Maintain a bounded per-commit history for Desktop Scale 2.0 results."""

from __future__ import annotations

import argparse
import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


def load_json(path: Path) -> dict[str, Any]:
    with path.open("r", encoding="utf-8") as handle:
        payload = json.load(handle)
    if not isinstance(payload, dict):
        raise ValueError(f"{path} must contain a JSON object")
    return payload


def scenario_map(payload: dict[str, Any]) -> dict[str, dict[str, Any]]:
    scenarios = payload.get("scenarios", [])
    if not isinstance(scenarios, list):
        raise ValueError("current results must contain a scenarios array")
    result: dict[str, dict[str, Any]] = {}
    for scenario in scenarios:
        if not isinstance(scenario, dict):
            continue
        name = scenario.get("name")
        if isinstance(name, str) and name:
            result[name] = scenario
    if not result:
        raise ValueError("current results contain no named scenarios")
    return result


def previous_entry(entries: list[dict[str, Any]], commit: str) -> dict[str, Any] | None:
    for entry in reversed(entries):
        if entry.get("commit") != commit:
            return entry
    return None


def percent_delta(current: float | int | None, previous: float | int | None) -> str:
    if current is None or previous is None:
        return "n/a"
    try:
        current_value = float(current)
        previous_value = float(previous)
    except (TypeError, ValueError):
        return "n/a"
    if previous_value == 0:
        return "n/a"
    return f"{((current_value - previous_value) / previous_value) * 100:+.1f}%"


def metric(value: Any, suffix: str = "") -> str:
    if value is None:
        return "n/a"
    if isinstance(value, float):
        return f"{value:.2f}{suffix}"
    return f"{value}{suffix}"


def build_summary(
    current: dict[str, dict[str, Any]],
    previous: dict[str, Any] | None,
    commit: str,
) -> str:
    previous_scenarios: dict[str, dict[str, Any]] = {}
    if previous:
        raw = previous.get("scenarios", {})
        if isinstance(raw, dict):
            previous_scenarios = {
                name: value
                for name, value in raw.items()
                if isinstance(name, str) and isinstance(value, dict)
            }

    lines = [
        "## Desktop Scale 2.0",
        "",
        f"Commit: `{commit[:12]}`",
        "",
        "| Scenario | p95 | p99 | CPU | RSS growth | p95 vs previous |",
        "| --- | ---: | ---: | ---: | ---: | ---: |",
    ]

    for name in sorted(current):
        scenario = current[name]
        previous_scenario = previous_scenarios.get(name, {})
        lines.append(
            "| {name} | {p95} | {p99} | {cpu} | {rss} | {delta} |".format(
                name=name,
                p95=metric(scenario.get("p95Ms"), " ms"),
                p99=metric(scenario.get("p99Ms"), " ms"),
                cpu=metric(scenario.get("cpuMs"), " ms"),
                rss=metric(scenario.get("rssGrowthMiB"), " MiB"),
                delta=percent_delta(
                    scenario.get("p95Ms"),
                    previous_scenario.get("p95Ms"),
                ),
            )
        )

    if previous:
        previous_commit = str(previous.get("commit", ""))
        lines.extend(
            [
                "",
                f"Compared with previous recorded commit `{previous_commit[:12]}`.",
            ]
        )
    else:
        lines.extend(["", "No previous Desktop Scale 2.0 history entry was available."])

    return "\n".join(lines) + "\n"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--current", type=Path, required=True)
    parser.add_argument("--previous", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--summary", type=Path, required=True)
    parser.add_argument("--commit", required=True)
    parser.add_argument("--ref", default="")
    parser.add_argument("--event", default="")
    parser.add_argument("--max-entries", type=int, default=60)
    args = parser.parse_args()

    if args.max_entries <= 0:
        parser.error("--max-entries must be positive")

    current_payload = load_json(args.current)
    current_scenarios = scenario_map(current_payload)

    history: dict[str, Any] = {"version": 1, "suite": "desktop-scale-2", "entries": []}
    if args.previous and args.previous.is_file():
        previous_payload = load_json(args.previous)
        if previous_payload.get("suite") == "desktop-scale-2":
            history = previous_payload

    raw_entries = history.get("entries", [])
    entries = [entry for entry in raw_entries if isinstance(entry, dict)]
    before = previous_entry(entries, args.commit)

    entry = {
        "commit": args.commit,
        "ref": args.ref,
        "event": args.event,
        "recordedAt": datetime.now(timezone.utc).isoformat(),
        "scenarios": current_scenarios,
    }

    entries = [item for item in entries if item.get("commit") != args.commit]
    entries.append(entry)
    entries = entries[-args.max_entries :]

    history = {
        "version": 1,
        "suite": "desktop-scale-2",
        "entries": entries,
    }

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(
        json.dumps(history, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )

    args.summary.parent.mkdir(parents=True, exist_ok=True)
    args.summary.write_text(
        build_summary(current_scenarios, before, args.commit),
        encoding="utf-8",
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
