import importlib.util
import pathlib
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[2]
SPEC = importlib.util.spec_from_file_location(
    "check_release_contract",
    ROOT / "tools" / "check_release_contract.py",
)
MODULE = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
SPEC.loader.exec_module(MODULE)


class ReleaseContractTests(unittest.TestCase):
    def test_project_version_is_parsed(self):
        text = """
        project(
            qt-material3-widgets
            VERSION 0.5.0
            LANGUAGES CXX
        )
        """
        self.assertEqual(MODULE.project_version(text), "0.5.0")

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
        self.assertEqual(MODULE.derived_maturity(axes), "usable")

        axes["example"] = 0
        self.assertEqual(MODULE.derived_maturity(axes), "partial")

    def test_keyboard_not_applicable_counts_as_certified_axis(self):
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
        self.assertEqual(MODULE.derived_maturity(axes), "complete")


if __name__ == "__main__":
    unittest.main()
