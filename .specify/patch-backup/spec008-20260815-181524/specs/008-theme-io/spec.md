# Feature Specification: ThemeIO

**Feature Branch**: `008-theme-io`
**Created**: 2026-08-15
**Status**: Seeded; run `/speckit.analyze` before implementation
**Program/Milestone**: 0.5 Architecture Foundation
**Master-plan IDs**: THEME-009, THEME-010, THEME-011, THEME-012, ARCH-012

## Problem Statement
Serialization, XML compatibility, textual token keys and serialization convenience currently leak beyond a dedicated IO boundary. The architecture baseline attributes 37 violations to ThemeIO/text-key ownership.

## Scope
### In Scope
- Own JSON serialization/version schema/migration.
- Own XML legacy adapter.
- Own text-key <-> typed-ID mappings.
- Remove persistence ownership from ThemeManager/Context.
- Drive text-key architecture category to zero with 010.
### Out of Scope
- Theme runtime propagation.
- Dropping XML immediately.
- Material conformance.

## User / Maintainer Scenarios
### US1 - Theme round-trips without runtime (P1)

**Value**: Persistence must not require singleton/runtime.

**Independent Test**: JSON round-trip ThemeModel value.

**Acceptance Scenarios**
1. **Given** valid Theme, **When** JSON round-trip completes, **Then** equivalent model data is preserved.

### US2 - Legacy XML is isolated (P2)

**Value**: Compatibility should not contaminate model/specs.

**Independent Test**: Inspect link graph.

**Acceptance Scenarios**
1. **Given** ThemeModel/Specs, **When** linked, **Then** XML adapter is absent.


## Requirements
### Functional Requirements
- **FR-001**: Create versioned JSON schema with schemaVersion.
- **FR-002**: Keep/move serializer and XML adapter in ThemeIO.
- **FR-003**: Own all textual token conversion.
- **FR-004**: Define strict parse/migration diagnostics.
- **FR-005**: Move serialization convenience out of ThemeManager/Context.
### Architecture Requirements
- **AR-001**: ThemeIO -> ThemeModel; never reverse.
- **AR-002**: Specs/renderer paths do not link ThemeIO.
- **AR-003**: Text conversion outside IO/tooling reaches zero.
### Compatibility Requirements
- **CR-001**: Qt5/Qt6 compile.
- **CR-002**: Existing JSON workflows have migration path.
### Quality Requirements
- **QR-001**: New or changed behavior has automated evidence.
- **QR-002**: Fixed architecture debt is removed from the baseline in the same change.
- **QR-003**: Affected docs/registry/ADR artifacts are synchronized.

## Edge Cases
- A partial migration must not leave an installable public header whose implementation is not linkable.
- Optional backends/features must fail or fall back explicitly, never silently change semantics.
- CI must distinguish unsupported environment/setup failure from product regression.

## Success Criteria
- **SC-001**: JSON round-trip and invalid-version tests pass.
- **SC-002**: ThemeModel/Specs link without ThemeIO.
- **SC-003**: Text-key category reaches zero after 010 integration.

## Assumptions
- XML remains legacy-supported through 0.5 unless separately removed.

## Dependencies
- 006 split.
- 007 model.
- 010 tokens.

## Traceability
See `tasks.md` and `../MASTER-PLAN-MAPPING.md`.
