# Feature Specification: Material Conformance Report

**Feature Branch**: `017-material-conformance-report`
**Status**: Phase 017.A complete / runtime visual evidence unavailable
**Milestone**: Material Conformance Harness
**Created**: 2026-09-09

## Scope

Phase 017.A aggregates the existing strict 013--016 evidence into one closed,
deterministic report for every component registry entry. It reports structure,
tokens, states, semantic rendering and visual rendering separately, then derives
the four public headline statuses. It does not create evidence, execute Qt or
promote the contract-only visual matrix.

## Requirements

- **MCH-FR-001** Report every component in exact registry order.
- **MCH-FR-002** Bind modeled components to their active Material profile and
  component model; report all other entries explicitly as `not-modeled`.
- **MCH-FR-003** Aggregate the reference-model, structural, renderer and visual
  contract gates through their `validate_tree` APIs.
- **MCH-FR-004** Preserve each gate's errors and propagate owning-gate failures
  to affected component dimensions.
- **MCH-FR-005** Derive structural counts only from evidence-backed structural
  suites.
- **MCH-FR-006** Split renderer evidence into token, state and complete semantic
  renderer counts.
- **MCH-FR-007** Treat a valid visual contract as `pending-runtime` until
  reviewed target references and comparison results are available.
- **MCH-FR-008** Never report a visual or overall pass from synthetic comparator
  tests or source inspection.
- **MCH-FR-009** Publish structure, tokens, states and rendering headline
  statuses for each component.
- **MCH-FR-010** Derive complete, incomplete, failed and not-modeled summary
  counts from component statuses.
- **MCH-FR-011** Validate the report with a closed versioned JSON Schema.
- **MCH-FR-012** Generate byte-stable JSON and fail when the committed report is
  stale.
- **MCH-FR-013** Independently lock the current 017.A evidence boundary in a
  strict checker.
- **MCH-FR-014** Reject escape markers and subprocess output parsing in the
  aggregation source.
- **MCH-FR-015** Run the harness through repository health after its four source
  gates.

## Current result

The report contains all 32 registry components. Five Classic Button variants
are modeled: one has structural evidence and all five have semantic renderer
evidence. No target has reviewed visual evidence. The result is therefore 0
fully conformant, 5 incomplete and 27 not modeled components, with root status
`incomplete`.

## Out of scope

This increment does not compile or run Qt, capture images, review references,
ingest comparison reports, add structural suites, add component models or alter
widget/rendering code.
