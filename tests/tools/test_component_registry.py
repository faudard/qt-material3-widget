from __future__ import annotations

import importlib.util
import json
import sys
import tempfile
import unittest
from pathlib import Path

SCRIPT = Path(__file__).resolve().parents[2] / "tools" / "check_component_registry.py"
SPEC = importlib.util.spec_from_file_location("qtm3_component_registry_checker", SCRIPT)
assert SPEC is not None and SPEC.loader is not None
checker = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = checker
SPEC.loader.exec_module(checker)

AXES = [
    "api","rendering","states","accessibility","keyboard",
    "hidpi","rtl","tests","example","docs",
]


def complete_component(cid="button.filled"):
    evidence = {axis: [f"{axis} evidence"] for axis in AXES}
    axes = {axis: 4 for axis in AXES}
    axes.update({
        "lastReviewed": "2026-08-15",
        "gaps": [],
        "nextActions": ["keep evidence current"],
        "evidence": evidence,
    })
    return {
        "id": cid,
        "name": "Filled Button",
        "family": "Buttons",
        "maturity": "complete",
        "maturityPolicy": "manual",
        "publicHeader": f"qtmaterial/widgets/buttons/{cid.replace('.', '')}.h",
        "specType": "ButtonSpec",
        "widgetType": "QtMaterialFilledButton",
        "testTarget": "tst_filledbutton",
        "galleryRoute": f"/{cid.replace('.', '/')}",
        "docsPath": "docs/public-api/buttons.md",
        "releaseScope": True,
        "referenceCandidate": True,
        "maturityAxes": axes,
    }


class GovernanceTests(unittest.TestCase):
    def test_complete_component_passes(self):
        errors, warnings = checker.validate_governance([complete_component()], axes=AXES)
        self.assertEqual([], errors)
        self.assertEqual([], warnings)

    def test_duplicate_public_header_fails(self):
        a = complete_component("button.a")
        b = complete_component("button.b")
        b["publicHeader"] = a["publicHeader"]
        b["widgetType"] = "QtMaterialB"
        b["galleryRoute"] = "/button/b"
        b["referenceCandidate"] = False
        errors, _ = checker.validate_governance([a, b], axes=AXES)
        self.assertTrue(any("publicHeader" in e and "multiple components" in e for e in errors))

    def test_complete_component_requires_all_axis_evidence(self):
        item = complete_component()
        del item["maturityAxes"]["evidence"]["keyboard"]
        errors, _ = checker.validate_governance([item], axes=AXES)
        self.assertIn(
            "button.filled: complete component requires non-empty evidence.keyboard",
            errors,
        )

    def test_reference_candidate_must_be_complete(self):
        item = complete_component()
        item["maturity"] = "usable"
        item.pop("maturityAxes")
        errors, _ = checker.validate_governance([item], axes=AXES)
        self.assertIn("button.filled: referenceCandidate must be complete", errors)

    def test_implicit_release_and_reference_metadata_warn(self):
        item = complete_component()
        item.pop("releaseScope")
        item.pop("referenceCandidate")
        errors, warnings = checker.validate_governance([item], axes=AXES)
        self.assertEqual([], errors)
        self.assertTrue(any("releaseScope is implicit" in w for w in warnings))
        self.assertTrue(any("referenceCandidate is implicit" in w for w in warnings))

    def test_schema_contract_is_versioned(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "schema.json"
            path.write_text(json.dumps({
                "$schema": "https://json-schema.org/draft/2020-12/schema",
                "$id": "test",
                "x-qtm3-schemaVersion": 1,
                "type": "array",
                "items": {
                    "required": [
                        "id","name","family","maturity","publicHeader",
                        "specType","widgetType","galleryRoute","docsPath"
                    ]
                },
            }), encoding="utf-8")
            self.assertEqual([], checker.validate_schema_contract(path))


if __name__ == "__main__":
    unittest.main()
