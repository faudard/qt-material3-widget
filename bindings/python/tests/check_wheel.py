"""Fail-closed checks for a wheel built on the current runner.

A wheel's platform/Python tag cannot be inferred from a successful import:
validate its archive and exact PySide6 runtime requirement *before* installing.
This is a CI/build audit, not a repair tool (no auditwheel/delocate claim).
"""
from __future__ import annotations

import email
from pathlib import Path
import sys
import zipfile

from packaging.tags import sys_tags
from packaging.utils import parse_wheel_filename


def check_wheel(directory: Path) -> None:
    wheels = list(directory.glob("*.whl"))
    if len(wheels) != 1:
        raise AssertionError(f"Expected one built wheel in {directory}: {wheels}")

    wheel = wheels[0]
    name, version, build, tags = parse_wheel_filename(wheel.name)
    if str(name) != "qtmaterial3-widgets":
        raise AssertionError(f"Unexpected wheel distribution: {name}")
    if not tags.intersection(set(sys_tags())):
        raise AssertionError(f"Wheel is incompatible with this runner: {wheel.name}")

    with zipfile.ZipFile(wheel) as archive:
        contents = set(archive.namelist())
        for path in ("QtMaterial3/__init__.py", "QtMaterial3/Widgets.py",
                     "QtMaterial3/LazyTabs.py", "QtMaterial3/AsyncLazyTabs.py"):
            if path not in contents:
                raise AssertionError(f"Missing wheel facade: {path}")
        native = [
            path for path in contents
            if path.startswith("QtMaterial3/_QtMaterial3.")
            and (path.endswith(".so") or path.endswith(".pyd"))
        ]
        if len(native) != 1:
            raise AssertionError(f"Expected one native extension, found {native}")

        dist_info = f"qtmaterial3_widgets-{version}.dist-info/"
        metadata_file = dist_info + "METADATA"
        wheel_file = dist_info + "WHEEL"
        record_file = dist_info + "RECORD"
        for path in (metadata_file, wheel_file, record_file):
            if path not in contents:
                raise AssertionError(f"Missing wheel metadata: {path}")

        metadata = email.message_from_bytes(archive.read(metadata_file))
        requirements = metadata.get_all("Requires-Dist") or []
        if not any(req.replace(" ", "").lower() == "pyside6==6.6.3" for req in requirements):
            raise AssertionError(f"Missing pinned PySide6 runtime: {requirements}")

        # Native static QtMaterial3 libraries belong inside the extension.
        # Do not mistakenly ship Qt/PySide/Shiboken binaries from the SDK.
        forbidden = ("libQt6", "Qt6Core.dll", "Qt6Widgets.dll",
                     "libpyside6", "libshiboken6")
        if any(any(token.lower() in path.lower() for token in forbidden)
               for path in contents):
            raise AssertionError("Wheel accidentally bundles Qt/PySide/Shiboken runtime")

    print(f"Wheel OK: {wheel.name}, native extension {native[0]}")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise SystemExit("usage: check_wheel.py <wheel-directory>")
    check_wheel(Path(sys.argv[1]))
