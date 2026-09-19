from __future__ import annotations

import copy
import hashlib
import importlib.util
import json
import shutil
import struct
import sys
import tempfile
import unittest
import zlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

if not (ROOT / "tools/compare_material_visual_capture.py").is_file():
    raise unittest.SkipTest("Material conformance tooling is not implemented yet: tools/compare_material_visual_capture.py")


def load_module(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec is not None and spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


comparator = load_module(
    "qtm3_material_visual_comparator_tests",
    ROOT / "tools/compare_material_visual_capture.py",
)
generator = load_module(
    "qtm3_material_visual_comparator_generator_tests",
    ROOT / "tools/generate_material_button_visual_contract.py",
)


def serialized(value) -> bytes:
    return (json.dumps(value, indent=2, ensure_ascii=False) + "\n").encode("utf-8")


def pixel_hash(image) -> str:
    return hashlib.sha256(image.pixels).hexdigest()


def png_with_row_filters(image, filters) -> bytes:
    stride = image.width * 4
    previous = bytes(stride)
    rows = []
    for row, filter_kind in enumerate(filters):
        current = image.pixels[row * stride : (row + 1) * stride]
        encoded = bytearray(stride)
        for index, value in enumerate(current):
            left = current[index - 4] if index >= 4 else 0
            up = previous[index]
            upper_left = previous[index - 4] if index >= 4 else 0
            predictors = (
                0,
                left,
                up,
                (left + up) // 2,
                comparator._paeth(left, up, upper_left),
            )
            encoded[index] = (value - predictors[filter_kind]) & 0xFF
        rows.append(bytes((filter_kind,)) + encoded)
        previous = current
    compressed = zlib.compress(b"".join(rows), level=9)
    split = max(1, len(compressed) // 2)
    header = struct.pack(
        ">IIBBBBB",
        image.width,
        image.height,
        8,
        6,
        0,
        0,
        0,
    )
    return b"".join(
        (
            comparator.PNG_SIGNATURE,
            comparator._png_chunk(b"IHDR", header),
            comparator._png_chunk(b"IDAT", compressed[:split]),
            comparator._png_chunk(b"IDAT", compressed[split:]),
            comparator._png_chunk(b"IEND", b""),
        )
    )


class ComparatorFixture:
    def __init__(self, directory: str, *, mismatch: bool = False) -> None:
        self.root = Path(directory)
        self.actual_root = self.root / "capture"
        self.reference_root = self.root / "reference-set"
        self.artifacts = self.root / "artifacts"
        self.contract_path = self.root / "contract.json"
        self._copy_runtime_files()

        generated = generator.generate()
        target = copy.deepcopy(generated["targets"][0])
        target["referenceRoot"] = "reference-set"
        case = copy.deepcopy(generated["cases"][0])
        case["references"] = [
            {
                "targetId": target["id"],
                "path": "reference-set/button.text/enabled/light.png",
            }
        ]
        self.contract = generated
        self.contract["status"] = "single-target-active"
        self.contract["targets"] = [target]
        self.contract["referenceSets"] = [
            {
                "targetId": target["id"],
                "status": "reviewed",
                "manifestPath": "reference-set/manifest.json",
            }
        ]
        self.contract["cases"] = [case]
        self.contract["caseDimensions"] = {
            "components": ["button.text"],
            "states": ["enabled"],
            "themes": ["light"],
            "targets": [target["id"]],
        }
        self.contract["expectedActualCount"] = 1
        self.contract["expectedReferenceCount"] = 1
        self.contract["imageNormalization"]["logicalCanvas"] = {
            "width": 16,
            "height": 8,
        }
        self.target = target

        reference_pixels = bytes((25, 50, 75, 255)) * (16 * 8)
        actual_pixels = bytearray(reference_pixels)
        if mismatch:
            actual_pixels[0] = 26
        self.reference_image = comparator.RgbaImage(16, 8, reference_pixels)
        self.actual_image = comparator.RgbaImage(16, 8, bytes(actual_pixels))
        self.write_all()

    def _copy_runtime_files(self) -> None:
        validator_target = self.root / comparator.SCHEMA_VALIDATOR_PATH
        validator_target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(ROOT / comparator.SCHEMA_VALIDATOR_PATH, validator_target)
        for schema in (
            comparator.CONTRACT_SCHEMA_PATH,
            comparator.CAPTURE_MANIFEST_SCHEMA_PATH,
            comparator.REFERENCE_MANIFEST_SCHEMA_PATH,
            comparator.REPORT_SCHEMA_PATH,
        ):
            target = self.root / schema
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(ROOT / schema, target)

    def runtime_fingerprint(self, operating_system_version: str = "Windows 2022"):
        values = {
            "runner-image": self.target["runnerImage"],
            "operating-system": self.target["operatingSystem"],
            "operating-system-version": operating_system_version,
            "architecture": self.target["architecture"],
            "qt-version": self.target["qtVersion"],
            "qt-build-abi": "x86_64-little_endian-llp64",
            "compiler": self.target["compiler"],
            "qpa-platform": "offscreen",
            "application-style": "Fusion",
            "font-family": self.target["font"]["family"],
            "font-face-sha256": "a" * 64,
            "font-style": "Regular",
            "font-weight": 50,
            "font-point-size": 14,
            "device-pixel-ratio": 1,
            "logical-dpi-x": 96,
            "logical-dpi-y": 96,
        }
        values = {key: values[key] for key in sorted(values)}
        return {
            "algorithm": "sha256",
            "digest": comparator._sha256(comparator._canonical_json_bytes(values)),
            "values": values,
        }

    def write_all(self) -> None:
        contract_bytes = serialized(self.contract)
        self.contract_path.write_bytes(contract_bytes)
        contract_hash = hashlib.sha256(contract_bytes).hexdigest()
        case = self.contract["cases"][0]

        actual_path = self.actual_root / case["actualPath"]
        actual_path.parent.mkdir(parents=True, exist_ok=True)
        actual_path.write_bytes(comparator.encode_normalized_png(self.actual_image))
        reference_path = self.reference_root / "button.text/enabled/light.png"
        reference_path.parent.mkdir(parents=True, exist_ok=True)
        reference_path.write_bytes(
            comparator.encode_normalized_png(self.reference_image)
        )

        actual_manifest = {
            "schemaVersion": 1,
            "contractId": self.contract["id"],
            "contractSha256": contract_hash,
            "targetId": self.target["id"],
            "captureMode": "actual-only",
            "runtimeFingerprint": self.runtime_fingerprint(),
            "caseCount": 1,
            "cases": [
                {
                    "id": case["id"],
                    "componentId": case["componentId"],
                    "state": case["state"],
                    "theme": case["theme"],
                    "actualPath": case["actualPath"],
                    "width": 16,
                    "height": 8,
                    "pixelSha256": pixel_hash(self.actual_image),
                }
            ],
        }
        actual_manifest_bytes = serialized(actual_manifest)
        (self.actual_root / "manifest.json").write_bytes(actual_manifest_bytes)
        reference_manifest = {
            "schemaVersion": 1,
            "contractId": self.contract["id"],
            "contractSha256": contract_hash,
            "targetId": self.target["id"],
            "referenceMode": "reviewed",
            "sourceCaptureManifestSha256": hashlib.sha256(
                actual_manifest_bytes
            ).hexdigest(),
            "runtimeFingerprint": self.runtime_fingerprint(),
            "caseCount": 1,
            "cases": [
                {
                    "id": case["id"],
                    "componentId": case["componentId"],
                    "state": case["state"],
                    "theme": case["theme"],
                    "referencePath": "button.text/enabled/light.png",
                    "width": 16,
                    "height": 8,
                    "pixelSha256": pixel_hash(self.reference_image),
                }
            ],
        }
        (self.reference_root / "manifest.json").write_bytes(
            serialized(reference_manifest)
        )

    def compare(self):
        return comparator.compare_capture(
            root=self.root,
            contract_path=self.contract_path,
            target_id=self.target["id"],
            actual_root=self.actual_root,
            artifacts=self.artifacts,
        )


class MaterialVisualComparatorTests(unittest.TestCase):
    def test_decoder_reconstructs_all_png_row_filters_and_split_idat(self) -> None:
        width = 8
        height = 5
        pixels = bytes(
            (index * 37 + channel * 53) % 256
            for index in range(width * height)
            for channel in range(4)
        )
        image = comparator.RgbaImage(width, height, pixels)
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "filtered.png"
            path.write_bytes(png_with_row_filters(image, (0, 1, 2, 3, 4)))
            decoded = comparator.decode_normalized_png(
                path,
                expected_width=width,
                expected_height=height,
            )
        self.assertEqual(image, decoded)

    def test_identical_capture_passes_all_metrics(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            fixture = ComparatorFixture(directory)
            report = fixture.compare()
            self.assertEqual("pass", report["status"])
            self.assertEqual(1, report["passedCaseCount"])
            self.assertEqual(0, report["failedCaseCount"])
            result = report["cases"][0]
            self.assertEqual(0, result["differentPixels"])
            self.assertEqual(0, result["maxChannelDelta"])
            self.assertEqual(1.0, result["ssim"])
            self.assertEqual([], result["failureKinds"])
            self.assertTrue((fixture.artifacts / "comparison-report.json").is_file())
            self.assertFalse((fixture.artifacts / "failures").exists())

    def test_pixel_mismatch_fails_and_emits_three_diagnostics(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            fixture = ComparatorFixture(directory, mismatch=True)
            report = fixture.compare()
            self.assertEqual("fail", report["status"])
            result = report["cases"][0]
            self.assertEqual(1, result["differentPixels"])
            self.assertEqual(1, result["maxChannelDelta"])
            self.assertLess(result["ssim"], 1.0)
            self.assertEqual(
                ["pixel-count", "channel-delta", "ssim"],
                result["failureKinds"],
            )
            for key in (
                "actualArtifactPath",
                "diffArtifactPath",
                "metricsArtifactPath",
            ):
                self.assertTrue((fixture.artifacts / result[key]).is_file())

    def test_missing_reference_is_rejected_before_artifact_creation(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            fixture = ComparatorFixture(directory)
            (fixture.reference_root / "button.text/enabled/light.png").unlink()
            with self.assertRaisesRegex(comparator.ComparisonError, "inventory mismatch"):
                fixture.compare()
            self.assertFalse(fixture.artifacts.exists())

    def test_unexpected_reference_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            fixture = ComparatorFixture(directory)
            (fixture.reference_root / "unexpected.png").write_bytes(b"unexpected")
            with self.assertRaisesRegex(comparator.ComparisonError, "unexpected"):
                fixture.compare()

    def test_runtime_environment_mismatch_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            fixture = ComparatorFixture(directory)
            path = fixture.reference_root / "manifest.json"
            manifest = json.loads(path.read_text(encoding="utf-8"))
            manifest["runtimeFingerprint"] = fixture.runtime_fingerprint(
                "Different Windows image"
            )
            path.write_bytes(serialized(manifest))
            with self.assertRaisesRegex(
                comparator.ComparisonError,
                "runtime fingerprints differ",
            ):
                fixture.compare()

    def test_manifest_pixel_hash_mismatch_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            fixture = ComparatorFixture(directory)
            path = fixture.actual_root / "manifest.json"
            manifest = json.loads(path.read_text(encoding="utf-8"))
            manifest["cases"][0]["pixelSha256"] = "f" * 64
            path.write_bytes(serialized(manifest))
            with self.assertRaisesRegex(comparator.ComparisonError, "pixel hash mismatch"):
                fixture.compare()

    def test_png_ancillary_metadata_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            fixture = ComparatorFixture(directory)
            path = fixture.actual_root / "actual/button.text/enabled/light.png"
            png = path.read_bytes()
            png = png[:33] + comparator._png_chunk(b"tEXt", b"key\x00value") + png[33:]
            path.write_bytes(png)
            with self.assertRaisesRegex(
                comparator.ComparisonError,
                "metadata or unsupported chunk",
            ):
                fixture.compare()

    def test_pending_reference_set_cannot_be_compared(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            fixture = ComparatorFixture(directory)
            fixture.contract["status"] = "contract-only"
            fixture.contract["referenceSets"][0]["status"] = "pending"
            fixture.contract_path.write_bytes(serialized(fixture.contract))
            with self.assertRaisesRegex(comparator.ComparisonError, "no reviewed"):
                fixture.compare()

    def test_artifacts_inside_reference_root_are_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            fixture = ComparatorFixture(directory)
            fixture.artifacts = fixture.reference_root / "diagnostics"
            with self.assertRaisesRegex(
                comparator.ComparisonError,
                "outside every reference root",
            ):
                fixture.compare()

    def test_non_tiled_ssim_dimensions_are_rejected(self) -> None:
        first = comparator.RgbaImage(9, 8, bytes((0, 0, 0, 255)) * 72)
        second_pixels = bytearray(first.pixels)
        second_pixels[0] = 1
        second = comparator.RgbaImage(9, 8, bytes(second_pixels))
        with self.assertRaisesRegex(comparator.ComparisonError, "exact window-size"):
            comparator.structural_similarity(
                first,
                second,
                comparator.SUPPORTED_COMPARISON_POLICY["ssim"],
            )


if __name__ == "__main__":
    unittest.main()
