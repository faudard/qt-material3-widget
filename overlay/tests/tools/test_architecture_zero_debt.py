from __future__ import annotations

import importlib.util
import json
import sys
import tempfile
import unittest
from pathlib import Path

SCRIPT = Path(__file__).resolve().parents[2] / "tools/check_architecture_zero_debt.py"
SPEC = importlib.util.spec_from_file_location("qtm3_zero_debt", SCRIPT)
assert SPEC is not None and SPEC.loader is not None
checker = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = checker
SPEC.loader.exec_module(checker)


class ZeroDebtPolicyTests(unittest.TestCase):
    def test_comments_only_baseline_is_empty(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "baseline.txt"
            path.write_text("# retired\n\n# no debt\n", encoding="utf-8")
            self.assertEqual([], checker.active_baseline_entries(path))

    def test_active_baseline_entry_is_debt(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "baseline.txt"
            path.write_text(
                "# legacy\nARCH-WIDGETS-RESOLVED-SPEC-ONLY|x|y|z\n",
                encoding="utf-8",
            )
            self.assertEqual(
                ["ARCH-WIDGETS-RESOLVED-SPEC-ONLY|x|y|z"],
                checker.active_baseline_entries(path),
            )

    def test_target_graph_requires_runtime_and_io_boundaries(self):
        config = {
            "required_targets": sorted(checker.REQUIRED_TARGETS),
            "target_forbidden_links": {
                "qtmaterial3_theme_model": [
                    "qtmaterial3_theme_io",
                    "qtmaterial3_theme_runtime",
                    "qtmaterial3_specs",
                    "qtmaterial3_core",
                    "qtmaterial3_widgets",
                ],
                "qtmaterial3_theme_io": [
                    "qtmaterial3_theme_runtime",
                    "qtmaterial3_specs",
                    "qtmaterial3_core",
                    "qtmaterial3_widgets",
                ],
                "qtmaterial3_theme_runtime": [
                    "qtmaterial3_theme_io",
                    "qtmaterial3_specs",
                    "qtmaterial3_core",
                    "qtmaterial3_widgets",
                ],
                "qtmaterial3_specs": [
                    "qtmaterial3_theme_io",
                    "qtmaterial3_theme_runtime",
                    "qtmaterial3_core",
                    "qtmaterial3_widgets",
                ],
                "qtmaterial3_core": [
                    "qtmaterial3_theme_io",
                    "qtmaterial3_widgets",
                ],
            },
        }
        self.assertEqual([], checker.check_config(config))

    def test_missing_runtime_target_fails(self):
        config = {
            "required_targets": sorted(
                checker.REQUIRED_TARGETS - {"qtmaterial3_theme_runtime"}
            ),
            "target_forbidden_links": {},
        }
        errors = checker.check_config(config)
        self.assertTrue(any("qtmaterial3_theme_runtime" in e for e in errors))

    def test_suppression_marker_is_detected(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            (root / "src").mkdir()
            (root / "src/example.cpp").write_text(
                "// ARCH-IGNORE: temporary\n",
                encoding="utf-8",
            )
            errors = checker.find_suppressions(root)
            self.assertEqual(1, len(errors))


if __name__ == "__main__":
    unittest.main()
