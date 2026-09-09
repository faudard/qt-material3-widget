# Feature Specification: Renderer Conformance

**Feature Branch**: `015-renderer-conformance`
**Status**: Phase 015.E complete / all five Button renderer verticals active
**Milestone**: Material Conformance Harness
**Created**: 2026-09-01

## Scope

The active renderer scope is the Classic small Text, Filled, Filled Tonal,
Elevated and Outlined Buttons at default density with label-only content. All
five cover enabled, disabled, hovered, focused and pressed states in light and
dark themes.

## Requirements

- **RC-FR-001** Read expected renderer values only from the five evidence-backed
  10-case suites.
- **RC-FR-002** Compare all 530 semantic renderer assertions.
- **RC-FR-003** Resolve disabled container/content roles and alpha before paint.
- **RC-FR-004** Resolve hover/focus/press opacity from state tokens.
- **RC-FR-005** Resolve effective elevation for every required state.
- **RC-FR-006** Make press ripple-owned; do not double-paint a fixed press layer.
- **RC-FR-007** Suppress state-layer, ripple and focus-ring feedback while
  disabled.
- **RC-FR-008** Require paint and private render helpers to consume only
  `ButtonSpec` for Material values.
- **RC-FR-009** Reject Material semantic literals in renderer regions.
- **RC-FR-010** Resolve the Qt focus-ring width before paint.
- **RC-FR-011** Exercise mutations for roles, alpha, elevation, state ownership,
  disabled guards, direct theme access and literals.
- **RC-FR-012** Provide no debt file, exception list or alternate pass mode.
- **RC-FR-013** Run through repository health and CI.
- **RC-FR-014** Keep the checker Python-standard-library-only.
- **RC-FR-015** Resolve distinct normal and hover elevation styles and verify
  exact Level0/Level1/Level2 state semantics.
- **RC-FR-016** Require Elevated Button to bind its variant spec and inherit the
  certified shared renderer without a private paint bypass.
- **RC-FR-017** Require Filled Tonal Button to bind its variant spec, retain its
  component identity in every constructor and inherit the certified renderer.
- **RC-FR-018** Enforce Filled Tonal `SecondaryContainer`/
  `OnSecondaryContainer` enabled semantics and `OnSurface` 12%/38% disabled
  semantics through typed component-token roles.
- **RC-FR-019** Enforce Outlined transparent containers,
  `OnSurfaceVariant` content and `OutlineVariant` border semantics.
- **RC-FR-020** Resolve the 1 dp outline width into `ButtonSpec`; renderer and
  private helper must not derive it from device-pixel ratio or literals.
- **RC-FR-021** Require the distinct Outlined paint path to consume only
  resolved values and suppress disabled ripple/focus feedback.
- **RC-FR-022** Keep pressed Outlined feedback ripple-owned without a duplicate
  fixed state layer.
- **RC-FR-023** Enforce Text transparent containers, `Primary` enabled content,
  `OnSurfaceVariant` disabled content at 38% and `Level0` in every state.
- **RC-FR-024** Preserve Text semantic roles and disabled alpha through the
  typed `ButtonText` override branch without injecting an outline role.
- **RC-FR-025** Require the distinct Text paint path to consume container,
  label, icon, interaction and focus values only from `ButtonSpec`.
- **RC-FR-026** Reject disabled Text state-layer, ripple and focus feedback, and
  reject any non-zero Material literal or direct material color in paint.
- **RC-FR-027** Cover every required Button variant from the reference model.

## Non-goals for phase 015.E

This phase does not certify other component families. It does not compare Qt
runtime geometry, timing curves or pixels.
