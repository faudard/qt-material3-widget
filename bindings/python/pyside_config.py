# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause
#
# Minimal Qt for Python 6.4-compatible discovery helper derived from the
# official PySide samplebinding pyside_config.py. It intentionally exposes
# only the paths required by the QtMaterial3 binding build.

from __future__ import annotations

import glob
import importlib
import importlib.machinery
import importlib.metadata
import os
import sys
import sysconfig


def package_path(name: str) -> str:
    module = importlib.import_module(name)
    path = getattr(module, "__path__", None)
    if path:
        return os.path.realpath(next(iter(path)))
    filename = getattr(module, "__file__", None)
    if not filename:
        raise RuntimeError(f"Unable to locate package {name}")
    return os.path.realpath(os.path.dirname(filename))


def shared_library_glob(package: str, token: str) -> str:
    root = package_path(package)
    if sys.platform == "win32":
        pattern = "*.lib"
    elif sys.platform == "darwin":
        pattern = "lib*.dylib"
    else:
        pattern = "lib*.so.*"
    libraries = [
        os.path.realpath(path)
        for path in glob.glob(os.path.join(root, pattern))
        if token in os.path.basename(path).lower()
    ]
    if not libraries:
        raise RuntimeError(f"No {token} shared library found in {root}")
    return ";".join(libraries)


def extension_suffix() -> str:
    value = sysconfig.get_config_var("EXT_SUFFIX")
    if value:
        return value
    return importlib.machinery.EXTENSION_SUFFIXES[0]


# Pinned at the ABI proven by the 1.17.0–1.17.3 wheel lane.
# PySide6 6.6.3 explicitly requires CPython < 3.13 on PyPI.
EXPECTED_QT_VERSION = "6.6.3"


def check_abi(*, require_generator: bool = True) -> str:
    if sys.implementation.name != "cpython":
        raise RuntimeError("QtMaterial3 wheels require CPython")
    if not ((3, 10) <= sys.version_info[:2] < (3, 13)):
        raise RuntimeError(
            "QtMaterial3/PySide6 6.6.3 supports CPython 3.10–3.12, "
            f"not {sys.version_info.major}.{sys.version_info.minor}"
        )
    if sys.maxsize <= 2**32:
        raise RuntimeError("QtMaterial3 wheels require a 64-bit Python interpreter")

    names = ["PySide6", "PySide6-Essentials", "PySide6-Addons", "shiboken6"]
    # The generator is a PEP 517 build dependency and is deliberately absent
    # from a clean installation of the published runtime wheel.
    if require_generator:
        names.append("shiboken6-generator")
    versions = {name: importlib.metadata.version(name) for name in names}
    mismatched = {name: version for name, version in versions.items()
                  if version != EXPECTED_QT_VERSION}
    if mismatched:
        raise RuntimeError(
            f"PySide6/Shiboken ABI mismatch: expected {EXPECTED_QT_VERSION}; "
            f"found {mismatched}"
        )

    from PySide6.QtCore import qVersion

    runtime_version = qVersion()
    if runtime_version != EXPECTED_QT_VERSION:
        raise RuntimeError(
            f"Qt runtime {runtime_version} does not match "
            f"PySide6/Shiboken {EXPECTED_QT_VERSION}"
        )
    return runtime_version


OPTIONS = {
    "--verify-abi": lambda: "QtMaterial3 build ABI verified: Qt " + check_abi(),
    "--verify-runtime-abi": lambda: (
        "QtMaterial3 runtime ABI verified: Qt "
        + check_abi(require_generator=False)
    ),
    "--qt-version": check_abi,
    "--shiboken-module-path": lambda: package_path("shiboken6"),
    "--shiboken-generator-path": lambda: package_path("shiboken6_generator"),
    "--pyside-path": lambda: package_path("PySide6"),
    "--python-include-path": lambda: sysconfig.get_path("include"),
    "--shiboken-generator-include-path": lambda: os.path.join(
        package_path("shiboken6_generator"), "include"
    ),
    "--pyside-include-path": lambda: os.path.join(package_path("PySide6"), "include"),
    "--shiboken-module-shared-libraries-cmake": lambda: shared_library_glob(
        "shiboken6", "shiboken"
    ),
    "--pyside-shared-libraries-cmake": lambda: shared_library_glob(
        "PySide6", "pyside"
    ),
    "--python-extension-suffix": extension_suffix,
}


def main() -> int:
    if len(sys.argv) != 2 or sys.argv[1] not in OPTIONS:
        print("usage: pyside_config.py <option>", file=sys.stderr)
        return 2
    try:
        print(OPTIONS[sys.argv[1]]())
    except Exception as exc:
        print(str(exc), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
