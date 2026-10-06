from __future__ import annotations

import copy
import json
import sys
import tempfile
import unittest
from pathlib import Path

TOOLS = Path(__file__).resolve().parents[2] / "tools"
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))

import check_expressive_catalogue_1_10_1_11 as certification


class ExpressiveCertificationTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.payload = certification.read_ledger()

    def complete_payload(self) -> dict:
        payload = copy.deepcopy(self.payload)
        payload["visual"]["status"] = "pass"
        payload["visual"]["reviewer"] = "Visual reviewer"
        payload["visual"]["reviewedAt"] = "2026-10-06"
        payload["visual"]["evidence"] = "review/expressive-visual"

        for record in payload["platforms"].values():
            record["status"] = "pass"
            record["reviewer"] = "AT reviewer"
            record["reviewedAt"] = "2026-10-06"
            record["evidence"] = "review/native-at"
            for checks in record["components"].values():
                for key in checks:
                    checks[key] = "pass"
        return payload

    def test_pending_ledger_is_structurally_valid(self) -> None:
        self.assertEqual(
            certification.validate(
                certification.ROOT,
                copy.deepcopy(self.payload),
                require_complete=False,
            ),
            [],
        )

    def test_complete_gate_rejects_pending_evidence(self) -> None:
        errors = certification.validate(
            certification.ROOT,
            copy.deepcopy(self.payload),
            require_complete=True,
        )
        self.assertTrue(any("visual review" in error for error in errors))
        self.assertTrue(any("promotion requires pass" in error for error in errors))

    def test_platform_cannot_claim_pass_with_pending_checks(self) -> None:
        payload = copy.deepcopy(self.payload)
        payload["platforms"]["windows-nvda"]["status"] = "pass"
        errors = certification.validate(
            certification.ROOT,
            payload,
            require_complete=False,
        )
        self.assertTrue(
            any("cannot be pass" in error for error in errors),
            errors,
        )

    def test_component_contract_drift_is_rejected(self) -> None:
        payload = copy.deepcopy(self.payload)
        checks = payload["platforms"]["linux-orca"]["components"]["button.split"]
        checks.pop("secondaryAction")
        errors = certification.validate(
            certification.ROOT,
            payload,
            require_complete=False,
        )
        self.assertTrue(
            any("checks do not match" in error for error in errors),
            errors,
        )

    def test_complete_evidence_passes_with_present_goldens(self) -> None:
        payload = self.complete_payload()
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for relative in certification.VISUAL_GOLDENS:
                path = root / relative
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_bytes(b"reviewed")
            self.assertEqual(
                certification.validate(root, payload, require_complete=True),
                [],
            )


if __name__ == "__main__":
    unittest.main()
