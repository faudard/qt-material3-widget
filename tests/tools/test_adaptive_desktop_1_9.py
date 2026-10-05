from __future__ import annotations

import sys
import tempfile
import unittest
from pathlib import Path

TOOLS = Path(__file__).resolve().parents[2] / "tools"
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))

import check_adaptive_desktop_1_9 as certification
import promote_adaptive_desktop_1_9 as promotion


class AdaptiveDesktop19CertificationTests(unittest.TestCase):
    def make_payload(self, *, complete: bool) -> dict:
        result = "pass" if complete else "pending"
        platforms = {}
        for platform_id, reader in certification.PLATFORMS.items():
            platforms[platform_id] = {
                "screenReader": reader,
                "status": result,
                "reviewer": "reviewer" if complete else "",
                "reviewedAt": "2026-10-04" if complete else "",
                "evidence": "artifact://reader" if complete else "",
                "components": {
                    component_id: {
                        check: result
                        for check in checks
                    }
                    for component_id, checks
                    in certification.COMPONENT_CHECKS.items()
                },
            }

        return {
            "schemaVersion": 1,
            "certification": "QtMaterial3 1.9 Adaptive / Desktop",
            "requiredComponents": list(certification.COMPONENT_CHECKS),
            "visual": {
                "prefix": "adaptive_desktop",
                "renderer": {
                    "qtVersion": "6.4.0",
                    "style": "Fusion",
                    "platform": "xcb",
                    "scaleFactor": "1",
                    "fontDpi": "96",
                },
                "status": result,
                "reviewer": "reviewer" if complete else "",
                "reviewedAt": "2026-10-04" if complete else "",
                "evidence": "artifact://visual" if complete else "",
                "requiredGoldens": list(certification.VISUAL_GOLDENS),
            },
            "platforms": platforms,
        }

    def write_goldens(self, root: Path) -> None:
        for relative in certification.VISUAL_GOLDENS:
            path = root / relative
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(b"reviewed")

    def make_components(self) -> list[dict]:
        result = []
        for component_id in certification.COMPONENT_CHECKS:
            axes = {
                "api": 4,
                "rendering": 3,
                "states": 4,
                "accessibility": 3,
                "keyboard": (
                    4 if component_id == "navigation.suite" else "N/A"
                ),
                "hidpi": 4,
                "rtl": 4,
                "tests": 4,
                "example": 4,
                "docs": 4,
                "lastReviewed": "2026-10-04",
                "gaps": ["review pending"],
                "nextActions": ["review evidence"],
                "evidence": {
                    "rendering": ["candidate"],
                    "accessibility": ["automated"],
                },
            }
            result.append({
                "id": component_id,
                "maturity": "usable",
                "maturityPolicy": "derived",
                "releaseScope": False,
                "referenceCandidate": False,
                "maturityAxes": axes,
            })
        return result

    def test_pending_ledger_is_structurally_valid(self):
        with tempfile.TemporaryDirectory() as tmp:
            errors = certification.validate(
                Path(tmp),
                self.make_payload(complete=False),
                require_complete=False,
            )
        self.assertEqual(errors, [])

    def test_pending_ledger_blocks_complete_promotion(self):
        with tempfile.TemporaryDirectory() as tmp:
            errors = certification.validate(
                Path(tmp),
                self.make_payload(complete=False),
                require_complete=True,
            )
        self.assertTrue(
            any("promotion requires pass" in error for error in errors),
            errors,
        )

    def test_complete_ledger_requires_all_thirty_goldens(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            payload = self.make_payload(complete=True)
            errors = certification.validate(
                root,
                payload,
                require_complete=True,
            )
            self.assertTrue(
                any("reviewed golden is missing" in error for error in errors),
                errors,
            )
            self.write_goldens(root)
            self.assertEqual(
                certification.validate(
                    root,
                    payload,
                    require_complete=True,
                ),
                [],
            )

    def test_platform_pass_rejects_incomplete_component_check(self):
        payload = self.make_payload(complete=True)
        payload["platforms"]["windows-nvda"]["components"][
            "navigation.suite"
        ]["focusAcrossModeSwitch"] = "pending"
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            self.write_goldens(root)
            errors = certification.validate(
                root,
                payload,
                require_complete=True,
            )
        self.assertTrue(any("cannot be pass" in error for error in errors), errors)

    def test_promotion_prerequisites_allow_only_visual_and_a11y_gaps(self):
        components = self.make_components()
        self.assertEqual(
            promotion.promotion_prerequisite_errors(components),
            [],
        )

        components[0]["maturityAxes"]["rtl"] = 3
        errors = promotion.promotion_prerequisite_errors(components)
        self.assertTrue(any("rtl=3" in error for error in errors), errors)

    def test_promotion_sets_complete_and_representative_visual_mapping(self):
        components = self.make_components()
        promotion.promote_registry(
            components,
            reviewed_at="2026-10-05",
        )
        for component in components:
            self.assertEqual(component["maturity"], "complete")
            self.assertTrue(component["releaseScope"])
            self.assertEqual(component["maturityAxes"]["rendering"], 4)
            self.assertEqual(component["maturityAxes"]["accessibility"], 4)
            self.assertEqual(component["maturityAxes"]["gaps"], [])

        rules = {
            "base": {
                "stable_release": {
                    "visual_goldens": [],
                    "component_visual_prefixes": {},
                }
            }
        }
        promotion.promote_rules(rules)
        stable = rules["base"]["stable_release"]

        self.assertEqual(
            len(stable["visual_goldens"]),
            len(certification.VISUAL_GOLDENS),
        )
        for relative in certification.VISUAL_GOLDENS:
            self.assertIn(relative, stable["visual_goldens"])
        self.assertEqual(
            stable["component_visual_prefixes"]["navigation.suite"],
            "adaptive_desktop_compact_ltr",
        )
        self.assertEqual(
            stable["component_visual_prefixes"]["layout.adaptive-shell"],
            "adaptive_desktop_expanded_ltr",
        )


if __name__ == "__main__":
    unittest.main()
