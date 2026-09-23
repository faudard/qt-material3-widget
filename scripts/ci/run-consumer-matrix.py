#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import os
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[2]
MATRIX = ROOT / "tests" / "consumers" / "consumer-matrix.json"

def run(command: list[str], *, expect_failure: bool = False) -> None:
    print("+", " ".join(command), flush=True)
    completed = subprocess.run(command, check=False)
    ok = completed.returncode != 0 if expect_failure else completed.returncode == 0
    if not ok:
        expectation = "failure" if expect_failure else "success"
        raise RuntimeError(
            f"command returned {completed.returncode}; expected {expectation}: {' '.join(command)}"
        )

def configure(source: Path, build: Path, *, generator: str | None,
              definitions: dict[str, str], expect_failure: bool = False) -> None:
    command = ["cmake", "-S", str(source), "-B", str(build)]
    if generator:
        command += ["-G", generator]
    command.extend(f"-D{k}={v}" for k, v in definitions.items())
    run(command, expect_failure=expect_failure)

def build(build_dir: Path, config: str) -> None:
    run(["cmake", "--build", str(build_dir), "--config", config, "--parallel"])

def install(build_dir: Path, prefix: Path, config: str) -> None:
    run(["cmake", "--install", str(build_dir), "--config", config, "--prefix", str(prefix)])

def producer_defs(linkage: str, install_enabled: bool) -> dict[str, str]:
    return {
        "CMAKE_BUILD_TYPE": "Release",
        "BUILD_SHARED_LIBS": "ON" if linkage == "shared" else "OFF",
        "QTMATERIAL3_BUILD_TESTS": "OFF",
        "QTMATERIAL3_BUILD_EXAMPLES": "OFF",
        "QTMATERIAL3_BUILD_BENCHMARKS": "OFF",
        "QTMATERIAL3_INSTALL": "ON" if install_enabled else "OFF",
        "QTMATERIAL3_USE_MCU": "OFF",
    }

def execute(name: str, scenario: dict[str, Any], *, build_root: Path,
            generator: str | None, config: str,
            producer_build_dir: Path | None = None) -> None:
    kind = scenario["kind"]
    linkage = scenario["linkage"]
    scenario_root = build_root / name
    if scenario_root.exists():
        shutil.rmtree(scenario_root)
    scenario_root.mkdir(parents=True)

    if kind == "source":
        b = scenario_root / "build"
        configure(ROOT, b, generator=generator,
                  definitions=producer_defs(linkage, False))
        build(b, config)
        return

    if kind in {"add_subdirectory", "fetchcontent"}:
        src = ROOT / "tests" / "consumers" / (
            "add-subdirectory" if kind == "add_subdirectory" else "fetchcontent"
        )
        b = scenario_root / "build"
        configure(src, b, generator=generator, definitions={
            "CMAKE_BUILD_TYPE": "Release",
            "BUILD_SHARED_LIBS": "ON" if linkage == "shared" else "OFF",
            "QTMATERIAL3_SOURCE_DIR": str(ROOT),
        })
        build(b, config)
        return

    if kind in {"install", "invalid_component"}:
        prefix = scenario_root / "prefix"
        if producer_build_dir is None:
            producer = scenario_root / "producer"
            configure(ROOT, producer, generator=generator,
                      definitions=producer_defs(linkage, True))
            build(producer, config)
        else:
            producer = producer_build_dir
            if not producer.is_dir():
                raise RuntimeError(f"producer build directory does not exist: {producer}")
        install(producer, prefix, config)

        run([
            sys.executable,
            str(ROOT / "tools" / "check_api_surface.py"),
            "--root", str(ROOT),
            "--scope", "installed",
            "--prefix", str(prefix),
        ])

        src = ROOT / "tests" / "consumers" / (
            "installed" if kind == "install" else "invalid-component"
        )
        consumer = scenario_root / "consumer"
        configure(src, consumer, generator=generator, definitions={
            "CMAKE_BUILD_TYPE": "Release",
            "CMAKE_PREFIX_PATH": str(prefix),
        }, expect_failure=(kind == "invalid_component"))
        if kind == "install":
            build(consumer, config)
        return

    raise RuntimeError(f"unsupported scenario kind: {kind}")

def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--profile", default="ci")
    parser.add_argument("--scenario", action="append")
    parser.add_argument("--build-root", type=Path, default=ROOT / "build" / "consumer-matrix")
    parser.add_argument("--generator", default=os.environ.get("QTM3_GENERATOR", "Ninja"))
    parser.add_argument(
        "--producer-build-dir",
        type=Path,
        help="Reuse an already configured and built producer for install scenarios",
    )
    parser.add_argument("--config", default="Release")
    parser.add_argument("--list", action="store_true")
    args = parser.parse_args(argv)

    data = json.loads(MATRIX.read_text(encoding="utf-8"))
    scenarios = data["scenarios"]

    if args.list:
        print("\n".join(scenarios))
        return 0

    selected = args.scenario or data["profiles"].get(args.profile)
    if not selected:
        print(f"unknown or empty profile: {args.profile}", file=sys.stderr)
        return 2

    failures: list[tuple[str, str]] = []
    for name in selected:
        print(f"\n=== consumer scenario: {name} ===", flush=True)
        try:
            execute(
                name,
                scenarios[name],
                build_root=args.build_root,
                generator=args.generator or None,
                config=args.config,
                producer_build_dir=args.producer_build_dir,
            )
        except Exception as exc:
            failures.append((name, str(exc)))

    print("\nConsumer matrix summary")
    print("=======================")
    for name in selected:
        message = next((m for n, m in failures if n == name), None)
        print(("FAIL" if message else "PASS"), name,
              f"- {message}" if message else "")
    return 1 if failures else 0

if __name__ == "__main__":
    raise SystemExit(main())
