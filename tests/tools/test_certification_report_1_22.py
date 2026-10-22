from __future__ import annotations

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
import certification_report_1_22 as health


class CertificationHealth122Tests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)
        self.registry = self.root / "docs/components/component-registry.json"
        self.ledger = self.root / "docs/components/fixture-ledger.json"
        self.registry.parent.mkdir(parents=True)
        self.scope = patch.dict(center.MILESTONES, {
            "1.7": {
                "name": "Fixture",
                "ledger": "docs/components/fixture-ledger.json",
                "visual": True,
            },
        }, clear=True)
        self.scope.start()
        self.addCleanup(self.scope.stop)
        self.components = [
            {
                "id": "navigation.bar", "name": "Navigation Bar",
                "maturity": "usable", "maturityPolicy": "manual",
                "releaseScope": True,
                "maturityAxes": {
                    "api": 4, "rendering": 3, "states": 4,
                    "accessibility": 3, "keyboard": 4, "hidpi": 4,
                    "rtl": 4, "tests": 4, "example": 4, "docs": 4,
                    "gaps": ["Native AT pending"], "nextActions": ["Check announcements"],
                },
            },
            {
                "id": "button.filled", "name": "Filled Button",
                "maturity": "complete", "maturityPolicy": "manual",
                "releaseScope": True,
                "maturityAxes": {key: 4 for key in health.registry.AXES},
            },
        ]
        self.evidence = {
            "schemaVersion": 1,
            "requiredComponents": ["navigation.bar"],
            "platforms": {
                platform: {
                    "screenReader": reader, "status": "pending",
                    "reviewer": "", "reviewedAt": "", "evidence": "",
                    "components": {"navigation.bar": {
                        "traversal": "pending", "activation": "pending",
                    }},
                }
                for platform, reader in health.READERS.items()
            },
            "visual": {
                "status": "pending", "reviewer": "", "reviewedAt": "",
                "evidence": "",
                "requiredGoldens": ["tests/visual/goldens/test.png"],
                "renderer": {
                    "qtVersion": "6.4.0", "style": "Fusion",
                    "platform": "xcb", "scaleFactor": "1", "fontDpi": "96",
                },
            },
        }
        self.write()

    def write(self):
        self.registry.write_text(json.dumps(self.components), encoding="utf-8")
        self.ledger.write_text(json.dumps(self.evidence), encoding="utf-8")

    def test_pending_signoff_is_not_a_structural_error(self):
        result = health.build_report(self.root)
        self.assertEqual(result["issues"], [])
        self.assertEqual(result["summary"]["tracked"], 2)
        self.assertEqual(result["summary"]["notComplete"], 1)
        self.assertEqual(result["summary"]["releaseBlocked"], 1)
        self.assertFalse(result["summary"]["readyForPromotion"])
        candidate = result["components"][0]
        self.assertEqual([axis["axis"] for axis in candidate["axisGaps"]],
                         ["rendering", "accessibility"])
        self.assertEqual(len(candidate["nativeAT"][0]["platforms"]), 3)
        self.assertEqual(candidate["visual"][0]["missingGoldens"],
                         ["tests/visual/goldens/test.png"])
        self.assertIn("navigation.bar", health.markdown(result))
        self.assertNotIn("| button.filled |", health.markdown(result))

    def test_at_and_visual_evidence_never_auto_promote_maturity(self):
        for platform in self.evidence["platforms"].values():
            platform.update(status="pass", reviewer="Tester",
                            reviewedAt="2026-10-22", evidence="native/real-run")
            platform["components"]["navigation.bar"] = {
                "traversal": "pass", "activation": "pass",
            }
        self.evidence["visual"].update(
            status="pass", reviewer="Reviewer",
            reviewedAt="2026-10-22", evidence="render/pinned-review",
        )
        golden = self.root / "tests/visual/goldens/test.png"
        golden.parent.mkdir(parents=True)
        golden.write_bytes(b"fixture")
        self.write()
        result = health.build_report(self.root)
        self.assertEqual(result["issues"], [])
        self.assertTrue(result["components"][0]["nativeAT"][0]["certified"])
        self.assertTrue(result["components"][0]["visual"][0]["certified"])
        self.assertFalse(result["summary"]["readyForPromotion"])
        self.assertEqual(result["summary"]["notComplete"], 1)

    def test_forged_at_pass_is_rejected(self):
        self.evidence["platforms"]["windows-nvda"]["status"] = "pass"
        self.write()
        result = health.build_report(self.root)
        self.assertTrue(any("unsubstantiated native AT pass" in issue
                            for issue in result["issues"]))
        self.assertFalse(result["components"][0]["nativeAT"][0]["certified"])

    def test_visual_pass_needs_review_and_committed_files(self):
        self.evidence["visual"].update(
            status="pass", reviewer="Human",
            reviewedAt="2026-10-22", evidence="review",
        )
        self.write()
        result = health.build_report(self.root)
        self.assertTrue(any("visual pass lacks" in issue for issue in result["issues"]))

    def test_missing_platform_or_unmapped_usable_component_is_reported(self):
        self.evidence["platforms"].pop("macos-voiceover")
        self.components[0]["id"] = "navigation.other"
        self.write()
        result = health.build_report(self.root)
        self.assertTrue(any("three native" in issue for issue in result["issues"]))
        self.assertTrue(any("unknown required component" in issue for issue in result["issues"]))
        self.assertTrue(any("no native AT certification ledger" in issue for issue in result["issues"]))

    def test_reject_unsafe_goldens_and_invalid_dates(self):
        self.evidence["visual"]["requiredGoldens"] = ["../outside.png"]
        self.write()
        self.assertTrue(any("unsafe golden" in issue
                            for issue in health.build_report(self.root)["issues"]))
        self.assertFalse(health.attributed({
            "reviewer": "Someone", "reviewedAt": "2026-02-30", "evidence": "artifact",
        }))

    def test_canonical_repository_has_17_identified_usable_components(self):
        # Contract against the tracked 1.22 baseline; no manual statuses invented.
        actual = health.build_report(health.ROOT)
        self.assertEqual(actual["summary"]["tracked"], 60)
        self.assertEqual(actual["summary"]["notComplete"], 17)
        self.assertEqual(actual["summary"]["maturity"]["complete"], 43)
        self.assertEqual(actual["issues"], [])
        self.assertEqual(len([c for c in actual["components"]
                              if c["maturity"] == "usable" and c["nativeAT"]]), 17)


if __name__ == "__main__":
    unittest.main()
