#!/usr/bin/env python3
"""Package existing certification executables from a static QtMaterial3 build."""
from __future__ import annotations

import argparse
import json
import os
import platform
import plistlib
import shutil
import subprocess
import sys
from pathlib import Path

DEMOS = {
    "qtmaterial3_accessibility_certification": "accessibility-certification",
    "qtmaterial3_catalogue_certification": "material3-catalogue-certification",
    "qtmaterial3_adaptive_desktop_certification": "adaptive-desktop-certification",
    "qtmaterial3_gallery": "gallery",
}


def find_binary(build, name, directory, system):
    suffix = ".exe" if system == "Windows" else ""
    candidates = [build / "bin" / (name + suffix),
                  build / "examples" / directory / (name + suffix),
                  build / "examples" / directory / "Release" / (name + suffix),
                  build / "examples" / directory / (name + ".app") / "Contents" / "MacOS" / name]
    for candidate in candidates:
        if candidate.is_file():
            return candidate
    raise FileNotFoundError(f"Missing built demo {name}: {candidates}")


def execute(command):
    subprocess.run([str(value) for value in command], check=True)


def smoke(binary, output):
    # Startup smoke only, no actual native screen-reader session is claimed.
    env = {k: v for k, v in os.environ.items()
           if k not in ("QT_PLUGIN_PATH", "QT_QPA_PLATFORM_PLUGIN_PATH",
                        "LD_LIBRARY_PATH", "DYLD_LIBRARY_PATH", "DYLD_FRAMEWORK_PATH")}
    env["QT_QPA_PLATFORM"] = "offscreen"
    with subprocess.Popen([str(binary)], cwd=output, env=env,
                          stdout=subprocess.PIPE, stderr=subprocess.STDOUT) as proc:
        try:
            log, _ = proc.communicate(timeout=4)
            raise RuntimeError(f"{binary.name} exited at startup ({proc.returncode}): {log!r}")
        except subprocess.TimeoutExpired:
            proc.terminate()
            try:
                proc.communicate(timeout=5)
            except subprocess.TimeoutExpired:
                proc.kill()
                proc.communicate()


def package(build, output, qt_version, source_commit, system=None, *, smoke_test=True):
    system = system or platform.system()
    if output.exists() and any(output.iterdir()):
        raise ValueError(f"Output directory is not empty: {output}")
    output.mkdir(parents=True, exist_ok=True)
    binaries = []
    for name, directory in DEMOS.items():
        source = find_binary(build, name, directory, system)
        if system == "Darwin":
            app = output / (name + ".app")
            contents = app / "Contents"
            dest = contents / "MacOS" / name
            dest.parent.mkdir(parents=True)
            shutil.copy2(source, dest)
            with (contents / "Info.plist").open("wb") as stream:
                plistlib.dump({"CFBundleExecutable": name,
                               "CFBundleIdentifier": "org.qtmaterial3." + name,
                               "CFBundleName": name,
                               "CFBundlePackageType": "APPL",
                               "CFBundleVersion": "1",
                               "CFBundleShortVersionString": "1.22"}, stream)
            execute(["macdeployqt", app, "-always-overwrite"])
            execute(["codesign", "--force", "--deep", "--sign", "-", app])
        else:
            dest = output / source.name
            shutil.copy2(source, dest)
            if system == "Windows":
                execute(["windeployqt", "--release", "--no-translations", "--compiler-runtime", dest])
        binaries.append(dest)
    if system == "Windows":
        if not (output / "platforms" / "qwindows.dll").exists():
            raise RuntimeError("Windows platform plugin missing")
    elif system == "Darwin":
        for binary in binaries:
            if not (binary.parent.parent / "PlugIns" / "platforms" / "libqcocoa.dylib").exists():
                raise RuntimeError(f"Cocoa plugin missing for {binary}")
    elif system != "Linux":
        raise ValueError(f"Unsupported packaging system {system}")

    metadata = {"schemaVersion": 1, "sourceCommit": source_commit,
                "qtVersion": qt_version, "os": system,
                "architecture": platform.machine(), "executables": list(DEMOS)}
    (output / "source.json").write_text(json.dumps(metadata, indent=2) + "\n", encoding="utf-8")
    (output / "README.txt").write_text(
        "QtMaterial3 1.22 — existing native accessibility certification demos\n\n"
        "Run the named executable for your milestone:\n"
        "1.5: qtmaterial3_accessibility_certification\n"
        "1.7: qtmaterial3_catalogue_certification\n"
        "1.9: qtmaterial3_adaptive_desktop_certification\n"
        "1.10/1.11: qtmaterial3_gallery --route <route>\n\n"
        "Windows: extract the full ZIP, run the .exe; VC++ x64 redistributable may be required.\n"
        "Linux: extract the .tar.gz on Ubuntu 24.04 x86_64. Install system Qt 6 runtime, "
        "X11/Wayland dependencies as needed and Orca; see repository docs. No build required.\n"
        "macOS: extract the entire ZIP; .app bundles are ad-hoc signed, not notarized. "
        "Use the matching CPU architecture and organization's normal security policy.\n"
        "Start the real NVDA/Orca/VoiceOver before opening the demo. Record source.json "
        "and actual native announcements. Startup smoke is not native AT certification.\n"
        "Project source: https://github.com/faudard/qt-material3-widget\n"
        "Qt licensing and source: https://www.qt.io/licensing/ and https://code.qt.io/cgit/qt/qtbase.git/\n",
        encoding="utf-8",
    )
    root = Path(__file__).resolve().parents[2]
    for name in ("LICENSE", "LICENSE.md", "COPYING", "COPYING.LESSER"):
        if (root / name).is_file():
            shutil.copy2(root / name, output / name)
    if smoke_test:
        for binary in binaries:
            smoke(binary, output)
    return metadata


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--qt-version", required=True)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    commit = subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=root, text=True).strip()
    print(json.dumps(package(args.build.resolve(), args.output.resolve(), args.qt_version, commit), indent=2))


if __name__ == "__main__":
    main()
