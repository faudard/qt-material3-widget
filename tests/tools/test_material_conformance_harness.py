from __future__ import annotations

import copy
import importlib.util
import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def load_module(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec is not None and spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


checker = load_module(
    "qtm3_material_conformance_harness_tests",
    ROOT / "tools/check_material_conformance_harness.py",
)
generator = load_module(
    "qtm3_material_conformance_generator_tests",
    ROOT / "tools/generate_material_conformance_report.py",
)
reference_checker = load_module(
    "qtm3_material_conformance_schema_tests",
    ROOT / "tools/check_material_reference_model.py",
)


def load_json(path: Path):
    return json.loads(path.read_text(encoding="utf-8"))


class MaterialConformanceHarnessTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.report_path = ROOT / checker.REPORT_PATH
        cls.report = load_json(cls.report_path)
        cls.registry = load_json(ROOT / checker.REGISTRY_PATH)
        cls.model = load_json(ROOT / checker.MODEL_PATH)
        cls.schema = load_json(ROOT / checker.SCHEMA_PATH)
        cls.by_id = {
            component["componentId"]: component
            for component in cls.report["components"]
        }

    def test_complete_conformance_harness_tree_passes(self) -> None:
        self.assertEqual([], checker.validate_tree(ROOT))

    def test_generated_report_is_current(self) -> None:
        generated = generator.build_report(ROOT)
        self.assertEqual(generated, self.report)
        self.assertEqual(
            generator.serialized_report(generated),
            self.report_path.read_text(encoding="utf-8"),
        )

    def test_summary_reports_the_complete_registry_without_overclaiming(self) -> None:
        self.assertEqual("incomplete", self.report["status"])
        self.assertEqual(
            {
                "componentCount": 32,
                "modeledCount": 5,
                "fullyConformantCount": 0,
                "incompleteCount": 5,
                "notModeledCount": 27,
                "failedCount": 0,
                "headlinePassCounts": {
                    "structure": 1,
                    "tokens": 5,
                    "states": 5,
                    "rendering": 0,
                },
            },
            self.report["summary"],
        )
        self.assertEqual(
            [item["id"] for item in self.registry],
            [item["componentId"] for item in self.report["components"]],
        )

    def test_filled_button_has_the_only_structural_pass(self) -> None:
        filled = self.by_id["button.filled"]
        self.assertEqual("pass", filled["headline"]["structure"])
        self.assertEqual(
            90, filled["dimensions"]["structure"]["assertionCount"]
        )
        self.assertEqual(
            10, filled["dimensions"]["structure"]["caseCount"]
        )
        for component_id in checker.EXPECTED_MODELED_COMPONENTS:
            if component_id == "button.filled":
                continue
            component = self.by_id[component_id]
            self.assertEqual("not-evaluated", component["headline"]["structure"])
            self.assertEqual(
                "no-evidence-suite",
                component["dimensions"]["structure"]["reason"],
            )

    def test_renderer_counts_are_aggregated_by_dimension(self) -> None:
        for component_id in checker.EXPECTED_MODELED_COMPONENTS:
            component = self.by_id[component_id]
            outlined = component_id == "button.outlined"
            self.assertEqual("pass", component["headline"]["tokens"])
            self.assertEqual("pass", component["headline"]["states"])
            self.assertEqual(
                100 if outlined else 70,
                component["dimensions"]["tokens"]["assertionCount"],
            )
            self.assertEqual(
                130 if outlined else 100,
                component["dimensions"]["semanticRendering"]["assertionCount"],
            )
            self.assertEqual(
                30,
                component["dimensions"]["states"]["assertionCount"],
            )

    def test_visual_contract_is_not_a_visual_pass(self) -> None:
        for component_id in checker.EXPECTED_MODELED_COMPONENTS:
            component = self.by_id[component_id]
            visual = component["dimensions"]["visualRendering"]
            self.assertEqual("pending-runtime", visual["status"])
            self.assertEqual("awaiting-reviewed-reference", visual["reason"])
            self.assertEqual(10, visual["caseCount"])
            self.assertEqual(4, visual["targetCount"])
            self.assertEqual(0, visual["passedTargetCount"])
            self.assertEqual("pending-runtime", component["headline"]["rendering"])
            self.assertEqual("incomplete", component["overallStatus"])
        self.assertFalse(
            any(
                item["overallStatus"] == "pass"
                for item in self.report["components"]
            )
        )

    def test_unmodeled_components_are_explicit_and_zero_counted(self) -> None:
        unmodeled = [
            item
            for item in self.report["components"]
            if item["componentId"] not in checker.EXPECTED_MODELED_COMPONENTS
        ]
        self.assertEqual(27, len(unmodeled))
        for component in unmodeled:
            self.assertEqual("not-modeled", component["modelStatus"])
            self.assertEqual("not-modeled", component["overallStatus"])
            for evidence in component["dimensions"].values():
                self.assertEqual("not-modeled", evidence["status"])
                self.assertEqual("no-reference-model", evidence["reason"])
                self.assertEqual([], evidence["sourceIds"])
                for field in checker.ZERO_COUNT_FIELDS:
                    self.assertEqual(0, evidence[field])

    def test_schema_rejects_an_unknown_report_field(self) -> None:
        changed = copy.deepcopy(self.report)
        changed["optionalOverride"] = True
        errors = reference_checker.validate_json_schema(changed, self.schema)
        self.assertTrue(any("additional property is forbidden" in error for error in errors))

    def test_schema_rejects_a_fifth_gate(self) -> None:
        changed = copy.deepcopy(self.report)
        changed["gates"].append(copy.deepcopy(changed["gates"][0]))
        errors = reference_checker.validate_json_schema(changed, self.schema)
        self.assertTrue(any("allows at most 4" in error for error in errors))

    def test_premature_visual_pass_is_rejected(self) -> None:
        changed = copy.deepcopy(self.report)
        changed["components"][0]["dimensions"]["visualRendering"]["status"] = "pass"
        errors = checker.validate_report_contract(changed, self.registry, self.model)
        self.assertTrue(any("must remain pending-runtime" in error for error in errors))
        self.assertTrue(any("must not claim runtime visual" in error for error in errors))

    def test_unproven_structural_pass_is_rejected(self) -> None:
        changed = copy.deepcopy(self.report)
        text = changed["components"][0]
        text["dimensions"]["structure"]["status"] = "pass"
        text["headline"]["structure"] = "pass"
        errors = checker.validate_report_contract(changed, self.registry, self.model)
        self.assertTrue(any("structural status" in error for error in errors))

    def test_injected_renderer_gate_failure_propagates_fail_closed(self) -> None:
        report = generator.build_report(
            ROOT,
            gate_errors={"renderer-conformance": ["synthetic failure"]},
        )
        self.assertEqual("failed", report["status"])
        gate = next(
            item for item in report["gates"] if item["id"] == "renderer-conformance"
        )
        self.assertEqual("fail", gate["status"])
        self.assertEqual(1, gate["errorCount"])
        for component in report["components"][:5]:
            self.assertEqual("fail", component["dimensions"]["tokens"]["status"])
            self.assertEqual("fail", component["dimensions"]["states"]["status"])
            self.assertEqual(
                "fail", component["dimensions"]["semanticRendering"]["status"]
            )
            self.assertEqual("fail", component["headline"]["rendering"])
            self.assertEqual("fail", component["overallStatus"])

    def test_source_audit_rejects_escape_markers(self) -> None:
        generator_text = (ROOT / checker.GENERATOR_PATH).read_text(encoding="utf-8")
        checker_text = (ROOT / checker.CHECKER_PATH).read_text(encoding="utf-8")
        errors = checker.audit_source_texts(
            generator_text + "\n# baseline escape\n",
            checker_text + "\n# suppression escape\n",
        )
        self.assertTrue(any("forbidden marker: baseline" in error for error in errors))
        self.assertTrue(any("forbidden marker: suppression" in error for error in errors))

    def test_source_audit_rejects_subprocess_import(self) -> None:
        generator_text = (ROOT / checker.GENERATOR_PATH).read_text(encoding="utf-8")
        checker_text = (ROOT / checker.CHECKER_PATH).read_text(encoding="utf-8")
        errors = checker.audit_source_texts(
            generator_text + "\nimport subprocess\n",
            checker_text,
        )
        self.assertIn("generator must not import subprocess", errors)


if __name__ == "__main__":
    unittest.main()
