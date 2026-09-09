# Feature Specification: Structural Conformance

**Feature Branch**: `014-structural-conformance`
**Status**: Phase 014.B complete / first structural and semantic vertical active
**Milestone**: Material Conformance Harness
**Created**: 2026-09-01

## Scope

The first structural vertical is the Classic small Filled Button at default
density with label-only content. It covers enabled, disabled, hovered, focused
and pressed states in light and dark themes.

## Requirements

- **SC-FR-001** Read expectations only from the active evidence-backed suite.
- **SC-FR-002** Compare all nine structural expectations with active product
  sources.
- **SC-FR-003** Verify the registry → public widget → resolver → `ButtonSpec`
  binding chain.
- **SC-FR-004** Verify the common and Filled variant component-token scopes.
- **SC-FR-005** Verify container, label, optional leading-icon, state-layer and
  focus-indicator implementation evidence.
- **SC-FR-006** Require the complete five-state/two-theme Cartesian vertical.
- **SC-FR-007** Fail on missing, duplicated or conflicting structural assertions.
- **SC-FR-008** Provide no baseline, suppression, allowlist or bypass mode.
- **SC-FR-009** Run the comparator through repository health and CI.
- **SC-FR-010** Keep the comparator Python-standard-library-only.
- **SC-FR-011** Compare Filled Button semantic color roles and disabled alpha in
  a separate evidence-backed suite.
- **SC-FR-012** Compare interaction opacity/type and effective per-state
  elevation for all ten cases.

## Non-goals for phase 014.A

014.A remains the structural certification. 014.B supplies the semantic values
consumed by phase 015.A. Runtime Qt layout, animation timing and pixels are not
certified by phase 014.
