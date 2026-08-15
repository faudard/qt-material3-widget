from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

def load(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module

generator = load(
    "qtm3_material_seed",
    ROOT / "tools/generate_material_button_reference_seed.py",
)
validator = load(
    "qtm3_material_reference",
    ROOT / "tools/check_material_reference_model.py",
)


class MaterialReferenceModelTests(unittest.TestCase):
    def registry(self):
        return {
            "button.text": {"id": "button.text", "family": "Buttons"},
            "button.filled": {
                "id": "button.filled",
                "family": "Buttons",
                "referenceCandidate": True,
            },
            "button.filled-tonal": {
                "id": "button.filled-tonal", "family": "Buttons"
            },
            "button.outlined": {"id": "button.outlined", "family": "Buttons"},
            "button.elevated": {"id": "button.elevated", "family": "Buttons"},
        }

    def profiles(self):
        return {
            "m3-classic.compose-material3-1.4.0": {
                "id": "m3-classic.compose-material3-1.4.0",
                "sources": [
                    {"id": "m3-buttons"},
                    {"id": "m3-design-tokens"},
                    {"id": "m3-states"},
                    {"id": "androidx-material3-1.4.0"},
                ],
            }
        }

    def test_button_seed_is_50_case_cartesian_product(self):
        suite = generator.generate()
        self.assertEqual(50, len(suite["cases"]))
        keys = {
            (c["componentId"], c["state"], c["theme"])
            for c in suite["cases"]
        }
        self.assertEqual(50, len(keys))
        self.assertEqual(validator.expected_case_keys(suite), keys)

    def test_seed_suite_validates_against_registry(self):
        errors = validator.validate_suite(
            generator.generate(),
            self.profiles(),
            self.registry(),
        )
        self.assertEqual([], errors)

    def test_anchor_must_be_reference_candidate(self):
        registry = self.registry()
        registry["button.filled"]["referenceCandidate"] = False
        errors = validator.validate_suite(
            generator.generate(), self.profiles(), registry
        )
        self.assertTrue(any("not referenceCandidate" in e for e in errors))

    def test_unknown_component_fails(self):
        suite = generator.generate()
        suite["caseDimensions"]["components"][0] = "button.future"
        errors = validator.validate_suite(suite, self.profiles(), self.registry())
        self.assertTrue(any("missing from product registry" in e for e in errors))

    def test_evidence_backed_case_requires_assertions(self):
        suite = generator.generate()
        suite["status"] = "evidence-backed"
        errors = validator.validate_suite(suite, self.profiles(), self.registry())
        self.assertTrue(any("zero assertions" in e for e in errors))

    def test_missing_source_ref_fails(self):
        suite = generator.generate()
        suite["sourceRefs"].append("missing-source")
        errors = validator.validate_suite(suite, self.profiles(), self.registry())
        self.assertTrue(any("sourceRef missing" in e for e in errors))


if __name__ == "__main__":
    unittest.main()
