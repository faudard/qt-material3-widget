from __future__ import annotations

import copy
import importlib.util
import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

if not (ROOT / "tools/check_material_structural_conformance.py").is_file():
    raise unittest.SkipTest("Material conformance tooling is not implemented yet: tools/check_material_structural_conformance.py")
SCRIPT = ROOT / "tools/check_material_structural_conformance.py"
SPEC = importlib.util.spec_from_file_location(
    "qtm3_material_structural_conformance_tests", SCRIPT
)
assert SPEC is not None and SPEC.loader is not None
checker = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = checker
SPEC.loader.exec_module(checker)


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def load_json(path: Path):
    return json.loads(read(path))


class MaterialStructuralConformanceTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.model = load_json(ROOT / checker.MODEL_PATH)
        cls.suite = load_json(ROOT / checker.SUITE_PATH)
        cls.spec_header = read(ROOT / checker.SPEC_HEADER_PATH)
        cls.widget_header = read(ROOT / checker.WIDGET_HEADER_PATH)
        cls.resolver_source = read(ROOT / checker.RESOLVER_PATH)
        cls.widget_source = read(ROOT / checker.WIDGET_SOURCE_PATH)
        cls.render_helper = read(ROOT / checker.RENDER_HELPER_PATH)

    def test_active_filled_button_vertical_passes(self) -> None:
        self.assertEqual([], checker.validate_tree(ROOT))

    def test_reference_values_match_extracted_product_values(self) -> None:
        reference, reference_errors = checker.collect_reference_values(self.suite)
        product, product_errors = checker.extract_product_values(
            self.spec_header, self.widget_source
        )
        self.assertEqual([], reference_errors)
        self.assertEqual([], product_errors)
        self.assertEqual(reference, product)

    def test_changed_spec_value_is_detected_without_a_baseline(self) -> None:
        changed_header = self.spec_header.replace(
            "int horizontalPadding = 24;",
            "int horizontalPadding = 23;",
        )
        reference, _ = checker.collect_reference_values(self.suite)
        product, errors = checker.extract_product_values(
            changed_header, self.widget_source
        )
        self.assertEqual([], errors)
        self.assertNotEqual(
            reference["resolvedSpec.horizontalPadding"],
            product["resolvedSpec.horizontalPadding"],
        )

    def test_conflicting_case_expectation_is_rejected(self) -> None:
        suite = copy.deepcopy(self.suite)
        assertion = next(
            item
            for item in suite["cases"][1]["assertions"]
            if item["path"] == "resolvedSpec.containerHeight"
        )
        assertion["expected"] = 41
        _, errors = checker.collect_reference_values(suite)
        self.assertTrue(any("conflicts across cases" in error for error in errors))

    def test_missing_component_token_scope_is_rejected(self) -> None:
        resolver = self.resolver_source.replace(
            "QVector<ComponentId>{ ComponentId::Button, ComponentId::ButtonFilled }",
            "QVector<ComponentId>{ ComponentId::Button }",
        )
        errors = checker.validate_product_structure(
            widget_header=self.widget_header,
            resolver_source=resolver,
            widget_source=self.widget_source,
            render_helper=self.render_helper,
        )
        self.assertTrue(any("ButtonFilled" in error for error in errors))

    def test_missing_state_layer_slot_is_rejected(self) -> None:
        widget_source = self.widget_source.replace(
            "QtMaterialStateLayerPainter::paintPath(",
            "RemovedStateLayerPainter::paintPath(",
        )
        errors = checker.validate_product_structure(
            widget_header=self.widget_header,
            resolver_source=self.resolver_source,
            widget_source=widget_source,
            render_helper=self.render_helper,
        )
        self.assertTrue(any("slot state-layer" in error for error in errors))

    def test_first_vertical_covers_all_required_states(self) -> None:
        states = [
            state["id"]
            for state in self.model["states"]
            if state["required"] is True
        ]
        self.assertEqual(states, self.suite["caseDimensions"]["states"])
        self.assertEqual(10, len(self.suite["cases"]))


if __name__ == "__main__":
    unittest.main()
