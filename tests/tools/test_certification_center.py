from __future__ import annotations

import copy
import sys
import unittest
from pathlib import Path

TOOLS = Path(__file__).resolve().parents[2] / "tools"
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))

import certification_center as center


def sample_ledger() -> dict:
    return {
        "schemaVersion": 1,
        "platforms": {
            "windows-nvda": {
                "screenReader": "NVDA",
                "status": "pending",
                "reviewer": "",
                "reviewedAt": "",
                "evidence": "",
                "notes": "",
                "components": {
                    "navigation.rail": {
                        "traversal": "pending",
                        "activation": "pending",
                    },
                    "navigation.tabs": {
                        "traversal": "pending",
                        "activation": "pending",
                    },
                },
            }
        },
    }


class CertificationCenterTests(unittest.TestCase):
    def test_derive_platform_status(self) -> None:
        self.assertEqual(
            center.derive_platform_status(
                {"a": {"x": "pass"}, "b": {"y": "pass"}}
            ),
            "pass",
        )
        self.assertEqual(
            center.derive_platform_status(
                {"a": {"x": "pass"}, "b": {"y": "fail"}}
            ),
            "fail",
        )
        self.assertEqual(
            center.derive_platform_status(
                {"a": {"x": "pass"}, "b": {"y": "pending"}}
            ),
            "pending",
        )

    def test_template_is_fail_closed_and_matches_contract(self) -> None:
        template = center.build_session_template(
            "1.5",
            "windows-nvda",
            sample_ledger(),
        )
        self.assertEqual(template["milestone"], "1.5")
        self.assertEqual(template["platform"], "windows-nvda")
        self.assertEqual(template["screenReader"], "NVDA")
        self.assertEqual(
            template["checks"],
            {
                "navigation.rail": {
                    "traversal": "pending",
                    "activation": "pending",
                },
                "navigation.tabs": {
                    "traversal": "pending",
                    "activation": "pending",
                },
            },
        )
        self.assertEqual(template["reviewer"], "")
        self.assertEqual(template["evidence"], "")

    def test_validate_session_rejects_pending_and_missing_metadata(self) -> None:
        payload = sample_ledger()
        session = center.build_session_template(
            "1.5",
            "windows-nvda",
            payload,
        )
        errors = center.validate_session(
            session,
            payload,
            require_final_results=True,
        )
        self.assertTrue(any("pending" in error for error in errors))
        self.assertTrue(any("reviewer" in error for error in errors))
        self.assertTrue(any("reviewedAt" in error for error in errors))
        self.assertTrue(any("evidence" in error for error in errors))

    def test_validate_session_rejects_contract_drift(self) -> None:
        payload = sample_ledger()
        session = center.build_session_template(
            "1.5",
            "windows-nvda",
            payload,
        )
        session["checks"]["navigation.rail"].pop("activation")
        errors = center.validate_session(
            session,
            payload,
            require_final_results=False,
        )
        self.assertTrue(any("check names" in error for error in errors))

    def test_apply_session_derives_fail_instead_of_accepting_claimed_pass(self) -> None:
        payload = sample_ledger()
        session = center.build_session_template(
            "1.5",
            "windows-nvda",
            payload,
        )
        for checks in session["checks"].values():
            for check in checks:
                checks[check] = "pass"
        session["checks"]["navigation.tabs"]["activation"] = "fail"
        session["reviewer"] = "Native AT reviewer"
        session["reviewedAt"] = "2026-10-05"
        session["evidence"] = "review/session-001"

        errors = center.validate_session(
            session,
            payload,
            require_final_results=True,
        )
        self.assertEqual(errors, [])

        updated = center.apply_session(payload, session)
        record = updated["platforms"]["windows-nvda"]
        self.assertEqual(record["status"], "fail")
        self.assertEqual(record["reviewer"], "Native AT reviewer")
        self.assertEqual(record["reviewedAt"], "2026-10-05")
        self.assertEqual(record["evidence"], "review/session-001")
        self.assertEqual(payload["platforms"]["windows-nvda"]["status"], "pending")

    def test_apply_visual_review_preserves_deterministic_provenance(self) -> None:
        payload = {
            "visual": {
                "status": "pending",
                "reviewer": "",
                "reviewedAt": "",
                "evidence": "CI run 123 / artifact 456",
            }
        }
        updated = center.apply_visual_review(
            payload,
            status="pass",
            reviewer="Visual reviewer",
            reviewed_at="2026-10-05",
            evidence="review/visual-001",
        )
        visual = updated["visual"]
        self.assertEqual(visual["status"], "pass")
        self.assertEqual(visual["reviewer"], "Visual reviewer")
        self.assertIn("CI run 123 / artifact 456", visual["evidence"])
        self.assertIn("review/visual-001", visual["evidence"])
        self.assertEqual(payload["visual"]["status"], "pending")

    def test_ready_requires_platform_evidence_metadata(self) -> None:
        payload = sample_ledger()
        record = payload["platforms"]["windows-nvda"]
        for checks in record["components"].values():
            for check in checks:
                checks[check] = "pass"
        record["status"] = "pass"

        blocked = center.milestone_summary("1.5", payload)
        self.assertFalse(blocked["complete"])

        record["reviewer"] = "Native AT reviewer"
        record["reviewedAt"] = "2026-10-05"
        record["evidence"] = "review/session-001"
        ready = center.milestone_summary("1.5", payload)
        self.assertTrue(ready["complete"])

    def test_invalid_review_date_is_rejected(self) -> None:
        with self.assertRaises(ValueError):
            center.parse_review_date("05/10/2026")


if __name__ == "__main__":
    unittest.main()
