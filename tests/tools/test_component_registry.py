from __future__ import annotations

import copy
import importlib.util
import json
import sys
import tempfile
import unittest
from pathlib import Path

SCRIPT = Path(__file__).resolve().parents[2] / "tools" / "check_component_registry.py"
ROOT = SCRIPT.parents[1]
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
            "button.filled: maturityAxes.evidence.keyboard must contain non-empty statements",
            errors,
        )

    def test_reference_candidate_must_be_complete(self):
        item = complete_component()
        item["maturity"] = "usable"
        item.pop("maturityAxes")
        errors, _ = checker.validate_governance([item], axes=AXES)
        self.assertIn("button.filled: referenceCandidate must be complete", errors)

    def test_implicit_release_and_reference_metadata_fail(self):
        item = complete_component()
        item.pop("releaseScope")
        item.pop("referenceCandidate")
        errors, warnings = checker.validate_governance([item], axes=AXES)
        self.assertEqual([], warnings)
        self.assertTrue(any("releaseScope must be an explicit boolean" in e for e in errors))
        self.assertTrue(any("referenceCandidate must be an explicit boolean" in e for e in errors))

    def test_every_axis_requires_evidence_even_below_complete(self):
        item = complete_component()
        item["maturity"] = "usable"
        item["maturityPolicy"] = "derived"
        item["maturityAxes"].update({
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
            "gaps": ["runtime conformance is pending"],
        })
        del item["maturityAxes"]["evidence"]["rtl"]
        errors, _ = checker.validate_governance([item], axes=AXES)
        self.assertTrue(any("evidence.rtl" in error for error in errors))

    def test_non_complete_component_requires_a_gap(self):
        item = complete_component()
        item["maturity"] = "usable"
        item["maturityPolicy"] = "derived"
        item["maturityAxes"].update({
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
        })
        errors, _ = checker.validate_governance([item], axes=AXES)
        self.assertTrue(any("must declare a maturity gap" in error for error in errors))

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
                        "id","name","family","maturity","maturityPolicy",
                        "publicHeader","specType","widgetType","testTarget","galleryRoute",
                        "docsPath","releaseScope","referenceCandidate",
                        "maturityAxes"
                    ],
                    "additionalProperties": False,
                    "properties": {
                        "maturityAxes": {
                            "required": AXES + [
                                "lastReviewed", "gaps", "nextActions", "evidence"
                            ]
                        }
                    },
                },
            }), encoding="utf-8")
            self.assertEqual([], checker.validate_schema_contract(path))


class CurrentRegistryTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.generator = checker.load_generator()
        cls.components = cls.generator.load_registry()

    def test_current_registry_has_zero_strict_debt(self):
        base_errors, base_warnings = self.generator.validate_registry(
            self.components, strict=False
        )
        governance_errors, governance_warnings = checker.validate_governance(
            self.components, axes=self.generator.AXES
        )
        self.assertEqual([], base_errors)
        self.assertEqual([], base_warnings)
        self.assertEqual([], governance_errors)
        self.assertEqual([], governance_warnings)

    def test_every_component_has_closed_evaluated_metadata(self):
        for component in self.components:
            self.assertIsInstance(component["releaseScope"], bool)
            self.assertIsInstance(component["referenceCandidate"], bool)
            axes = component["maturityAxes"]
            for axis in self.generator.AXES:
                self.assertIn(axis, axes)
                self.assertIsNotNone(axes[axis])
                self.assertTrue(axes["evidence"][axis])
            self.assertTrue(axes["nextActions"])
            self.assertEqual(
                component["maturity"],
                self.generator.derived_maturity(axes),
            )
            if component["maturity"] == "complete":
                self.assertEqual([], axes["gaps"])
            else:
                self.assertTrue(axes["gaps"])

    def test_non_button_assessments_are_conservative(self):
        for component in self.components:
            if component["family"] == "Buttons":
                continue
            for axis in self.generator.AXES:
                value = component["maturityAxes"][axis]
                if isinstance(value, int):
                    self.assertLessEqual(value, 2, component["id"])
            self.assertFalse(component["referenceCandidate"])

    def test_missing_axis_and_unknown_evidence_are_rejected(self):
        changed = copy.deepcopy(self.components[8])
        del changed["maturityAxes"]["rtl"]
        changed["maturityAxes"]["evidence"]["invented"] = ["not allowed"]
        changed["invented"] = True
        errors, _ = checker.validate_governance(
            [changed], axes=self.generator.AXES
        )
        self.assertTrue(any("maturityAxes.rtl must be evaluated" in e for e in errors))
        self.assertTrue(any("unknown maturityAxes.evidence field" in e for e in errors))
        self.assertTrue(any("unknown component registry field" in e for e in errors))

    def test_generated_matrix_has_zero_unevaluated_axes(self):
        maturity = (ROOT / "docs/components/maturity.md").read_text(
            encoding="utf-8"
        )
        section = maturity.split("## Not evaluated axes", 1)[1].split(
            "## Next actions", 1
        )[0]
        for axis in self.generator.AXES:
            label = self.generator.AXIS_LABELS[axis]
            self.assertIn(f"| {label} | 0 |", section)

    def test_component_maturity_workflow_is_strict(self):
        workflow = (ROOT / ".github/workflows/component-maturity.yml").read_text(
            encoding="utf-8"
        )
        self.assertIn(
            "python tools/check_component_registry.py --check-generated --strict",
            workflow,
        )


if __name__ == "__main__":
    unittest.main()
