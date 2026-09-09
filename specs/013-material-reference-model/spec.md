# Feature Specification: Material Reference Model

**Feature Branch**: `013-material-reference-model`
**Status**: Phase 013.C complete / Button renderer evidence completed by phase 015.E
**Milestone**: Material Conformance Harness
**Created**: 2026-09-01

## Audited state

The component registry already identifies `button.filled` as the Buttons-family
`referenceCandidate` and contains all five labeled Classic button variants.

The repository had internal widget and render evidence but no active, versioned,
machine-readable external Material model. A prototype existed outside the active
tree; it lacked an explicit component-family schema and referenced a superseded
Expressive alpha release.

As reviewed on 2026-09-01, AndroidX Compose Material3 publishes stable 1.4.0 and
experimental 1.5.0-alpha27. Classic and Expressive must remain independent.

## Requirements

- **MRM-FR-001** Define versioned catalog, profile, component and suite JSON schemas.
- **MRM-FR-002** Catalog every active profile, component model and suite.
- **MRM-FR-003** Version Classic and Expressive independently.
- **MRM-FR-004** Pin Material guidance, AndroidX artifact and release provenance.
- **MRM-FR-005** Reject mutable AndroidX implementation references.
- **MRM-FR-006** Make source precedence explicit.
- **MRM-FR-007** Allow exactly `MUST_MATCH`, `PLATFORM_ADAPTATION` and
  `QTMATERIAL_EXTENSION`.
- **MRM-FR-008** Require rationale for adaptations and extensions.
- **MRM-FR-009** Cover geometry, tokens, colors, typography, states, motion,
  accessibility and behavior.
- **MRM-FR-010** Compare typography specification rather than raw glyph pixels.
- **MRM-FR-011** Define component family, required variants, slots, states, token
  roles and expected behavior explicitly.
- **MRM-FR-012** Bind component variants to real registry component IDs.
- **MRM-FR-013** Require the component-model anchor to remain a registry
  `referenceCandidate` of the same family.
- **MRM-FR-014** Bind every component token role to real `ResolvedSpec` paths.
- **MRM-FR-015** Map every public field of the bound `ResolvedSpec` exactly once,
  including explicit separation of runtime-support fields.
- **MRM-FR-016** Require every model item and evidence assertion to cite profile
  sources.
- **MRM-FR-017** Initial Classic Buttons coverage is exactly five variants, five
  required states and two theme modes.
- **MRM-FR-018** Generate the resulting 50 unique Cartesian cases deterministically.
- **MRM-FR-019** Require canonical case IDs.
- **MRM-FR-020** Forbid assertions in a seeded suite and empty cases in an
  evidence-backed suite.
- **MRM-FR-021** Detect uncatalogued reference files.
- **MRM-FR-022** Introduce no unsourced numerical Material expectation in 013.A.
- **MRM-FR-023** Run the model checker through repository health.
- **MRM-FR-024** Keep all tooling Python-standard-library-only.
- **MRM-FR-025** Pin exact source evidence to the AndroidX 1.4.0 release-tip
  revision published by the official release notes.
- **MRM-FR-026** Distinguish complete `full-model` suites from separately
  cataloged `vertical-slice` suites.
- **MRM-FR-027** Require every evidence assertion to carry a file/line locator
  and at least one revision-pinned source.
- **MRM-FR-028** Cover the Filled Button structural fixture across all five
  required states and both themes as exactly 10 cases.
- **MRM-FR-029** Generate the 10 cases and 90 assertions deterministically.
- **MRM-FR-030** Keep the 50-case full Buttons seed fully seeded; never partially
  promote it through the vertical suite.
- **MRM-FR-031** Generate a separate renderer-token suite for the same 10-case
  vertical without weakening the structural suite.
- **MRM-FR-032** Locate all 100 renderer assertions in the revision-pinned
  AndroidX source tree.
- **MRM-FR-033** Catalog a separate 10-case, 100-assertion Elevated Button
  renderer suite without changing the full-model seed.
- **MRM-FR-034** Keep Filled and Elevated renderer-suite construction
  deterministic through one shared declarative builder.
- **MRM-FR-035** Catalog a separate 10-case, 100-assertion Filled Tonal Button
  renderer suite through the same declarative builder.
- **MRM-FR-036** Catalog a separate 10-case, 130-assertion Outlined Button
  renderer suite, including outline role, alpha and width.
- **MRM-FR-037** Map the resolved Outlined width as a first-class semantic token
  and retain exhaustive `ButtonSpec` coverage at 40/40 fields.
- **MRM-FR-038** Catalog a separate 10-case, 100-assertion Text Button renderer
  suite and complete evidence for all five required Button variants.

## Phase boundary

013.B extracts structural expectations for the Classic small Filled Button,
default density and label-only content. 013.C adds its semantic renderer suite;
phases 015.B-E extend renderer evidence to Elevated, Filled Tonal, Outlined and
Text using the same fixture and model. Runtime image comparison (016) and
aggregate component results (017) remain separate.
