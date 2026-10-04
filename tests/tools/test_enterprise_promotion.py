from __future__ import annotations

import sys
import unittest
from pathlib import Path

TOOLS = Path(__file__).resolve().parents[2] / "tools"
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))

import check_release
import promote_enterprise_1_5


class EnterprisePromotionTests(unittest.TestCase):
    def make_components(self):
        result = []
        for component_id in check_release.ENTERPRISE_AT_COMPONENTS:
            axes = {
                axis: 4
                for axis in check_release.component_registry.AXES
            }
            axes["accessibility"] = 3
            axes.update({
                "lastReviewed": "2026-10-04",
                "gaps": ["native AT review pending"],
                "nextActions": ["record native AT evidence"],
                "evidence": {
                    axis: [f"{axis} evidence"]
                    for axis in check_release.component_registry.AXES
                },
            })
            result.append({
                "id": component_id,
                "maturity": "usable",
                "maturityPolicy": "derived",
                "releaseScope": True,
                "maturityAxes": axes,
            })
        return result

    def test_prerequisites_accept_only_accessibility_as_remaining_gap(self):
        self.assertEqual(
            [],
            promote_enterprise_1_5.promotion_prerequisite_errors(
                self.make_components()
            ),
        )

    def test_prerequisites_reject_other_sub_four_axis(self):
        components = self.make_components()
        components[0]["maturityAxes"]["keyboard"] = 3
        errors = promote_enterprise_1_5.promotion_prerequisite_errors(
            components
        )
        self.assertTrue(any("keyboard=3" in error for error in errors), errors)

    def test_promotion_sets_complete_accessibility_and_clears_gaps(self):
        components = self.make_components()
        promote_enterprise_1_5.promote_registry(
            components,
            reviewed_at="2026-10-05",
        )
        for component in components:
            axes = component["maturityAxes"]
            self.assertEqual(component["maturity"], "complete")
            self.assertEqual(axes["accessibility"], 4)
            self.assertEqual(axes["gaps"], [])
            self.assertEqual(axes["lastReviewed"], "2026-10-05")
            self.assertTrue(
                any(
                    "enterprise-accessibility-1.5.json" in item
                    for item in axes["evidence"]["accessibility"]
                )
            )

    def test_promotion_enables_enterprise_complete(self):
        rules = {
            "base": {
                "stable_release": {
                    "enterprise_complete": False,
                }
            }
        }
        promote_enterprise_1_5.promote_rules(rules)
        self.assertTrue(
            rules["base"]["stable_release"]["enterprise_complete"]
        )


if __name__ == "__main__":
    unittest.main()
