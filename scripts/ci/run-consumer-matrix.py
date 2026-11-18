#!/usr/bin/env python3
from __future__ import annotations

import argparse
import hashlib
import hmac
import json
import os
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[2]
MATRIX = ROOT / "tests" / "consumers" / "consumer-matrix.json"

def run(command: list[str], *, expect_failure: bool = False,
        env: dict[str, str] | None = None, cwd: Path | None = None,
        timeout: int | None = None) -> None:
    print("+", " ".join(command), flush=True)
    completed = subprocess.run(command, check=False, env=env, cwd=cwd, timeout=timeout)
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
        "QTMATERIAL3_BUILD_DESIGNER_PLUGIN": "OFF",
        "QTMATERIAL3_BUILD_PYTHON_BINDINGS": "OFF",
        "QTMATERIAL3_BUILD_VISUAL_STUDIO": "OFF",
        "QTMATERIAL3_INSTALL": "ON" if install_enabled else "OFF",
        "QTMATERIAL3_USE_MCU": "OFF",
    }

def verify_archive_checksum(archive: Path) -> None:
    sidecar = archive.with_name(archive.name + ".sha256")
    fields = sidecar.read_text(encoding="utf-8").split()
    if not fields or len(fields[0]) != 64:
        raise RuntimeError(f"invalid SHA256 sidecar: {sidecar}")
    digest = hashlib.sha256()
    with archive.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    if not hmac.compare_digest(digest.hexdigest(), fields[0].lower()):
        raise RuntimeError(f"SHA256 mismatch: {archive}")


def package_config(prefix: Path) -> Path:
    configs = list(prefix.rglob("QtMaterial3WidgetsConfig.cmake"))
    if len(configs) != 1:
        raise RuntimeError(f"expected exactly one package config below {prefix}")
    return configs[0]


def extracted_prefix(extraction: Path) -> Path:
    config = package_config(extraction)
    for candidate in config.parents:
        if candidate != extraction and extraction not in candidate.parents:
            break
        if (candidate / "include" / "qtmaterial").is_dir():
            return candidate
    raise RuntimeError(f"package has no include/qtmaterial prefix: {config}")


def check_installed_consumer(prefix: Path, scenario_root: Path, *,
                             generator: str | None, config: str,
                             invalid: bool = False) -> None:
    run([
        sys.executable, str(ROOT / "tools" / "check_api_surface.py"),
        "--root", str(ROOT), "--scope", "installed", "--prefix", str(prefix),
    ])
    # Copy the consumer outside the source layout. It must only use installed
    # headers and imported CMake targets, never sibling implementation sources.
    sources = scenario_root / "downstream"
    kind = "invalid-component" if invalid else "installed"
    shutil.copytree(ROOT / "tests" / "consumers" / kind, sources / kind)
    shutil.copytree(ROOT / "tests" / "consumers" / "common", sources / "common")
    consumer = scenario_root / "consumer"
    config_file = package_config(prefix)
    configure(sources / kind, consumer, generator=generator, definitions={
        "CMAKE_BUILD_TYPE": config,
        "CMAKE_PREFIX_PATH": str(prefix),
        "QtMaterial3Widgets_DIR": str(config_file.parent),
        "CMAKE_FIND_USE_PACKAGE_REGISTRY": "OFF",
        "CMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY": "OFF",
    }, expect_failure=invalid)
    if invalid:
        return
    build(consumer, config)
    exe_name = "qtm3-installed-consumer.exe" if os.name == "nt" else "qtm3-installed-consumer"
    candidates = [p for p in consumer.rglob(exe_name) if p.is_file()]
    if len(candidates) != 1:
        raise RuntimeError(f"expected one installed consumer executable: {candidates}")
    env = os.environ.copy()
    env["QT_QPA_PLATFORM"] = "offscreen"
    # Make the installed application runtime search path explicit. Qt itself
    # remains a separately installed SDK/runtime dependency.
    libdir = config_file.parent.parent.parent
    runtime_dirs = [str(prefix / "bin"), str(libdir)]
    variable = ("PATH" if os.name == "nt" else
                "DYLD_LIBRARY_PATH" if sys.platform == "darwin" else "LD_LIBRARY_PATH")
    env[variable] = os.pathsep.join(runtime_dirs + ([env[variable]] if env.get(variable) else []))
    run([str(candidates[0])], env=env, cwd=consumer, timeout=60)


def execute_archive(archive: Path, *, build_root: Path,
                    generator: str | None, config: str) -> None:
    archive = archive.resolve()
    verify_archive_checksum(archive)
    workspace = build_root / ("archive-" + archive.name)
    if workspace.exists():
        shutil.rmtree(workspace)
    extraction = workspace / "extracted with spaces"
    extraction.mkdir(parents=True)
    # Only consume trusted, locally generated CPack binary archives.
    run(["cmake", "-E", "tar", "xf", str(archive)], cwd=extraction)
    prefix = extracted_prefix(extraction)
    relocated = workspace / "relocated SDK"
    shutil.move(str(prefix), str(relocated))
    check_installed_consumer(relocated, workspace, generator=generator, config=config)


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

        relocated = scenario_root / "relocated SDK"
        shutil.move(str(prefix), str(relocated))
        check_installed_consumer(relocated, scenario_root,
                                 generator=generator, config=config,
                                 invalid=(kind == "invalid_component"))
        return

    raise RuntimeError(f"unsupported scenario kind: {kind}")

def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--profile", default="ci")
    parser.add_argument("--scenario", action="append")
    parser.add_argument("--archive", action="append", type=Path,
                        help="Verify and consume a trusted CPack binary archive (repeatable)")
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

    args.build_root = args.build_root.resolve()
    if args.archive:
        if args.scenario or args.producer_build_dir or args.list:
            parser.error("--archive cannot be combined with scenario, producer or list options")
        for archive in args.archive:
            try:
                execute_archive(archive, build_root=args.build_root,
                                generator=args.generator or None, config=args.config)
            except (OSError, RuntimeError, subprocess.SubprocessError) as exc:
                print(f"archive validation failed: {exc}", file=sys.stderr)
                return 1
        return 0

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
