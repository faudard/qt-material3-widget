# Feature Specification: Material Reference Model

**Feature Branch**: `013-material-reference-model`  
**Status**: Ready for application  
**Milestone**: Material Conformance Harness  
**Created**: 2026-08-15

## Audited state

The component registry already marks `button.filled` as the Buttons-family reference
candidate and contains the five Classic button variants used by the initial POC.

QtMaterial3 has internal rendering/maturity evidence, but that does not establish conformance
to a pinned external Material reference.

AndroidX currently publishes Compose Material3 stable 1.4.0 and alpha 1.5.0-alpha26.
Material 3 Expressive is an expansion of M3 and must be tracked independently from Classic.

## Requirements

- **MRM-FR-001** Define a versioned machine-readable Material reference profile schema.
- **MRM-FR-002** Define a machine-readable reference-suite/assertion schema.
- **MRM-FR-003** Classic and Expressive use independent profile IDs/directories/versions.
- **MRM-FR-004** Profiles record Material guideline, AndroidX implementation and release provenance.
- **MRM-FR-005** Source precedence is explicit.
- **MRM-FR-006** Conformance classifications are exactly MUST_MATCH,
  PLATFORM_ADAPTATION and QTMATERIAL_EXTENSION.
- **MRM-FR-007** Comparison domains cover geometry/tokens/colors/typography/states/motion/
  accessibility/behavior.
- **MRM-FR-008** Typography policy compares spec, not raw glyph pixels.
- **MRM-FR-009** Every evidence assertion cites at least one profile source.
- **MRM-FR-010** Evidence-backed cases cannot have zero assertions.
- **MRM-FR-011** Seeded cases are permitted before structural evidence extraction.
- **MRM-FR-012** Suite component IDs must exist in the component registry.
- **MRM-FR-013** Suite anchor must be a registry `referenceCandidate`.
- **MRM-FR-014** Initial Classic Buttons POC is exactly 5 variants × 5 states × 2 themes.
- **MRM-FR-015** Initial POC therefore contains exactly 50 unique Cartesian cases.
- **MRM-FR-016** The generated POC is deterministic and checked for drift.
- **MRM-FR-017** Expressive alpha data cannot be used as a Classic baseline.
- **MRM-FR-018** No unsourced numerical Material expectation is introduced by Spec 013.
- **MRM-FR-019** Repository health validates reference-model integrity.
- **MRM-FR-020** Reference model tooling is Python-standard-library-only.

## Non-goals

Spec 013 does not yet extract complete numeric button geometry/tokens. That is Spec 014.
It does not yet build/run the Android reference renderer (Spec 015), compare images
(Spec 016), generate scoring reports (Spec 017), or compare motion frames (Spec 018).
