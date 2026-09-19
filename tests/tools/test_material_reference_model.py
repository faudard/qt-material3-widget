from __future__ import annotations

import copy
import importlib.util
import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

if not (ROOT / "tools/generate_material_button_reference_seed.py").is_file():
    raise unittest.SkipTest("Material conformance tooling is not implemented yet: tools/generate_material_button_reference_seed.py")


def load_module(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec is not None and spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


generator = load_module(
    "qtm3_material_button_seed_tests",
    ROOT / "tools/generate_material_button_reference_seed.py",
)
structure_generator = load_module(
    "qtm3_material_button_filled_structure_tests",
    ROOT / "tools/generate_material_button_filled_structure_reference.py",
)
renderer_generator = load_module(
    "qtm3_material_button_filled_renderer_tests",
    ROOT / "tools/generate_material_button_filled_renderer_reference.py",
)
elevated_renderer_generator = load_module(
    "qtm3_material_button_elevated_renderer_tests",
    ROOT / "tools/generate_material_button_elevated_renderer_reference.py",
)
filled_tonal_renderer_generator = load_module(
    "qtm3_material_button_filled_tonal_renderer_tests",
    ROOT / "tools/generate_material_button_filled_tonal_renderer_reference.py",
)
outlined_renderer_generator = load_module(
    "qtm3_material_button_outlined_renderer_tests",
    ROOT / "tools/generate_material_button_outlined_renderer_reference.py",
)
text_renderer_generator = load_module(
    "qtm3_material_button_text_renderer_tests",
    ROOT / "tools/generate_material_button_text_renderer_reference.py",
)
validator = load_module(
    "qtm3_material_reference_tests",
    ROOT / "tools/check_material_reference_model.py",
)


def load_json(path: Path):
    return json.loads(path.read_text(encoding="utf-8"))


class MaterialReferenceModelTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.catalog = load_json(
            ROOT / "references/material3/reference-catalog.json"
        )
        cls.profile = load_json(
            ROOT
            / "references/material3/classic/compose-material3-1.4.0/profile.json"
        )
        cls.model = load_json(
            ROOT
            / "references/material3/classic/compose-material3-1.4.0/components/buttons.json"
        )
        cls.suite = load_json(
            ROOT
            / "references/material3/classic/compose-material3-1.4.0/suites/buttons-seed.json"
        )
        cls.structure_suite = load_json(
            ROOT
            / "references/material3/classic/compose-material3-1.4.0/"
            "suites/filled-button-structure.json"
        )
        cls.renderer_suite = load_json(
            ROOT
            / "references/material3/classic/compose-material3-1.4.0/"
            "suites/filled-button-renderer.json"
        )
        cls.elevated_renderer_suite = load_json(
            ROOT
            / "references/material3/classic/compose-material3-1.4.0/"
            "suites/elevated-button-renderer.json"
        )
        cls.filled_tonal_renderer_suite = load_json(
            ROOT
            / "references/material3/classic/compose-material3-1.4.0/"
            "suites/filled-tonal-button-renderer.json"
        )
        cls.outlined_renderer_suite = load_json(
            ROOT
            / "references/material3/classic/compose-material3-1.4.0/"
            "suites/outlined-button-renderer.json"
        )
        cls.text_renderer_suite = load_json(
            ROOT
            / "references/material3/classic/compose-material3-1.4.0/"
            "suites/text-button-renderer.json"
        )
        cls.registry = validator.index_registry(
            load_json(ROOT / "docs/components/component-registry.json")
        )
        cls.profiles = {cls.profile["id"]: cls.profile}
        cls.models = {cls.model["id"]: cls.model}

    def validate_model(self, model):
        return validator.validate_component_model(
            model,
            self.profiles,
            self.registry,
            ROOT,
        )

    def validate_suite(self, suite):
        return validator.validate_suite(
            suite,
            self.profiles,
            self.models,
            self.registry,
        )

    def test_complete_reference_tree_validates(self) -> None:
        self.assertEqual([], validator.validate_tree(ROOT))

    def test_button_seed_is_exact_cartesian_product(self) -> None:
        generated = generator.generate(self.model)
        self.assertEqual(50, len(generated["cases"]))
        keys = {
            (case["componentId"], case["state"], case["theme"])
            for case in generated["cases"]
        }
        self.assertEqual(50, len(keys))
        self.assertEqual(validator.expected_case_keys(generated), keys)
        self.assertEqual("full-model", generated["coverage"]["mode"])
        self.assertEqual(generated, self.suite)

    def test_filled_structure_vertical_is_complete_and_generated(self) -> None:
        generated = structure_generator.generate(self.model)
        self.assertEqual("evidence-backed", generated["status"])
        self.assertEqual("vertical-slice", generated["coverage"]["mode"])
        self.assertEqual(
            {"size": "small", "density": "default", "content": "label-only"},
            generated["fixture"],
        )
        self.assertEqual(10, len(generated["cases"]))
        self.assertEqual(
            90,
            sum(len(case["assertions"]) for case in generated["cases"]),
        )
        self.assertTrue(
            all(
                assertion["evidence"]
                for case in generated["cases"]
                for assertion in case["assertions"]
            )
        )
        self.assertEqual([], self.validate_suite(generated))
        self.assertEqual(generated, self.structure_suite)

    def test_filled_renderer_vertical_is_complete_and_generated(self) -> None:
        generated = renderer_generator.generate(self.model)
        self.assertEqual("evidence-backed", generated["status"])
        self.assertEqual("vertical-slice", generated["coverage"]["mode"])
        self.assertEqual(
            {"size": "small", "density": "default", "content": "label-only"},
            generated["fixture"],
        )
        self.assertEqual(10, len(generated["cases"]))
        self.assertEqual(
            100,
            sum(len(case["assertions"]) for case in generated["cases"]),
        )
        self.assertTrue(
            all(
                assertion["evidence"]
                for case in generated["cases"]
                for assertion in case["assertions"]
            )
        )
        self.assertEqual([], self.validate_suite(generated))
        self.assertEqual(generated, self.renderer_suite)

    def test_elevated_renderer_vertical_is_complete_and_generated(self) -> None:
        generated = elevated_renderer_generator.generate(self.model)
        self.assertEqual("evidence-backed", generated["status"])
        self.assertEqual("vertical-slice", generated["coverage"]["mode"])
        self.assertEqual(
            {"size": "small", "density": "default", "content": "label-only"},
            generated["fixture"],
        )
        self.assertEqual(["button.elevated"], generated["caseDimensions"]["components"])
        self.assertEqual(10, len(generated["cases"]))
        self.assertEqual(
            100,
            sum(len(case["assertions"]) for case in generated["cases"]),
        )
        self.assertTrue(
            all(
                assertion["evidence"]
                for case in generated["cases"]
                for assertion in case["assertions"]
            )
        )
        self.assertEqual([], self.validate_suite(generated))
        self.assertEqual(generated, self.elevated_renderer_suite)

    def test_filled_tonal_renderer_vertical_is_complete_and_generated(self) -> None:
        generated = filled_tonal_renderer_generator.generate(self.model)
        self.assertEqual("evidence-backed", generated["status"])
        self.assertEqual("vertical-slice", generated["coverage"]["mode"])
        self.assertEqual(
            {"size": "small", "density": "default", "content": "label-only"},
            generated["fixture"],
        )
        self.assertEqual(
            ["button.filled-tonal"],
            generated["caseDimensions"]["components"],
        )
        self.assertEqual(10, len(generated["cases"]))
        self.assertEqual(
            100,
            sum(len(case["assertions"]) for case in generated["cases"]),
        )
        self.assertTrue(
            all(
                assertion["evidence"]
                for case in generated["cases"]
                for assertion in case["assertions"]
            )
        )
        self.assertEqual([], self.validate_suite(generated))
        self.assertEqual(generated, self.filled_tonal_renderer_suite)

    def test_outlined_renderer_vertical_is_complete_and_generated(self) -> None:
        generated = outlined_renderer_generator.generate(self.model)
        self.assertEqual("evidence-backed", generated["status"])
        self.assertEqual("vertical-slice", generated["coverage"]["mode"])
        self.assertEqual(
            {"size": "small", "density": "default", "content": "label-only"},
            generated["fixture"],
        )
        self.assertEqual(
            ["button.outlined"],
            generated["caseDimensions"]["components"],
        )
        self.assertEqual(10, len(generated["cases"]))
        self.assertEqual(
            130,
            sum(len(case["assertions"]) for case in generated["cases"]),
        )
        self.assertTrue(
            all(
                assertion["evidence"]
                for case in generated["cases"]
                for assertion in case["assertions"]
            )
        )
        self.assertEqual([], self.validate_suite(generated))
        self.assertEqual(generated, self.outlined_renderer_suite)

    def test_text_renderer_vertical_is_complete_and_generated(self) -> None:
        generated = text_renderer_generator.generate(self.model)
        self.assertEqual("evidence-backed", generated["status"])
        self.assertEqual("vertical-slice", generated["coverage"]["mode"])
        self.assertEqual(
            {"size": "small", "density": "default", "content": "label-only"},
            generated["fixture"],
        )
        self.assertEqual(
            ["button.text"],
            generated["caseDimensions"]["components"],
        )
        self.assertEqual(10, len(generated["cases"]))
        self.assertEqual(
            100,
            sum(len(case["assertions"]) for case in generated["cases"]),
        )
        self.assertTrue(
            all(
                assertion["evidence"]
                for case in generated["cases"]
                for assertion in case["assertions"]
            )
        )
        self.assertEqual([], self.validate_suite(generated))
        self.assertEqual(generated, self.text_renderer_suite)

    def test_profiles_pin_current_classic_and_expressive_independently(self) -> None:
        profiles = {item["profile"]: item["id"] for item in self.catalog["profiles"]}
        self.assertEqual(
            "m3-classic.compose-material3-1.4.0", profiles["classic"]
        )
        self.assertEqual(
            "m3-expressive.compose-material3-1.5.0-alpha27",
            profiles["expressive"],
        )
        self.assertNotEqual(profiles["classic"], profiles["expressive"])

    def test_mutable_androidx_main_is_rejected(self) -> None:
        profile = copy.deepcopy(self.profile)
        implementation = next(
            source
            for source in profile["sources"]
            if source["kind"] == "androidx-implementation"
        )
        implementation["uri"] = (
            "https://android.googlesource.com/platform/frameworks/support/+/"
            "androidx-main/compose/material3/1.4.0"
        )
        errors = validator.validate_profile(profile)
        self.assertTrue(any("mutable androidx-main" in error for error in errors))

    def test_gitiles_evidence_source_is_revision_pinned(self) -> None:
        source = next(
            source
            for source in self.profile["sources"]
            if source["id"] == "androidx-material3-source-1.4.0"
        )
        self.assertEqual(40, len(source["revision"]))
        self.assertIn(source["revision"], source["uri"])
        profile = copy.deepcopy(self.profile)
        mutable_source = next(
            item
            for item in profile["sources"]
            if item["id"] == "androidx-material3-source-1.4.0"
        )
        del mutable_source["revision"]
        errors = validator.validate_profile(profile)
        self.assertTrue(
            any("Gitiles source requires an immutable revision" in error for error in errors)
        )

    def test_schema_rejects_unknown_profile_property(self) -> None:
        profile = copy.deepcopy(self.profile)
        profile["silentBypass"] = True
        schema = load_json(
            ROOT / "references/material3/schema/reference-profile.schema.json"
        )
        errors = validator.validate_json_schema(profile, schema)
        self.assertTrue(any("additional property is forbidden" in error for error in errors))

    def test_component_model_maps_every_button_spec_field_once(self) -> None:
        self.assertEqual([], self.validate_model(self.model))
        fields = validator.extract_struct_fields(
            (
                ROOT / self.model["qtBinding"]["resolvedSpecHeader"]
            ).read_text(encoding="utf-8"),
            self.model["qtBinding"]["resolvedSpecType"],
        )
        mapped = {
            path
            for role in self.model["tokenRoles"]
            for path in role["resolvedSpecPaths"]
        } | set(self.model["qtBinding"]["runtimeSupportFields"])
        self.assertEqual(40, len(fields))
        self.assertEqual(fields, mapped)

    def test_unknown_resolved_spec_field_is_rejected(self) -> None:
        model = copy.deepcopy(self.model)
        model["qtBinding"]["runtimeSupportFields"].append("futureField")
        errors = self.validate_model(model)
        self.assertTrue(any("unknown ResolvedSpec fields" in error for error in errors))

    def test_unmapped_resolved_spec_field_is_rejected(self) -> None:
        model = copy.deepcopy(self.model)
        model["qtBinding"]["runtimeSupportFields"].remove(
            "hasResolvedMotionStyle"
        )
        errors = self.validate_model(model)
        self.assertTrue(any("fields unmapped" in error for error in errors))

    def test_registry_variant_binding_is_mandatory(self) -> None:
        model = copy.deepcopy(self.model)
        model["variants"][0]["componentId"] = "button.future"
        errors = self.validate_model(model)
        self.assertTrue(
            any("missing from product registry" in error for error in errors)
        )

    def test_anchor_must_remain_reference_candidate(self) -> None:
        registry = copy.deepcopy(self.registry)
        registry["button.filled"]["referenceCandidate"] = False
        errors = validator.validate_component_model(
            self.model,
            self.profiles,
            registry,
            ROOT,
        )
        self.assertTrue(any("not referenceCandidate" in error for error in errors))

    def test_platform_adaptation_requires_rationale(self) -> None:
        model = copy.deepcopy(self.model)
        focus_role = next(
            role
            for role in model["tokenRoles"]
            if role["id"] == "focus-indicator.color"
        )
        del focus_role["rationale"]
        errors = self.validate_model(model)
        self.assertTrue(any("requires a rationale" in error for error in errors))

    def test_suite_dimensions_must_follow_model_required_axes(self) -> None:
        suite = copy.deepcopy(self.suite)
        suite["caseDimensions"]["states"] = ["enabled", "disabled"]
        errors = self.validate_suite(suite)
        self.assertTrue(
            any("state dimension drifts" in error for error in errors)
        )

    def test_vertical_slice_must_be_an_ordered_model_subset(self) -> None:
        suite = copy.deepcopy(self.structure_suite)
        suite["caseDimensions"]["components"] = ["button.future"]
        errors = self.validate_suite(suite)
        self.assertTrue(any("ordered subset" in error for error in errors))

    def test_case_id_must_be_canonical(self) -> None:
        suite = copy.deepcopy(self.suite)
        suite["cases"][0]["id"] = "classic.buttons.seed.noncanonical"
        errors = self.validate_suite(suite)
        self.assertTrue(any("case id is not canonical" in error for error in errors))

    def test_seeded_suite_cannot_hide_partial_assertions(self) -> None:
        suite = copy.deepcopy(self.suite)
        suite["cases"][0]["assertions"] = [
            {
                "id": "container-height",
                "domain": "geometry",
                "classification": "MUST_MATCH",
                "path": "containerHeight",
                "operator": "equals",
                "expected": 40,
                "sourceRefs": ["androidx-material3-1.4.0"],
            }
        ]
        errors = self.validate_suite(suite)
        self.assertTrue(
            any("cannot carry partial assertions" in error for error in errors)
        )

    def test_evidence_backed_suite_requires_every_case_to_have_evidence(self) -> None:
        suite = copy.deepcopy(self.suite)
        suite["status"] = "evidence-backed"
        errors = self.validate_suite(suite)
        self.assertTrue(
            any("cannot have zero assertions" in error for error in errors)
        )

    def test_evidence_backed_assertion_requires_immutable_locator(self) -> None:
        suite = copy.deepcopy(self.structure_suite)
        del suite["cases"][0]["assertions"][0]["evidence"]
        errors = self.validate_suite(suite)
        self.assertTrue(any("requires located evidence" in error for error in errors))

    def test_evidence_locator_source_must_be_in_assertion_refs(self) -> None:
        suite = copy.deepcopy(self.structure_suite)
        evidence = suite["cases"][0]["assertions"][0]["evidence"][0]
        evidence["sourceRef"] = "m3-buttons"
        errors = self.validate_suite(suite)
        self.assertTrue(
            any("absent from assertion sourceRefs" in error for error in errors)
        )

    def test_unknown_source_reference_is_rejected(self) -> None:
        suite = copy.deepcopy(self.suite)
        suite["sourceRefs"].append("missing-source")
        errors = self.validate_suite(suite)
        self.assertTrue(any("sourceRef missing" in error for error in errors))


if __name__ == "__main__":
    unittest.main()
