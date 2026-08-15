from __future__ import annotations

import importlib.util
import json
import sys
import unittest
from pathlib import Path

SCRIPT = Path(__file__).resolve().parents[2] / "tools" / "check_build_consumer_matrix.py"
SPEC = importlib.util.spec_from_file_location("qtm3_matrix_checker", SCRIPT)
assert SPEC is not None and SPEC.loader is not None
checker = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = checker
SPEC.loader.exec_module(checker)

MATRIX = Path(__file__).resolve().parents[2] / "tests" / "consumers" / "consumer-matrix.json"

class ConsumerMatrixTests(unittest.TestCase):
    def load(self):
        return json.loads(MATRIX.read_text(encoding="utf-8"))

    def test_repository_matrix_is_valid(self):
        self.assertEqual([], checker.validate_matrix(self.load()))

    def test_ci_must_cover_both_linkages(self):
        data = self.load()
        data["profiles"]["ci"] = [
            n for n in data["profiles"]["ci"]
            if data["scenarios"][n]["linkage"] == "shared"
        ]
        self.assertTrue(any(
            "both shared and static" in e
            for e in checker.validate_matrix(data)
        ))

    def test_full_profile_is_exhaustive(self):
        data = self.load()
        data["profiles"]["full"].remove("fetchcontent-shared")
        self.assertIn(
            "full profile missing fetchcontent-shared",
            checker.validate_matrix(data),
        )

    def test_invalid_component_is_negative_contract(self):
        data = self.load()
        data["scenarios"]["invalid-package-component"]["expectConfigureFailure"] = False
        self.assertTrue(any(
            "must expect configure failure" in e
            for e in checker.validate_matrix(data)
        ))

if __name__ == "__main__":
    unittest.main()
