from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TOOLS = ROOT / "tools"
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))

SPEC = importlib.util.spec_from_file_location(
    "qtm3_release_checker",
    TOOLS / "check_release.py",
)
assert SPEC is not None and SPEC.loader is not None
release = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = release
SPEC.loader.exec_module(release)

import component_registry


class ReleaseCheckerTests(unittest.TestCase):
    def test_project_version_is_parsed(self):
        text = """
        project(
            qt-material3-widgets
            VERSION 0.5.0
            LANGUAGES CXX
        )
        """
        self.assertEqual("0.5.0", release.project_version(text))

    def test_current_checkout_satisfies_all_release_scopes(self):
        self.assertEqual([], release.validate(ROOT))

    def test_individual_scopes_remain_selectable(self):
        self.assertEqual(
            [],
            release.validate(ROOT, scopes=["theme"]),
        )
        self.assertEqual(
            [],
            release.validate(ROOT, scopes=["interaction"]),
        )

    def test_wrong_expected_version_is_reported(self):
        errors = release.validate(ROOT, expected_version="9.9.9")
        self.assertTrue(
            any("expected 9.9.9" in error for error in errors)
        )

    def test_derived_usable_requires_state_and_example_evidence(self):
        axes = {
            "api": 2,
            "rendering": 2,
            "states": 2,
            "accessibility": 1,
            "keyboard": 1,
            "hidpi": 1,
            "rtl": 1,
            "tests": 2,
            "example": 1,
            "docs": 2,
        }
        self.assertEqual(
            "usable",
            component_registry.derived_maturity(axes),
        )
        axes["example"] = 0
        self.assertEqual(
            "partial",
            component_registry.derived_maturity(axes),
        )

    def test_not_applicable_axis_counts_as_certified(self):
        axes = {
            "api": 4,
            "rendering": 4,
            "states": 4,
            "accessibility": 4,
            "keyboard": "N/A",
            "hidpi": 4,
            "rtl": 4,
            "tests": 4,
            "example": 4,
            "docs": 4,
        }
        self.assertEqual(
            "complete",
            component_registry.derived_maturity(axes),
        )


if __name__ == "__main__":
    unittest.main()
