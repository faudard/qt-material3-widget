from __future__ import annotations

import copy
import importlib.util
import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

if not (ROOT / "tools/check_material_renderer_conformance.py").is_file():
    raise unittest.SkipTest("Material conformance tooling is not implemented yet: tools/check_material_renderer_conformance.py")
SCRIPT = ROOT / "tools/check_material_renderer_conformance.py"
SPEC = importlib.util.spec_from_file_location(
    "qtm3_material_renderer_conformance_tests",
    SCRIPT,
)
assert SPEC is not None and SPEC.loader is not None
checker = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = checker
SPEC.loader.exec_module(checker)


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def load_json(path: Path):
    return json.loads(read(path))


class MaterialRendererConformanceTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.model = load_json(ROOT / checker.MODEL_PATH)
        cls.suite = load_json(ROOT / checker.SUITE_PATH)
        cls.elevated_suite = load_json(ROOT / checker.ELEVATED_SUITE_PATH)
        cls.filled_tonal_suite = load_json(ROOT / checker.FILLED_TONAL_SUITE_PATH)
        cls.outlined_suite = load_json(ROOT / checker.OUTLINED_SUITE_PATH)
        cls.text_suite = load_json(ROOT / checker.TEXT_SUITE_PATH)
        cls.spec_header = read(ROOT / checker.SPEC_HEADER_PATH)
        cls.resolver = read(ROOT / checker.RESOLVER_PATH)
        cls.applier = read(ROOT / checker.TOKEN_APPLIER_PATH)
        cls.theme_builder = read(ROOT / checker.THEME_BUILDER_PATH)
        cls.widget = read(ROOT / checker.WIDGET_SOURCE_PATH)
        cls.elevated_widget = read(ROOT / checker.ELEVATED_WIDGET_SOURCE_PATH)
        cls.filled_tonal_widget = read(
            ROOT / checker.FILLED_TONAL_WIDGET_SOURCE_PATH
        )
        cls.outlined_widget = read(ROOT / checker.OUTLINED_WIDGET_SOURCE_PATH)
        cls.text_button = read(ROOT / checker.TEXT_BUTTON_SOURCE_PATH)
        cls.motion_helper = read(ROOT / checker.MOTION_HELPER_PATH)
        cls.render_helper = read(ROOT / checker.RENDER_HELPER_PATH)

    def product(self, **overrides):
        inputs = {
            "resolver_source": self.resolver,
            "token_applier": self.applier,
            "theme_builder": self.theme_builder,
            "widget_source": self.widget,
            "text_button_source": self.text_button,
            "motion_helper": self.motion_helper,
        }
        inputs.update(overrides)
        return checker.extract_product_values(**inputs)

    def renderer_errors(self, **overrides):
        inputs = {
            "spec_header": self.spec_header,
            "widget_source": self.widget,
            "text_button_source": self.text_button,
            "motion_helper": self.motion_helper,
            "render_helper": self.render_helper,
        }
        inputs.update(overrides)
        return checker.validate_renderer_contract(**inputs)

    def elevated_product(self, **overrides):
        inputs = {
            "resolver_source": self.resolver,
            "token_applier": self.applier,
            "theme_builder": self.theme_builder,
            "widget_source": self.widget,
            "text_button_source": self.text_button,
            "motion_helper": self.motion_helper,
        }
        inputs.update(overrides)
        return checker.extract_elevated_product_values(**inputs)

    def filled_tonal_product(self, **overrides):
        inputs = {
            "resolver_source": self.resolver,
            "token_applier": self.applier,
            "theme_builder": self.theme_builder,
            "widget_source": self.widget,
            "text_button_source": self.text_button,
            "motion_helper": self.motion_helper,
        }
        inputs.update(overrides)
        return checker.extract_filled_tonal_product_values(**inputs)

    def outlined_product(self, **overrides):
        inputs = {
            "resolver_source": self.resolver,
            "token_applier": self.applier,
            "theme_builder": self.theme_builder,
            "widget_source": self.widget,
            "text_button_source": self.text_button,
            "motion_helper": self.motion_helper,
        }
        inputs.update(overrides)
        return checker.extract_outlined_product_values(**inputs)

    def outlined_renderer_errors(self, **overrides):
        inputs = {
            "spec_header": self.spec_header,
            "outlined_widget_source": self.outlined_widget,
            "render_helper": self.render_helper,
        }
        inputs.update(overrides)
        return checker.validate_outlined_renderer_contract(**inputs)

    def text_product(self, **overrides):
        inputs = {
            "resolver_source": self.resolver,
            "token_applier": self.applier,
            "theme_builder": self.theme_builder,
            "widget_source": self.widget,
            "text_button_source": self.text_button,
            "motion_helper": self.motion_helper,
        }
        inputs.update(overrides)
        return checker.extract_text_product_values(**inputs)

    def text_renderer_errors(self, **overrides):
        inputs = {
            "spec_header": self.spec_header,
            "text_button_source": self.text_button,
        }
        inputs.update(overrides)
        return checker.validate_text_renderer_contract(**inputs)

    def test_active_button_renderer_verticals_pass(self) -> None:
        self.assertEqual([], checker.validate_tree(ROOT))

    def test_all_100_reference_values_match_extracted_product_values(self) -> None:
        reference, reference_errors = checker.collect_reference_values(self.suite)
        product, product_errors = self.product()
        self.assertEqual([], reference_errors)
        self.assertEqual([], product_errors)
        compared = 0
        for (state, _theme), expected in reference.items():
            self.assertEqual(expected, product[state])
            compared += len(expected)
        self.assertEqual(100, compared)

    def test_disabled_container_role_drift_is_detected(self) -> None:
        changed = self.resolver.replace(
            "theme.colorScheme().color(ColorRole::OnSurface),",
            "theme.colorScheme().color(ColorRole::SurfaceContainerHigh),",
            1,
        )
        product, errors = self.product(resolver_source=changed)
        self.assertEqual([], errors)
        self.assertEqual(
            "SurfaceContainerHigh",
            product["disabled"]["render.container.color.role"],
        )
        self.assertNotEqual(
            "OnSurface",
            product["disabled"]["render.container.color.role"],
        )

    def test_disabled_alpha_drift_is_detected(self) -> None:
        changed = self.resolver.replace(
            "kDisabledButtonContainerOpacity = 0.10;",
            "kDisabledButtonContainerOpacity = 0.11;",
        )
        product, errors = self.product(resolver_source=changed)
        self.assertEqual([], errors)
        self.assertEqual(
            0.11,
            product["disabled"]["render.container.color.alpha"],
        )

    def test_hover_elevation_drift_is_detected(self) -> None:
        changed = self.resolver.replace(
            "spec.hoverElevationProgress = 1.0;",
            "spec.hoverElevationProgress = 0.0;",
        )
        product, errors = self.product(resolver_source=changed)
        self.assertEqual([], errors)
        self.assertEqual(
            "Level0",
            product["hovered"]["render.elevation.role"],
        )

    def test_all_100_elevated_reference_values_match_product(self) -> None:
        reference, reference_errors = checker.collect_reference_values(
            self.elevated_suite
        )
        product, product_errors = self.elevated_product()
        self.assertEqual([], reference_errors)
        self.assertEqual([], product_errors)
        compared = 0
        for (state, _theme), expected in reference.items():
            self.assertEqual(expected, product[state])
            compared += len(expected)
        self.assertEqual(100, compared)

    def test_all_100_filled_tonal_reference_values_match_product(self) -> None:
        reference, reference_errors = checker.collect_reference_values(
            self.filled_tonal_suite
        )
        product, product_errors = self.filled_tonal_product()
        self.assertEqual([], reference_errors)
        self.assertEqual([], product_errors)
        compared = 0
        for (state, _theme), expected in reference.items():
            self.assertEqual(expected, product[state])
            compared += len(expected)
        self.assertEqual(100, compared)

    def test_filled_tonal_disabled_alpha_drift_is_detected(self) -> None:
        changed = self.resolver.replace(
            "kFilledTonalDisabledContainerOpacity = 0.12;",
            "kFilledTonalDisabledContainerOpacity = 0.13;",
        )
        product, errors = self.filled_tonal_product(resolver_source=changed)
        self.assertEqual([], errors)
        self.assertEqual(
            0.13,
            product["disabled"]["render.container.color.alpha"],
        )

    def test_filled_tonal_disabled_content_role_drift_is_detected(self) -> None:
        tonal_start = self.resolver.index(
            "ButtonSpec ButtonSpecResolver::filledTonalButtonSpec("
        )
        tonal_source = self.resolver[tonal_start:].replace(
            "theme.colorScheme().color(ColorRole::OnSurface),\n"
            "        kDisabledButtonContentOpacity",
            "theme.colorScheme().color(ColorRole::OnSurfaceVariant),\n"
            "        kDisabledButtonContentOpacity",
            1,
        )
        changed = self.resolver[:tonal_start] + tonal_source
        product, errors = self.filled_tonal_product(resolver_source=changed)
        self.assertEqual([], errors)
        self.assertEqual(
            "OnSurfaceVariant",
            product["disabled"]["render.label.color.role"],
        )

    def test_filled_tonal_hover_elevation_drift_is_detected(self) -> None:
        tonal_start = self.resolver.index(
            "ButtonSpec ButtonSpecResolver::filledTonalButtonSpec("
        )
        tonal_source = self.resolver[tonal_start:].replace(
            "spec.hoverElevationRole = ElevationRole::Level1;",
            "spec.hoverElevationRole = ElevationRole::Level0;",
            1,
        )
        changed = self.resolver[:tonal_start] + tonal_source
        product, errors = self.filled_tonal_product(resolver_source=changed)
        self.assertEqual([], errors)
        self.assertEqual(
            "Level0",
            product["hovered"]["render.elevation.role"],
        )

    def test_all_130_outlined_reference_values_match_product(self) -> None:
        reference, reference_errors = checker.collect_reference_values(
            self.outlined_suite,
            checker.OUTLINED_REFERENCE_PATHS,
        )
        product, product_errors = self.outlined_product()
        self.assertEqual([], reference_errors)
        self.assertEqual([], product_errors)
        compared = 0
        for (state, _theme), expected in reference.items():
            self.assertEqual(expected, product[state])
            compared += len(expected)
        self.assertEqual(130, compared)

    def test_all_100_text_reference_values_match_product(self) -> None:
        reference, reference_errors = checker.collect_reference_values(
            self.text_suite
        )
        product, product_errors = self.text_product()
        self.assertEqual([], reference_errors)
        self.assertEqual([], product_errors)
        compared = 0
        for (state, _theme), expected in reference.items():
            self.assertEqual(expected, product[state])
            compared += len(expected)
        self.assertEqual(100, compared)

    def test_text_content_role_drift_is_detected(self) -> None:
        text_start = self.resolver.index(
            "ButtonSpec ButtonSpecResolver::textButtonSpec("
        )
        filled_start = self.resolver.index(
            "ButtonSpec ButtonSpecResolver::filledButtonSpec("
        )
        text_source = self.resolver[text_start:filled_start].replace(
            "ColorRole::Primary",
            "ColorRole::OnSurfaceVariant",
            1,
        )
        changed = self.resolver[:text_start] + text_source + self.resolver[filled_start:]
        product, errors = self.text_product(resolver_source=changed)
        self.assertEqual([], errors)
        self.assertEqual(
            "OnSurfaceVariant",
            product["enabled"]["render.label.color.role"],
        )

    def test_text_disabled_content_alpha_drift_is_detected(self) -> None:
        changed = self.resolver.replace(
            "kDisabledButtonContentOpacity = 0.38;",
            "kDisabledButtonContentOpacity = 0.39;",
        )
        product, errors = self.text_product(resolver_source=changed)
        self.assertEqual([], errors)
        self.assertEqual(
            0.39,
            product["disabled"]["render.label.color.alpha"],
        )

    def test_text_elevation_drift_is_detected(self) -> None:
        text_start = self.resolver.index(
            "ButtonSpec ButtonSpecResolver::textButtonSpec("
        )
        filled_start = self.resolver.index(
            "ButtonSpec ButtonSpecResolver::filledButtonSpec("
        )
        text_source = self.resolver[text_start:filled_start].replace(
            "spec.elevationRole = ElevationRole::Level0;",
            "spec.elevationRole = ElevationRole::Level1;",
            1,
        )
        changed = self.resolver[:text_start] + text_source + self.resolver[filled_start:]
        product, errors = self.text_product(resolver_source=changed)
        self.assertEqual([], errors)
        self.assertEqual(
            "Level1",
            product["enabled"]["render.elevation.role"],
        )

    def test_text_typed_override_branch_is_required(self) -> None:
        changed = self.applier.replace(
            "if (isTextButton) {",
            "if (isFilledButton) {",
            1,
        )
        _, errors = self.text_product(token_applier=changed)
        self.assertTrue(any("Text Button token resolution" in error for error in errors))

    def test_outlined_content_role_drift_is_detected(self) -> None:
        outlined_start = self.resolver.index(
            "ButtonSpec ButtonSpecResolver::outlinedButtonSpec("
        )
        outlined_source = self.resolver[outlined_start:].replace(
            "ColorRole::OnSurfaceVariant",
            "ColorRole::Primary",
            1,
        )
        changed = self.resolver[:outlined_start] + outlined_source
        product, errors = self.outlined_product(resolver_source=changed)
        self.assertEqual([], errors)
        self.assertEqual(
            "Primary",
            product["enabled"]["render.label.color.role"],
        )

    def test_outlined_outline_role_drift_is_detected(self) -> None:
        outlined_start = self.resolver.index(
            "ButtonSpec ButtonSpecResolver::outlinedButtonSpec("
        )
        outlined_source = self.resolver[outlined_start:].replace(
            "ColorRole::OutlineVariant",
            "ColorRole::Outline",
            1,
        )
        changed = self.resolver[:outlined_start] + outlined_source
        product, errors = self.outlined_product(resolver_source=changed)
        self.assertEqual([], errors)
        self.assertEqual(
            "Outline",
            product["enabled"]["render.outline.color.role"],
        )

    def test_outlined_disabled_outline_alpha_drift_is_detected(self) -> None:
        changed = self.resolver.replace(
            "kDisabledButtonContainerOpacity = 0.10;",
            "kDisabledButtonContainerOpacity = 0.11;",
        )
        product, errors = self.outlined_product(resolver_source=changed)
        self.assertEqual([], errors)
        self.assertEqual(
            0.11,
            product["disabled"]["render.outline.color.alpha"],
        )

    def test_outlined_outline_width_drift_is_detected(self) -> None:
        changed = self.resolver.replace(
            "kSmallOutlinedButtonOutlineWidth = 1.0;",
            "kSmallOutlinedButtonOutlineWidth = 2.0;",
        )
        product, errors = self.outlined_product(resolver_source=changed)
        self.assertEqual([], errors)
        self.assertEqual(2.0, product["enabled"]["render.outline.width"])

    def test_elevated_disabled_container_role_drift_is_detected(self) -> None:
        elevated_start = self.resolver.index(
            "ButtonSpec ButtonSpecResolver::elevatedButtonSpec("
        )
        elevated_source = self.resolver[elevated_start:]
        elevated_source = elevated_source.replace(
            "ColorRole::OnSurface",
            "ColorRole::SurfaceContainerHigh",
            1,
        )
        changed = self.resolver[:elevated_start] + elevated_source
        product, errors = self.elevated_product(resolver_source=changed)
        self.assertEqual([], errors)
        self.assertEqual(
            "SurfaceContainerHigh",
            product["disabled"]["render.container.color.role"],
        )

    def test_elevated_hover_elevation_role_drift_is_detected(self) -> None:
        changed = self.resolver.replace(
            "spec.hoverElevationRole = ElevationRole::Level2;",
            "spec.hoverElevationRole = ElevationRole::Level1;",
        )
        product, errors = self.elevated_product(resolver_source=changed)
        self.assertEqual([], errors)
        self.assertEqual(
            "Level1",
            product["hovered"]["render.elevation.role"],
        )

    def test_direct_theme_access_in_paint_is_rejected(self) -> None:
        changed = self.widget.replace(
            "spec.containerColor",
            "theme().colorScheme().color(ColorRole::Primary)",
            1,
        )
        errors = self.renderer_errors(widget_source=changed)
        self.assertTrue(any("outside resolution" in error for error in errors))

    def test_hard_coded_material_value_in_paint_is_rejected(self) -> None:
        changed = self.widget.replace(
            "ensurePolished();",
            "ensurePolished();\n const qreal materialOpacity = 0.38;",
            1,
        )
        errors = self.renderer_errors(widget_source=changed)
        self.assertTrue(any("hard-coded Material" in error for error in errors))

    def test_reformatted_material_literal_cannot_evade_renderer_gate(self) -> None:
        changed = self.widget.replace(
            "ensurePolished();",
            "ensurePolished();\n const qreal materialOpacity = 10e-2;",
            1,
        )
        errors = self.renderer_errors(widget_source=changed)
        self.assertTrue(any("hard-coded Material" in error for error in errors))

    def test_disabled_feedback_guard_is_required(self) -> None:
        changed = self.widget.replace(
            "if (isEnabled() && layerOpacity > 0.0)",
            "if (layerOpacity > 0.0)",
        )
        errors = self.renderer_errors(widget_source=changed)
        self.assertTrue(any("suppress a transitioning" in error for error in errors))

    def test_disabled_focus_ring_guard_is_required(self) -> None:
        changed = self.widget.replace(
            "if (isEnabled() && interactionState().isFocused())",
            "if (interactionState().isFocused())",
        )
        errors = self.renderer_errors(widget_source=changed)
        self.assertTrue(any("suppress focus-ring" in error for error in errors))

    def test_pressed_fixed_layer_plus_ripple_is_rejected(self) -> None:
        changed = self.motion_helper.replace(
            "return 0.0;\n    }\n    if (state.isFocused())",
            "return spec.pressStateLayerOpacity;\n    }\n    if (state.isFocused())",
            1,
        )
        product, errors = self.product(motion_helper=changed)
        self.assertTrue(any("pressed state-layer target drift" in error for error in errors))
        self.assertEqual(
            "state-layer-and-ripple",
            product["pressed"]["render.interaction.kind"],
        )

    def test_focus_ring_width_must_come_from_resolved_spec(self) -> None:
        changed = self.widget.replace(
            "spec.focusRingWidth",
            "2.0",
            1,
        )
        errors = self.renderer_errors(widget_source=changed)
        self.assertTrue(any("spec.focusRingWidth" in error for error in errors))

    def test_hover_elevation_style_must_come_from_resolved_spec(self) -> None:
        changed = self.widget.replace(
            "spec.hoverElevationStyle,",
            "spec.elevationStyle,",
            1,
        )
        errors = self.renderer_errors(widget_source=changed)
        self.assertTrue(any("spec.hoverElevationStyle" in error for error in errors))

    def test_elevated_variant_binding_is_required(self) -> None:
        changed = self.elevated_widget.replace(
            "factory.elevatedButtonSpec(theme(), density())",
            "factory.filledButtonSpec(theme(), density())",
        )
        errors = checker.validate_elevated_renderer_binding(changed)
        self.assertTrue(any("elevated ButtonSpec" in error for error in errors))

    def test_filled_tonal_variant_binding_is_required(self) -> None:
        changed = self.filled_tonal_widget.replace(
            "factory.filledTonalButtonSpec(theme(), density())",
            "factory.filledButtonSpec(theme(), density())",
        )
        errors = checker.validate_filled_tonal_renderer_binding(changed)
        self.assertTrue(any("filled-tonal ButtonSpec" in error for error in errors))

    def test_filled_tonal_text_constructor_preserves_identity(self) -> None:
        changed = self.filled_tonal_widget.replace(
            ": QtMaterialFilledTonalButton(parent)",
            ": QtMaterialFilledButton(parent)",
            1,
        )
        errors = checker.validate_filled_tonal_renderer_binding(changed)
        self.assertTrue(any("preserve variant identity" in error for error in errors))

    def test_outlined_variant_binding_is_required(self) -> None:
        changed = self.outlined_widget.replace(
            "factory.outlinedButtonSpec(theme(), density())",
            "factory.textButtonSpec(theme(), density())",
        )
        errors = self.outlined_renderer_errors(outlined_widget_source=changed)
        self.assertTrue(any("outlined ButtonSpec" in error for error in errors))

    def test_outlined_width_must_come_from_resolved_spec(self) -> None:
        changed = self.outlined_widget.replace(
            "button.currentButtonSpec().outlineWidth",
            "1.0 / button.devicePixelRatioF()",
        )
        errors = self.outlined_renderer_errors(outlined_widget_source=changed)
        self.assertTrue(any("outline width must come" in error for error in errors))
        self.assertTrue(any("hard-coded Material" in error for error in errors))

    def test_outlined_pressed_fixed_layer_plus_ripple_is_rejected(self) -> None:
        changed = self.render_helper.replace(
            "if (state.isPressed()) {\n        return 0.0;",
            "if (state.isPressed()) {\n        return spec.pressStateLayerOpacity;",
            1,
        )
        errors = self.outlined_renderer_errors(render_helper=changed)
        self.assertTrue(any("state.isPressed()" in error for error in errors))

    def test_outlined_disabled_feedback_guards_are_required(self) -> None:
        changed = self.outlined_widget.replace(
            "if (isEnabled()) {\n        setRippleClipPath",
            "if (true) {\n        setRippleClipPath",
            1,
        ).replace(
            "if (isEnabled() && interactionState().isFocused())",
            "if (interactionState().isFocused())",
            1,
        )
        errors = self.outlined_renderer_errors(outlined_widget_source=changed)
        self.assertTrue(any("suppress ripple" in error for error in errors))
        self.assertTrue(any("suppress focus-ring" in error for error in errors))

    def test_outlined_direct_theme_access_is_rejected(self) -> None:
        changed = self.outlined_widget.replace(
            "spec.outlineColor",
            "button.theme().colorScheme().color(ColorRole::OutlineVariant)",
            1,
        )
        errors = self.outlined_renderer_errors(outlined_widget_source=changed)
        self.assertTrue(any("outside resolution" in error for error in errors))

    def test_text_variant_binding_is_required(self) -> None:
        changed = self.text_button.replace(
            "factory.textButtonSpec(theme(), density())",
            "factory.filledButtonSpec(theme(), density())",
            1,
        )
        errors = self.text_renderer_errors(text_button_source=changed)
        self.assertTrue(any("text ButtonSpec" in error for error in errors))

    def test_text_focus_width_must_come_from_resolved_spec(self) -> None:
        changed = self.text_button.replace(
            "spec.focusRingWidth);",
            "2.0);",
            1,
        )
        errors = self.text_renderer_errors(text_button_source=changed)
        self.assertTrue(any("hard-coded Material" in error for error in errors))

    def test_text_direct_theme_access_is_rejected(self) -> None:
        changed = self.text_button.replace(
            "spec.containerColor",
            "theme().colorScheme().color(ColorRole::Primary)",
            1,
        )
        errors = self.text_renderer_errors(text_button_source=changed)
        self.assertTrue(any("outside resolution" in error for error in errors))

    def test_text_disabled_feedback_guards_are_required(self) -> None:
        changed = self.text_button.replace(
            "if (isEnabled() && layerOpacity > 0.0)",
            "if (layerOpacity > 0.0)",
            1,
        ).replace(
            "if (isEnabled()) {\n  setRippleClipPath(path);",
            "if (true) {\n  setRippleClipPath(path);",
            1,
        ).replace(
            "isEnabled()\n  && interactionState().isFocused()",
            "interactionState().isFocused()",
            1,
        )
        errors = self.text_renderer_errors(text_button_source=changed)
        self.assertTrue(any("transitioning state layer" in error for error in errors))
        self.assertTrue(any("suppress ripple" in error for error in errors))
        self.assertTrue(any("suppress focus-ring" in error for error in errors))

    def test_text_icon_color_must_come_from_resolved_spec(self) -> None:
        changed = self.text_button.replace(
            "isEnabled() ? spec.iconColor : spec.disabledLabelColor",
            "isEnabled() ? spec.labelColor : spec.disabledLabelColor",
            1,
        )
        errors = self.text_renderer_errors(text_button_source=changed)
        self.assertTrue(any("spec.iconColor" in error for error in errors))

    def test_missing_reference_path_is_rejected_without_a_baseline(self) -> None:
        suite = copy.deepcopy(self.suite)
        suite["cases"][0]["assertions"].pop()
        _, errors = checker.collect_reference_values(suite)
        self.assertTrue(any("expected exactly one assertion" in error for error in errors))

    def test_renderer_vertical_covers_all_required_states(self) -> None:
        states = [
            state["id"]
            for state in self.model["states"]
            if state["required"] is True
        ]
        for suite in (
            self.suite,
            self.filled_tonal_suite,
            self.elevated_suite,
            self.outlined_suite,
            self.text_suite,
        ):
            self.assertEqual(states, suite["caseDimensions"]["states"])
            self.assertEqual(10, len(suite["cases"]))
        self.assertNotIn("selected", states)


if __name__ == "__main__":
    unittest.main()
