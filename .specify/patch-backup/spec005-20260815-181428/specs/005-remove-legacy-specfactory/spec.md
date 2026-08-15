# Feature Specification: Remove Legacy SpecFactory

**Feature Branch**: `005-remove-legacy-specfactory`
**Created**: 2026-08-15
**Status**: Seeded; run `/speckit.analyze` before implementation
**Program/Milestone**: 0.5 Architecture Foundation
**Master-plan IDs**: ARCH-002

## Problem Statement
Active `src/specs/CMakeLists.txt` is resolver-oriented and does not list SpecFactory. If stale SpecFactory headers remain under `include/`, root `install(DIRECTORY include/)` can still ship a dead API.

## Scope
### In Scope
- Audit all SpecFactory references.
- Delete obsolete header/source if present.
- Remove docs/tests/examples references.
- Add guard against reintroduction.
### Out of Scope
- Changing resolver behavior.
- Theme split.
- Broad spec redesign.

## User / Maintainer Scenarios
### US1 - No zombie public API (P1)

**Value**: Pre-1.0 is the right time to remove it.

**Independent Test**: Search source/install tree and build consumer.

**Acceptance Scenarios**
1. **Given** installed package, **When** headers are enumerated, **Then** no obsolete SpecFactory header exists.


## Requirements
### Functional Requirements
- **FR-001**: Remove inactive SpecFactory header/source.
- **FR-002**: Remove all active references.
- **FR-003**: Prevent reintroduction through header/architecture audit.
### Architecture Requirements
- **AR-001**: Do not create compatibility facade unless explicit support contract requires it.
- **AR-002**: Resolvers remain the single spec path.
### Compatibility Requirements
- **CR-001**: Breaking removal allowed pre-1.0.
- **CR-002**: Install consumer remains green.
### Quality Requirements
- **QR-001**: New or changed behavior has automated evidence.
- **QR-002**: Fixed architecture debt is removed from the baseline in the same change.
- **QR-003**: Affected docs/registry/ADR artifacts are synchronized.

## Edge Cases
- A partial migration must not leave an installable public header whose implementation is not linkable.
- Optional backends/features must fail or fall back explicitly, never silently change semantics.
- CI must distinguish unsupported environment/setup failure from product regression.

## Success Criteria
- **SC-001**: No active SpecFactory reference/header/symbol remains.
- **SC-002**: Install consumer and specs tests pass.

## Assumptions
- 0.4 API not frozen.

## Dependencies
- 001 health.
- 003 consumer validation.

## Traceability
See `tasks.md` and `../MASTER-PLAN-MAPPING.md`.
