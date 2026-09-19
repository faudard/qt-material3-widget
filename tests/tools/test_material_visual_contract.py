from __future__ import annotations

import copy
import importlib.util
import json
import shutil
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

if not (ROOT / "tools/check_material_visual_contract.py").is_file():
    raise unittest.SkipTest("Material conformance tooling is not implemented yet: tools/check_material_visual_contract.py")


def load_module(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec is not None and spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


checker = load_module(
    "qtm3_material_visual_contract_tests",
    ROOT / "tools/check_material_visual_contract.py",
)
generator = load_module(
    "qtm3_material_visual_contract_generator_tests",
    ROOT / "tools/generate_material_button_visual_contract.py",
)
reference_checker = load_module(
    "qtm3_material_visual_manifest_schema_tests",
    ROOT / "tools/check_material_reference_model.py",
)


def load_json(path: Path):
    return json.loads(path.read_text(encoding="utf-8"))


class MaterialVisualContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.contract_path = ROOT / checker.CONTRACT_PATH
        cls.contract = load_json(cls.contract_path)
        cls.model = load_json(ROOT / checker.MODEL_PATH)
        cls.catalog = load_json(ROOT / checker.CATALOG_PATH)

    def validate(self, contract=None, model=None, catalog=None):
        return checker.validate_contract(
            self.contract if contract is None else contract,
            model=self.model if model is None else model,
            catalog=self.catalog if catalog is None else catalog,
            root=ROOT,
        )

    def test_complete_visual_contract_tree_passes(self) -> None:
        self.assertEqual([], checker.validate_tree(ROOT))

    def test_generated_contract_is_current_and_exhaustive(self) -> None:
        generated = generator.generate(self.model)
        self.assertEqual(generated, self.contract)
        self.assertEqual(
            generator.serialized(self.model), self.contract_path.read_text()
        )
        self.assertEqual(50, len(generated["cases"]))
        self.assertEqual(4, len(generated["targets"]))
        self.assertEqual(200, generated["expectedReferenceCount"])
        self.assertEqual(50, generated["expectedActualCount"])
        self.assertEqual(
            50,
            len({case["actualPath"] for case in generated["cases"]}),
        )
        self.assertEqual(
            200,
            sum(len(case["references"]) for case in generated["cases"]),
        )
        self.assertEqual(
            checker.LOCKED_COMPARISON_RUNNER,
            generated["comparisonRunner"],
        )
        self.assertEqual(
            ["pending"] * 4,
            [item["status"] for item in generated["referenceSets"]],
        )
        self.assertEqual(
            "rgba-independent",
            generated["comparisonPolicy"]["ssim"]["channelModel"],
        )

    def test_target_matrix_keeps_qt5_qt6_and_platforms_isolated(self) -> None:
        targets = self.contract["targets"]
        signatures = {
            (target["operatingSystem"], target["qtMajor"])
            for target in targets
        }
        self.assertIn(("windows", 5), signatures)
        self.assertIn(("windows", 6), signatures)
        self.assertIn(("linux", 6), signatures)
        self.assertIn(("macos", 6), signatures)
        roots = [target["referenceRoot"] for target in targets]
        self.assertEqual(len(roots), len(set(roots)))

    def test_unpinned_qt_target_is_rejected(self) -> None:
        changed = copy.deepcopy(self.contract)
        changed["targets"][0]["qtVersion"] = "5.14"
        errors = self.validate(changed)
        self.assertTrue(any("not fully pinned" in error for error in errors))
        self.assertTrue(any("target identities/order drift" in error for error in errors))

    def test_latest_runner_target_is_rejected(self) -> None:
        changed = copy.deepcopy(self.contract)
        changed["targets"][2]["runnerImage"] = "ubuntu-latest"
        errors = self.validate(changed)
        self.assertTrue(any("runner image is not pinned" in error for error in errors))

    def test_font_substitution_is_rejected(self) -> None:
        changed = copy.deepcopy(self.contract)
        changed["targets"][1]["font"]["substitutionsAllowed"] = True
        errors = self.validate(changed)
        self.assertTrue(any("font substitutionsAllowed" in error for error in errors))

    def test_font_contract_uses_points_and_face_tables(self) -> None:
        for target in self.contract["targets"]:
            font = target["font"]
            self.assertEqual(14, font["pointSize"])
            self.assertTrue(font["faceFingerprintRequired"])
            self.assertNotIn("pixelSize", font)
            self.assertNotIn("fileFingerprintRequired", font)
        fields = self.contract["environment"]["runtimeFingerprint"]["fields"]
        self.assertIn("font-face-sha256", fields)
        self.assertIn("font-point-size", fields)
        self.assertIn("logical-dpi-x", fields)
        self.assertNotIn("font-file-sha256", fields)

    def test_runtime_tolerance_is_rejected(self) -> None:
        changed = copy.deepcopy(self.contract)
        changed["comparisonPolicy"]["pixel"]["maxDifferentPixels"] = 1
        errors = self.validate(changed)
        self.assertTrue(any("zero-tolerance mode" in error for error in errors))
        self.assertTrue(any("value must equal schema const" in error for error in errors))

    def test_reference_writing_capture_mode_is_rejected(self) -> None:
        changed = copy.deepcopy(self.contract)
        changed["captureRunner"]["referenceWritesAllowed"] = True
        errors = self.validate(changed)
        self.assertTrue(any("actual-only fail-closed" in error for error in errors))
        self.assertTrue(any("value must equal schema const" in error for error in errors))

    def test_reference_writing_comparison_mode_is_rejected(self) -> None:
        changed = copy.deepcopy(self.contract)
        changed["comparisonRunner"]["referenceWritesAllowed"] = True
        errors = self.validate(changed)
        self.assertTrue(any("read-only fail-closed" in error for error in errors))
        self.assertTrue(any("value must equal schema const" in error for error in errors))

    def test_contract_only_reference_set_cannot_be_marked_reviewed(self) -> None:
        changed = copy.deepcopy(self.contract)
        changed["referenceSets"][0]["status"] = "reviewed"
        errors = self.validate(changed)
        self.assertTrue(any("reviewed reference-set count" in error for error in errors))
        self.assertTrue(any("must all be pending" in error for error in errors))

    def test_state_phase_drift_is_rejected(self) -> None:
        changed = copy.deepcopy(self.contract)
        pressed = next(
            setup for setup in changed["stateSetups"] if setup["id"] == "pressed"
        )
        pressed["feedback"]["progress"] = 0.6
        errors = self.validate(changed)
        self.assertTrue(any("deterministic fixed phases" in error for error in errors))

    def test_missing_case_is_rejected(self) -> None:
        changed = copy.deepcopy(self.contract)
        changed["cases"].pop()
        errors = self.validate(changed)
        self.assertTrue(any("Cartesian case matrix/order drift" in error for error in errors))
        self.assertTrue(any("reference inventory" in error for error in errors))
        self.assertTrue(any("actual inventory" in error for error in errors))

    def test_noncanonical_actual_path_is_rejected(self) -> None:
        changed = copy.deepcopy(self.contract)
        changed["cases"][0]["actualPath"] = "../reference.png"
        errors = self.validate(changed)
        self.assertTrue(any("actual path is not canonical" in error for error in errors))

    def test_cross_target_reference_substitution_is_rejected(self) -> None:
        changed = copy.deepcopy(self.contract)
        references = changed["cases"][0]["references"]
        references[0]["targetId"] = references[1]["targetId"]
        errors = self.validate(changed)
        self.assertTrue(any("target coverage/order mismatch" in error for error in errors))
        self.assertTrue(any("reference path is not canonical" in error for error in errors))

    def test_non_evidence_backed_renderer_suite_is_rejected(self) -> None:
        changed = copy.deepcopy(self.catalog)
        suite = next(
            item
            for item in changed["suites"]
            if item["id"] == "classic.buttons.text.renderer"
        )
        suite["status"] = "seeded"
        errors = self.validate(catalog=changed)
        self.assertTrue(any("not evidence-backed" in error for error in errors))

    def test_visual_contract_must_be_catalogued_exactly_once(self) -> None:
        changed = copy.deepcopy(self.catalog)
        changed["visualContracts"] = []
        errors = self.validate(catalog=changed)
        self.assertTrue(any("must bind classic.buttons.visual" in error for error in errors))

    def test_schema_rejects_unknown_contract_fields(self) -> None:
        changed = copy.deepcopy(self.contract)
        changed["optionalTolerance"] = 3
        errors = self.validate(changed)
        self.assertTrue(any("additional property is forbidden" in error for error in errors))

    def test_malformed_case_axis_reports_an_error_without_crashing(self) -> None:
        changed = copy.deepcopy(self.contract)
        changed["cases"][0]["componentId"] = []
        errors = self.validate(changed)
        self.assertTrue(any("case axes must be strings" in error for error in errors))

    def test_contract_only_status_rejects_premature_reference_files(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            target = self.contract["targets"][0]
            image = root / target["referenceRoot"] / "unexpected.png"
            image.parent.mkdir(parents=True)
            image.write_bytes(b"not-a-reference")
            errors = checker._validate_contract_only_inventory(root, self.contract)
        self.assertTrue(any("premature reference" in error for error in errors))

    def test_capture_runner_has_no_bypass_or_update_mode(self) -> None:
        source = (ROOT / checker.CAPTURE_MAIN_PATH).read_text(
            encoding="utf-8"
        )
        for forbidden in (
            "QTMATERIAL3_VISUAL_STRICT",
            "QTMATERIAL3_VISUAL_MAX_DIFF",
            "QTMATERIAL3_UPDATE_VISUAL_GOLDENS",
            "QSKIP",
            "--allow-missing",
            "--update",
            "--tolerance",
        ):
            self.assertNotIn(forbidden, source)

    def test_capture_runner_static_contract_passes(self) -> None:
        self.assertEqual([], checker.validate_capture_runner_sources(ROOT))

    def test_visual_comparator_static_contract_passes(self) -> None:
        self.assertEqual([], checker.validate_comparator_source(ROOT))

    def test_visual_comparator_tolerance_option_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            path = root / checker.COMPARATOR_PATH
            path.parent.mkdir(parents=True, exist_ok=True)
            source = (ROOT / checker.COMPARATOR_PATH).read_text(encoding="utf-8")
            path.write_text(source + "\n# --tolerance 1\n", encoding="utf-8")
            errors = checker.validate_comparator_source(root)
        self.assertTrue(any("runtime tolerance option" in error for error in errors))

    def validate_sources_with_mutation(
        self,
        relative: Path,
        mutate,
    ):
        paths = (
            checker.CAPTURE_MAIN_PATH,
            checker.CAPTURE_CONTROLLER_HEADER_PATH,
            checker.CAPTURE_CONTROLLER_SOURCE_PATH,
            checker.CAPTURE_EFFECTS_ACCESS_PATH,
            checker.TESTS_CMAKE_PATH,
            checker.RIPPLE_HEADER_PATH,
            checker.RIPPLE_SOURCE_PATH,
            checker.TRANSITION_HEADER_PATH,
            checker.TEXT_BUTTON_SOURCE_PATH,
            checker.FILLED_BUTTON_SOURCE_PATH,
        )
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for path in paths:
                target = root / path
                target.parent.mkdir(parents=True, exist_ok=True)
                shutil.copyfile(ROOT / path, target)
            path = root / relative
            path.write_text(mutate(path.read_text(encoding="utf-8")), encoding="utf-8")
            return checker.validate_capture_runner_sources(root)

    def test_event_loop_pumping_mutation_is_rejected(self) -> None:
        errors = self.validate_sources_with_mutation(
            checker.CAPTURE_MAIN_PATH,
            lambda source: source + "\nQCoreApplication::processEvents();\n",
        )
        self.assertTrue(any("event-loop pumping" in error for error in errors))

    def test_reference_write_guard_mutation_is_rejected(self) -> None:
        errors = self.validate_sources_with_mutation(
            checker.CAPTURE_MAIN_PATH,
            lambda source: source.replace(
                "isSameOrChildPath(candidate, referenceRoot)",
                "candidate == referenceRoot",
            ),
        )
        self.assertTrue(any("isSameOrChildPath" in error for error in errors))

    def test_fixed_ripple_mutation_is_rejected(self) -> None:
        errors = self.validate_sources_with_mutation(
            checker.CAPTURE_EFFECTS_ACCESS_PATH,
            lambda source: source.replace("fixedProgress", "clockProgress"),
        )
        self.assertTrue(any("fixedProgress" in error for error in errors))

    def test_capture_cmake_wiring_mutation_is_rejected(self) -> None:
        errors = self.validate_sources_with_mutation(
            checker.TESTS_CMAKE_PATH,
            lambda source: source.replace(
                "qtmaterial3_material_button_capture",
                "removed_material_button_capture",
            ),
        )
        self.assertTrue(any("capture CMake wiring" in error for error in errors))

    def test_capture_cannot_claim_a_ctest_pass(self) -> None:
        errors = self.validate_sources_with_mutation(
            checker.TESTS_CMAKE_PATH,
            lambda source: source
            + "\nadd_test(NAME qtmaterial3_material_button_capture "
            + "COMMAND qtmaterial3_material_button_capture)\n",
        )
        self.assertTrue(any("must not be registered" in error for error in errors))

    def test_comparator_cannot_be_registered_before_references_exist(self) -> None:
        errors = self.validate_sources_with_mutation(
            checker.TESTS_CMAKE_PATH,
            lambda source: source
            + "\nadd_test(NAME material_visual_compare "
            + "COMMAND python compare_material_visual_capture.py)\n",
        )
        self.assertTrue(
            any("comparator must not be registered" in error for error in errors)
        )

    def test_visual_comparator_reference_path_option_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            path = root / checker.COMPARATOR_PATH
            path.parent.mkdir(parents=True, exist_ok=True)
            source = (ROOT / checker.COMPARATOR_PATH).read_text(encoding="utf-8")
            path.write_text(
                source + "\nparser.add_argument('--reference')\n",
                encoding="utf-8",
            )
            errors = checker.validate_comparator_source(root)
        self.assertTrue(any("caller-selected reference path" in error for error in errors))

    def test_capture_manifest_schema_is_closed(self) -> None:
        schema = load_json(ROOT / checker.CAPTURE_MANIFEST_SCHEMA_PATH)
        values = {
            "runner-image": "ubuntu-24.04",
            "operating-system": "linux",
            "operating-system-version": "Ubuntu 24.04",
            "architecture": "x86_64",
            "qt-version": "6.8.4",
            "qt-build-abi": "x86_64-little_endian-lp64",
            "compiler": "gcc-13",
            "qpa-platform": "offscreen",
            "application-style": "Fusion",
            "font-family": "DejaVu Sans",
            "font-face-sha256": "a" * 64,
            "font-style": "Book",
            "font-weight": 600,
            "font-point-size": 14.0,
            "device-pixel-ratio": 1.0,
            "logical-dpi-x": 96.0,
            "logical-dpi-y": 96.0,
        }
        manifest = {
            "schemaVersion": 1,
            "contractId": "classic.buttons.visual",
            "contractSha256": "b" * 64,
            "targetId": "ubuntu-24.04.qt6.8.4.gcc-13",
            "captureMode": "actual-only",
            "runtimeFingerprint": {
                "algorithm": "sha256",
                "digest": "c" * 64,
                "values": values,
            },
            "caseCount": 1,
            "cases": [
                {
                    "id": "classic.buttons.visual.button.text.enabled.light",
                    "componentId": "button.text",
                    "state": "enabled",
                    "theme": "light",
                    "actualPath": "actual/button.text/enabled/light.png",
                    "width": 160,
                    "height": 80,
                    "pixelSha256": "d" * 64,
                }
            ],
        }
        self.assertEqual([], reference_checker.validate_json_schema(manifest, schema))
        changed = copy.deepcopy(manifest)
        changed["comparisonPassed"] = True
        errors = reference_checker.validate_json_schema(changed, schema)
        self.assertTrue(any("additional property is forbidden" in error for error in errors))

    def test_reference_manifest_schema_is_closed(self) -> None:
        schema = load_json(ROOT / checker.REFERENCE_MANIFEST_SCHEMA_PATH)
        values = {
            "runner-image": "ubuntu-24.04",
            "operating-system": "linux",
            "operating-system-version": "Ubuntu 24.04",
            "architecture": "x86_64",
            "qt-version": "6.8.4",
            "qt-build-abi": "x86_64-little_endian-lp64",
            "compiler": "gcc-13",
            "qpa-platform": "offscreen",
            "application-style": "Fusion",
            "font-family": "DejaVu Sans",
            "font-face-sha256": "a" * 64,
            "font-style": "Book",
            "font-weight": 600,
            "font-point-size": 14.0,
            "device-pixel-ratio": 1.0,
            "logical-dpi-x": 96.0,
            "logical-dpi-y": 96.0,
        }
        manifest = {
            "schemaVersion": 1,
            "contractId": "classic.buttons.visual",
            "contractSha256": "b" * 64,
            "targetId": "ubuntu-24.04.qt6.8.4.gcc-13",
            "referenceMode": "reviewed",
            "sourceCaptureManifestSha256": "c" * 64,
            "runtimeFingerprint": {
                "algorithm": "sha256",
                "digest": "d" * 64,
                "values": values,
            },
            "caseCount": 1,
            "cases": [
                {
                    "id": "classic.buttons.visual.button.text.enabled.light",
                    "componentId": "button.text",
                    "state": "enabled",
                    "theme": "light",
                    "referencePath": "button.text/enabled/light.png",
                    "width": 160,
                    "height": 80,
                    "pixelSha256": "e" * 64,
                }
            ],
        }
        self.assertEqual([], reference_checker.validate_json_schema(manifest, schema))
        changed = copy.deepcopy(manifest)
        changed["reviewNote"] = "not part of the machine contract"
        errors = reference_checker.validate_json_schema(changed, schema)
        self.assertTrue(any("additional property is forbidden" in error for error in errors))

    def test_comparison_report_schema_is_closed_and_bounds_ssim(self) -> None:
        schema = load_json(ROOT / checker.COMPARISON_REPORT_SCHEMA_PATH)
        case = {
            "id": "classic.buttons.visual.button.text.enabled.light",
            "componentId": "button.text",
            "state": "enabled",
            "theme": "light",
            "status": "pass",
            "actualPath": "actual/button.text/enabled/light.png",
            "referencePath": "button.text/enabled/light.png",
            "width": 160,
            "height": 80,
            "actualPixelSha256": "a" * 64,
            "referencePixelSha256": "a" * 64,
            "differentPixels": 0,
            "maxChannelDelta": 0,
            "ssim": 1.0,
            "failureKinds": [],
            "actualArtifactPath": "",
            "diffArtifactPath": "",
            "metricsArtifactPath": "",
        }
        report = {
            "schemaVersion": 1,
            "contractId": "classic.buttons.visual",
            "contractSha256": "b" * 64,
            "targetId": "ubuntu-24.04.qt6.8.4.gcc-13",
            "status": "pass",
            "actualManifestSha256": "c" * 64,
            "referenceManifestSha256": "d" * 64,
            "caseCount": 1,
            "passedCaseCount": 1,
            "failedCaseCount": 0,
            "cases": [case],
        }
        self.assertEqual([], reference_checker.validate_json_schema(report, schema))
        changed = copy.deepcopy(report)
        changed["cases"][0]["ssim"] = 1.1
        errors = reference_checker.validate_json_schema(changed, schema)
        self.assertTrue(any("above maximum" in error for error in errors))
        changed = copy.deepcopy(report)
        changed["optionalTolerance"] = 1
        errors = reference_checker.validate_json_schema(changed, schema)
        self.assertTrue(any("additional property is forbidden" in error for error in errors))


if __name__ == "__main__":
    unittest.main()
