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


OPTIONS = {
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
