# Feature Specification: Component Registry Source of Truth

**Feature Branch**: `002-component-registry-source-of-truth`
**Created**: 2026-08-15
**Status**: Seeded; run `/speckit.analyze` before implementation
**Program/Milestone**: 0.5 Architecture Foundation
**Master-plan IDs**: BASE-015, BASE-016, BASE-017, BASE-018, BASE-019, BASE-024

## Problem Statement
`docs/component-status.md` is already generated from `docs/components/component-registry.json`, but the registry must become the enforced canonical inventory for identity, maturity, headers, specs, tests, docs and gallery.

## Scope
### In Scope
- Version/validate registry schema.
- Deterministic status generation.
- Detect public component/header divergence.
- Enforce minimum evidence for `complete`.
### Out of Scope
- Completing partial components.
- Rewriting gallery implementation.
- Material conformance scoring.

## User / Maintainer Scenarios
### US1 - Single authoritative inventory (P1)

**Value**: Prevents drift.

**Independent Test**: Regenerate status from registry.

**Acceptance Scenarios**
1. **Given** valid registry, **When** generation runs, **Then** output is deterministic.
2. **Given** duplicate/missing artifact, **When** validation runs, **Then** it fails.

### US2 - Meaningful maturity (P2)

**Value**: `complete` must be objective.

**Independent Test**: Validate incomplete fixture marked complete.

**Acceptance Scenarios**
1. **Given** `complete` missing mandatory evidence, **When** validated, **Then** it fails.


## Requirements
### Functional Requirements
- **FR-001**: Define JSON schema.
- **FR-002**: Validate unique IDs and referenced artifacts.
- **FR-003**: Make status generation schema-aware/deterministic.
- **FR-004**: Define maturity-specific requirements.
- **FR-005**: Detect policy-defined public components absent from registry.
### Architecture Requirements
- **AR-001**: Registry/tooling stays outside Core.
- **AR-002**: Runtime widgets do not parse registry data.
### Compatibility Requirements
- **CR-001**: Deterministic on Windows/Linux.
- **CR-002**: Uses supported project Python.
### Quality Requirements
- **QR-001**: New or changed behavior has automated evidence.
- **QR-002**: Fixed architecture debt is removed from the baseline in the same change.
- **QR-003**: Affected docs/registry/ADR artifacts are synchronized.

## Edge Cases
- A partial migration must not leave an installable public header whose implementation is not linkable.
- Optional backends/features must fail or fall back explicitly, never silently change semantics.
- CI must distinguish unsupported environment/setup failure from product regression.

## Success Criteria
- **SC-001**: Duplicate/missing artifacts fail validation.
- **SC-002**: Generated status has clean diff.
- **SC-003**: `complete` cannot omit required evidence.

## Assumptions
- Existing JSON registry remains source; no second manifest.

## Dependencies
- 001 repository health.

## Traceability
See `tasks.md` and `../MASTER-PLAN-MAPPING.md`.
