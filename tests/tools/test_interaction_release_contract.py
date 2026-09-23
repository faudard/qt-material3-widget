import importlib.util
import pathlib
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[2]
SPEC = importlib.util.spec_from_file_location(
    "check_interaction_release_contract",
    ROOT / "tools" / "check_interaction_release_contract.py",
)
MODULE = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
SPEC.loader.exec_module(MODULE)


class InteractionReleaseContractTests(unittest.TestCase):
    def test_current_checkout_satisfies_interaction_contract(self):
        self.assertEqual(MODULE.validate(ROOT), [])

    def test_wrong_expected_version_is_reported(self):
        errors = MODULE.validate(ROOT, "9.9.9")
        self.assertTrue(any("expected 9.9.9" in error for error in errors))

    def test_project_version_parser(self):
        text = "project(qt-material3-widgets VERSION 1.2.3 LANGUAGES CXX)"
        self.assertEqual(MODULE.project_version(text), "1.2.3")


if __name__ == "__main__":
    unittest.main()
