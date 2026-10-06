from __future__ import annotations

import copy
import sys
import unittest
from pathlib import Path

TOOLS = Path(__file__).resolve().parents[2] / "tools"
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))

import component_registry
import promote_expressive_1_10_1_11 as promotion


class ExpressivePromotionTests(unittest.TestCase):
    def test_repository_is_structurally_ready_for_final_promotion(self) -> None:
        components = component_registry.load_registry(promotion.ROOT)
        self.assertEqual(
            promotion.promotion_prerequisite_errors(components),
            [],
        )

    def test_promotion_closes_only_standalone_expressive_components(self) -> None:
        components = copy.deepcopy(
            component_registry.load_registry(promotion.ROOT)
        )
        by_id_before = {
            item["id"]: copy.deepcopy(item)
            for item in components
        }

        promotion.promote_registry(
            components,
            reviewed_at_value="2026-10-06",
        )
        by_id = {item["id"]: item for item in components}

        for component_id in promotion.certification.PROMOTION_COMPONENTS:
            component = by_id[component_id]
            self.assertEqual(component["maturity"], "complete")
            self.assertTrue(component["releaseScope"])
            self.assertFalse(component["referenceCandidate"])
            axes = component["maturityAxes"]
            for axis in component_registry.AXES:
                self.assertIn(axes[axis], (4, "N/A"))
            self.assertEqual(axes["gaps"], [])
            self.assertEqual(axes["lastReviewed"], "2026-10-06")

        for component_id in promotion.certification.INTEGRATED_MODES:
            self.assertEqual(by_id[component_id], by_id_before[component_id])

    def test_rules_gain_expressive_goldens_and_component_prefixes(self) -> None:
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

        for relative in promotion.certification.VISUAL_GOLDENS:
            self.assertIn(relative, stable["visual_goldens"])
        for component_id in promotion.certification.PROMOTION_COMPONENTS:
            self.assertEqual(
                stable["component_visual_prefixes"][component_id],
                "expressive_catalogue_matrix",
            )


if __name__ == "__main__":
    unittest.main()
