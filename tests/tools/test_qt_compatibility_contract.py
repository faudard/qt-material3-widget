from __future__ import annotations

import copy
import importlib.util
import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TOOLS = ROOT / "tools"
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))

SCRIPT = TOOLS / "check_qt_compatibility_contract.py"
SPEC = importlib.util.spec_from_file_location("qtm3_qtcompat_checker", SCRIPT)
assert SPEC is not None and SPEC.loader is not None
checker = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = checker
SPEC.loader.exec_module(checker)

import qt_support

MANIFEST = ROOT / "docs/compatibility/qt-support.json"


class QtCompatibilityManifestTests(unittest.TestCase):
    def data(self):
        return json.loads(MANIFEST.read_text(encoding="utf-8"))

    def test_current_manifest_and_repository_are_valid(self):
        data = self.data()
        self.assertEqual([], qt_support.validate_manifest(data))
        self.assertEqual(
            [],
            checker.validate_repository_contract(data, ROOT),
        )

    def test_qt5_advertised_floor_tracks_minimum(self):
        data = copy.deepcopy(self.data())
        data["qt"]["5"]["advertisedFloor"] = "0.0.0"
        self.assertIn(
            "Qt5 advertisedFloor must equal the Qt5 minimum",
            qt_support.validate_manifest(data),
        )

    def test_primary_toolset_is_part_of_reference_set(self):
        data = copy.deepcopy(self.data())
        data["qt"]["5"]["primaryCertification"][
            "referenceMsvcToolsets"
        ] = ["14.28"]
        self.assertTrue(
            any(
                "hostedMsvcToolset" in error
                for error in qt_support.validate_manifest(data)
            )
        )

    def test_certification_profiles_are_manifest_driven(self):
        data = self.data()
        profiles = qt_support.certification_profiles(data)
        self.assertIn("qt5-primary", profiles)
        self.assertIn("qt6-linux-system", profiles)
        self.assertIn("qt6-windows", profiles)
        self.assertIn("qt6-macos", profiles)

    def test_unknown_profile_is_rejected(self):
        with self.assertRaises(qt_support.QtSupportError):
            qt_support.certification_profile(
                self.data(),
                "definitely-missing",
            )

    def test_qt6_certification_cannot_be_below_floor(self):
        data = copy.deepcopy(self.data())
        data["qt"]["6"]["certification"][1]["qtVersion"] = "1.0.0"
        self.assertTrue(
            any(
                "below the declared minimum" in error
                for error in qt_support.validate_manifest(data)
            )
        )


if __name__ == "__main__":
    unittest.main()
