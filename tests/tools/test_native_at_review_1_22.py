from __future__ import annotations

import copy
import json
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

TOOLS = Path(__file__).resolve().parents[2] / "tools"
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))

import certification_center as center
import native_at_review_1_22 as native
import verify_visual_baselines_1_22 as visual


class NativeReviewTests(unittest.TestCase):
    def setUp(self):
        self.ledger = center.load_ledger("1.7")
        self.session = native.create_session("1.7", "windows-nvda", self.ledger)

    def completed_fixture(self):
        session = copy.deepcopy(self.session)
        session.update(reviewer="TEST fixture", reviewedAt="2026-11-18", evidence="test-only")
        session["environment"] = {key: "fixture" for key in native.METADATA}
        session["environment"]["sourceCommit"] = "a" * 40
        for cid, checks in session["checks"].items():
            for key in checks:
                checks[key] = "pass"
                session["observations"][cid][key] = "Synthetic observation for unit test only"
        return session

    def test_templates_never_claim_evidence(self):
        self.assertEqual(self.session["schemaVersion"], 2)
        self.assertEqual(self.session["environment"]["sourceCommit"], "")
        self.assertEqual(self.session["observations"]["data.badge"]["dotAnnouncement"], "")
        self.assertTrue(native.validate(self.session, self.ledger))

    def test_observations_and_versions_are_required(self):
        session = self.completed_fixture()
        self.assertEqual(native.validate(session, self.ledger), [])
        session["observations"]["data.badge"]["dotAnnouncement"] = ""
        session["environment"]["screenReaderVersion"] = ""
        errors = native.validate(session, self.ledger)
        self.assertTrue(any("dotAnnouncement" in error for error in errors))
        self.assertTrue(any("screenReaderVersion" in error for error in errors))

    def test_reject_contract_omission_and_partial_review(self):
        session = self.completed_fixture()
        del session["observations"]["data.badge"]["numericAnnouncement"]
        session["checks"]["surface.tooltip"]["announcement"] = "pending"
        errors = native.validate(session, self.ledger)
        self.assertTrue(any("observation keys" in error for error in errors))
        self.assertTrue(any("pending" in error for error in errors))

    def test_all_228_scenarios_export_and_protect_existing_files(self):
        with tempfile.TemporaryDirectory() as directory:
            output = Path(directory)
            self.assertEqual(native.prepare(output), 12)
            files = sorted(output.glob("*.json"))
            self.assertEqual(len(files), 12)
            self.assertEqual(len(list(output.glob("*.md"))), 12)
            count = 0
            for path in files:
                session = json.loads(path.read_text(encoding="utf-8"))
                for checks in session["checks"].values():
                    count += len(checks)
                    self.assertEqual(set(checks.values()), {"pending"})
            self.assertEqual(count, 228)
            original = files[0].read_bytes()
            with self.assertRaises(FileExistsError):
                native.prepare(output)
            self.assertEqual(files[0].read_bytes(), original)

    def test_record_keeps_raw_observations_and_reports_failure(self):
        session = self.completed_fixture()
        session["checks"]["data.badge"]["dotAnnouncement"] = "fail"
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            ledger_path = center.ledger_path("1.7", root)
            ledger_path.parent.mkdir(parents=True)
            center.save_ledger("1.7", self.ledger, root)
            with patch.object(native, "current_commit", return_value="b" * 40), \
                    patch.object(native.subprocess, "run"):
                self.assertEqual(native.record(session, root), "fail")
                with self.assertRaises(FileExistsError):
                    native.record(session, root)
            updated = center.load_ledger("1.7", root)["platforms"]["windows-nvda"]
            self.assertEqual(updated["status"], "fail")
            relative = updated["evidence"].split(";")[0]
            self.assertEqual(json.loads((root / relative).read_text()), session)


class VisualBaselineTests(unittest.TestCase):
    def test_canonical_pngs_match_pinned_provenance(self):
        result = visual.verify()
        self.assertEqual(sum(result["verifiedImages"].values()), 63)

    def test_reject_changed_png(self):
        checker, _ = visual.FAMILIES["missing-material3"]
        provenance = json.loads((visual.ROOT / "docs/release/visual-baselines-1.22.json").read_text())
        report = provenance["families"]["missing-material3"]
        with tempfile.TemporaryDirectory() as directory:
            folder = Path(directory)
            for name in checker.CASES:
                path = folder / (name + ".png")
                path.write_bytes((visual.ROOT / "tests/visual/goldens" / path.name).read_bytes())
            target = folder / (checker.CASES[0] + ".png")
            data = bytearray(target.read_bytes())
            data[32] ^= 1
            target.write_bytes(data)
            with self.assertRaisesRegex(ValueError, "SHA256 mismatch"):
                visual.verify_family(folder, report, checker, provenance["sourceCommit"])


if __name__ == "__main__":
    unittest.main()
