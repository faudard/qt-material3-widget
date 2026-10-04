import importlib.util
import json
from pathlib import Path
import struct
import tempfile
import unittest
import zlib

ROOT = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location("navigation_repeatability", ROOT / "tools/check_navigation_visual_repeatability.py")
evidence = importlib.util.module_from_spec(spec)
spec.loader.exec_module(evidence)


def image(red=0):
    def chunk(kind, content):
        return struct.pack(">I", len(content)) + kind + content + struct.pack(">I", zlib.crc32(kind + content))
    return (b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", 1, 1, 8, 6, 0, 0, 0))
            + chunk(b"IDAT", zlib.compress(bytes((0, red, 0, 0, 255)))) + chunk(b"IEND", b""))


class NavigationVisualEvidenceTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.first, self.second = (Path(self.temp.name) / name for name in ("first", "second"))
        for directory in (self.first, self.second):
            (directory / "goldens").mkdir(parents=True)
            (directory / "artifacts").mkdir()
            entries = []
            for case in evidence.CASES:
                (directory / "goldens" / (case + ".png")).write_bytes(image())
                entries.append({**evidence.PROFILE, "name": case, "sourceCommit": "a" * 40,
                                "width": 1, "height": 1, "sha256": "b" * 64})
            (directory / "artifacts/manifest.json").write_text(json.dumps(entries))

    def change_records(self, change):
        path = self.second / "artifacts/manifest.json"
        records = json.loads(path.read_text())
        change(records)
        path.write_text(json.dumps(records))

    def test_identical_images_remain_pending_visual_review(self):
        report = evidence.compare(self.first, self.second)
        self.assertEqual(len(report["images"]), 27)
        self.assertTrue(report["repeatable"])
        self.assertEqual(report["visualReview"], "pending")

    def test_pixel_change_is_rejected(self):
        (self.second / "goldens" / (evidence.CASES[0] + ".png")).write_bytes(image(255))
        with self.assertRaisesRegex(ValueError, "passes differ"):
            evidence.compare(self.first, self.second)

    def test_missing_case_is_rejected(self):
        self.change_records(lambda records: records.pop())
        with self.assertRaisesRegex(ValueError, "Missing render"):
            evidence.compare(self.first, self.second)

    def test_duplicate_case_is_rejected(self):
        self.change_records(lambda records: records.append(records[0]))
        with self.assertRaisesRegex(ValueError, "Duplicate render"):
            evidence.compare(self.first, self.second)

    def test_wrong_renderer_is_rejected(self):
        self.change_records(lambda records: records[0].update(qtVersion="6.8.3"))
        with self.assertRaisesRegex(ValueError, "qtVersion"):
            evidence.compare(self.first, self.second)

    def test_different_source_commit_is_rejected(self):
        self.change_records(lambda records: records[0].update(sourceCommit="c" * 40))
        with self.assertRaisesRegex(ValueError, "different source"):
            evidence.compare(self.first, self.second)

    def test_invalid_png_is_rejected(self):
        (self.second / "goldens" / (evidence.CASES[0] + ".png")).write_bytes(b"not a PNG")
        with self.assertRaisesRegex(ValueError, "invalid PNG"):
            evidence.compare(self.first, self.second)

    def test_dimension_provenance_is_checked(self):
        self.change_records(lambda records: records[0].update(width=2))
        with self.assertRaisesRegex(ValueError, "dimensions disagree"):
            evidence.compare(self.first, self.second)
