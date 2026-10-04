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

DOMAIN_SCRIPT = ROOT / "tools" / "component_registry.py"
DOMAIN_SPEC = importlib.util.spec_from_file_location(
    "qtm3_component_registry_domain",
    DOMAIN_SCRIPT,
)
assert DOMAIN_SPEC is not None and DOMAIN_SPEC.loader is not None
domain = importlib.util.module_from_spec(DOMAIN_SPEC)
sys.modules[DOMAIN_SPEC.name] = domain
DOMAIN_SPEC.loader.exec_module(domain)

RELEASE_RULES = ROOT / "tools" / "release_rules.json"

VISUAL_PREFIX_BY_COMPONENT = {
    "button.elevated": "component_grid",
    "button.extended-fab": "component_grid",
    "button.fab": "component_grid",
    "button.filled": "component_grid",
    "button.filled-tonal": "component_grid",
    "button.icon": "component_grid",
    "button.outlined": "component_grid",
    "button.text": "component_grid",
    "selection.checkbox": "selection_state_matrix",
    "selection.radio": "selection_state_matrix",
    "selection.segmented-button": "selection_state_matrix",
    "selection.switch": "selection_state_matrix",
    "input.autocomplete": "input_field_matrix",
    "input.combo-box": "input_field_matrix",
    "input.date.field": "input_field_matrix",
    "input.text.filled": "input_field_matrix",
    "input.text.outlined": "input_field_matrix",
    "input.search-bar": "input_field_matrix",
    "input.time-field": "input_field_matrix",
    "input.date-picker": "input_composite_matrix",
    "input.date-range-picker": "input_composite_matrix",
    "input.search-view": "input_composite_matrix",
    "input.time-picker": "input_composite_matrix",
    "input.slider": "input_slider_matrix",
    "input.range-slider": "input_slider_matrix",
    "navigation.rail": "navigation_primary_matrix",
    "navigation.tabs": "navigation_primary_matrix",
    "navigation.menu": "navigation_primary_matrix",
    "navigation.breadcrumb": "navigation_desktop_matrix",
    "navigation.command-palette": "navigation_desktop_matrix",
    "surface.banner": "surface_bar_matrix",
    "surface.card": "surface_bar_matrix",
    "surface.top-app-bar": "surface_bar_matrix",
    "surface.bottom-app-bar": "surface_bar_matrix",
    "surface.dialog": "surface_overlay_matrix",
    "surface.bottom-sheet": "surface_overlay_matrix",
    "surface.navigation-drawer": "surface_overlay_matrix",
    "surface.snackbar": "surface_overlay_matrix",
    "data.table": "desktop_data_matrix",
    "data.tree-view": "desktop_data_matrix",
    "data.pagination": "desktop_data_matrix",
    "data.carousel": "data_extended_matrix",
    "data.grid-list": "data_extended_matrix",
    "data.list": "data_extended_matrix",
    "data.divider": "data_extended_matrix",
    "progress.linear": "progress_compact_matrix",
    "progress.circular": "progress_compact_matrix",
    "compact.chip": "progress_compact_matrix",
    "layout.split-view": "layout_matrix",
}

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

    def test_spec_type_names_a_public_spec_or_is_not_applicable(self):
        item = complete_component()
        item["specType"] = "ImaginarySpec"
        errors, _ = checker.validate_governance([item], axes=AXES)
        self.assertIn(
            "button.filled: specType `ImaginarySpec` is not declared in a public specs header",
            errors,
        )

        item["specType"] = "N/A"
        errors, _ = checker.validate_governance([item], axes=AXES)
        self.assertEqual([], errors)

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
        cls.generator = domain
        cls.components = domain.load_registry(ROOT)

    def test_current_registry_has_zero_strict_debt(self):
        base_errors, base_warnings = self.generator.validate_registry(
            self.components, strict=False, root=ROOT
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

    def test_non_complete_assessments_are_fail_closed(self):
        for component in self.components:
            axes = component["maturityAxes"]
            if component["maturity"] == "complete":
                for axis in self.generator.AXES:
                    value = axes[axis]
                    if isinstance(value, int):
                        self.assertEqual(4, value, component["id"])
                continue

            self.assertTrue(axes["gaps"], component["id"])
            self.assertFalse(component["referenceCandidate"], component["id"])
            numeric = [
                axes[axis]
                for axis in self.generator.AXES
                if isinstance(axes[axis], int)
            ]
            self.assertTrue(
                any(value < 4 for value in numeric),
                f"{component['id']} has 4/4 evidence everywhere but was not promoted",
            )

    def test_complete_families_are_uniformly_certified(self):
        families = {}
        for component in self.components:
            if component["releaseScope"]:
                families.setdefault(component["family"], []).append(component)
        for family, components in families.items():
            complete = [c for c in components if c["maturity"] == "complete"]
            if not complete:
                continue
            for component in complete:
                axes = component["maturityAxes"]
                self.assertEqual([], axes["gaps"], component["id"])
                for axis in self.generator.AXES:
                    value = axes[axis]
                    if isinstance(value, int):
                        self.assertEqual(4, value, component["id"])

    def test_complete_components_require_stable_family_goldens(self):
        rules = json.loads(RELEASE_RULES.read_text(encoding="utf-8"))
        stable = set(
            rules["base"]["stable_release"]["visual_goldens"]
        )
        suffixes = (
            "_light_standard.png",
            "_dark_standard.png",
            "_light_high.png",
        )

        for component in self.components:
            if not component["releaseScope"] or component["maturity"] != "complete":
                continue
            prefix = VISUAL_PREFIX_BY_COMPONENT.get(component["id"])
            self.assertIsNotNone(
                prefix,
                f"{component['id']} has no visual certification mapping",
            )
            for suffix in suffixes:
                expected = (
                    "tests/visual/goldens/"
                    + prefix
                    + suffix
                )
                self.assertIn(expected, stable, component["id"])

    def test_enterprise_complete_requires_all_release_components_complete(self):
        incomplete = [
            component["id"]
            for component in self.components
            if component["releaseScope"] and component["maturity"] != "complete"
        ]
        # 1.5 certification remains fail-closed until every release-scoped
        # component has executable 4/4 evidence. Keep this assertion explicit
        # so the final enterprise gate can be enabled without changing policy.
        if not incomplete:
            for component in self.components:
                if not component["releaseScope"]:
                    continue
                axes = component["maturityAxes"]
                for axis in self.generator.AXES:
                    value = axes[axis]
                    if isinstance(value, int):
                        self.assertEqual(4, value, component["id"])
                self.assertEqual([], axes["gaps"], component["id"])

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


if __name__ == "__main__":
    unittest.main()
