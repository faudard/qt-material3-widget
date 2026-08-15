from __future__ import annotations

import importlib.util
import sys
import tempfile
import unittest
from pathlib import Path

SCRIPT = Path(__file__).resolve().parents[2] / "tools" / "check_no_legacy_specfactory.py"
SPEC = importlib.util.spec_from_file_location("qtm3_no_specfactory", SCRIPT)
assert SPEC is not None and SPEC.loader is not None
checker = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = checker
SPEC.loader.exec_module(checker)


class NoLegacySpecFactoryTests(unittest.TestCase):
    def make_root(self) -> Path:
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        root = Path(temp.name)
        for d in checker.ACTIVE_ROOTS:
            (root / d).mkdir(parents=True, exist_ok=True)
        return root

    def test_resolver_based_tree_passes(self):
        root = self.make_root()
        (root/"src/specs.cpp").write_text(
            "ButtonSpec resolveButtonSpec();\n", encoding="utf-8"
        )
        (root/"README.md").write_text(
            "Specs use dedicated resolvers.\n", encoding="utf-8"
        )
        self.assertEqual([], checker.scan_tree(root))

    def test_symbol_reference_fails(self):
        root = self.make_root()
        (root/"src/legacy.cpp").write_text(
            "auto spec = SpecFactory::buttonSpec();\n", encoding="utf-8"
        )
        violations = checker.scan_tree(root)
        self.assertTrue(any("SpecFactory" in v.snippet for v in violations))

    def test_legacy_filename_fails_even_without_symbol(self):
        root = self.make_root()
        path = root/"include/qtmaterial/specs/qtmaterialspecfactory.h"
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text("#pragma once\n", encoding="utf-8")
        violations = checker.scan_tree(root)
        self.assertTrue(any("filename" in v.reason for v in violations))

    def test_current_readme_factory_wording_fails(self):
        root = self.make_root()
        (root/"README.md").write_text(
            "`qtmaterial3_specs` — immutable component specs and factory\n",
            encoding="utf-8",
        )
        violations = checker.scan_tree(root)
        self.assertTrue(any(v.path == Path("README.md") for v in violations))

    def test_roadmap_factory_path_fails(self):
        root = self.make_root()
        (root/"qt_material3_widget_roadmap_and_status.md").write_text(
            "[ ] default spec factory path exists\n", encoding="utf-8"
        )
        violations = checker.scan_tree(root)
        self.assertTrue(
            any(v.path == Path("qt_material3_widget_roadmap_and_status.md")
                for v in violations)
        )


if __name__ == "__main__":
    unittest.main()
