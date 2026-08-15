from __future__ import annotations
import importlib.util, json, sys, unittest
from pathlib import Path

SCRIPT = Path(__file__).resolve().parents[2] / "tools/check_qt_compatibility_contract.py"
SPEC = importlib.util.spec_from_file_location("qtm3_qtcompat_checker", SCRIPT)
checker = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = checker
SPEC.loader.exec_module(checker)

MANIFEST = Path(__file__).resolve().parents[2] / "docs/compatibility/qt-support.json"

class QtCompatibilityManifestTests(unittest.TestCase):
    def data(self):
        return json.loads(MANIFEST.read_text(encoding="utf-8"))

    def test_manifest_is_valid(self):
        self.assertEqual([], checker.validate_manifest(self.data()))

    def test_qt5_floor_is_exact(self):
        d = self.data()
        d["qt"]["5"]["minimum"] = "5.14.0"
        self.assertIn("Qt5 minimum must be exactly 5.14.2",
                      checker.validate_manifest(d))

    def test_qt5_archive_is_pinned(self):
        d = self.data()
        d["qt"]["5"]["primaryCertification"]["qtArchive"] = "win64_msvc2019_64"
        self.assertIn("Qt5 Windows archive must be win64_msvc2017_64",
                      checker.validate_manifest(d))

    def test_reference_toolsets_include_1428_and_1429(self):
        d = self.data()
        d["qt"]["5"]["primaryCertification"]["referenceMsvcToolsets"] = ["14.29"]
        self.assertTrue(any("14.28 and 14.29" in e
                            for e in checker.validate_manifest(d)))

    def test_qt6_floor_is_explicit(self):
        d = self.data()
        d["qt"]["6"]["minimum"] = "6.5.0"
        self.assertIn("Qt6 minimum must be 6.4.0",
                      checker.validate_manifest(d))

if __name__ == "__main__":
    unittest.main()
