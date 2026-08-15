# Feature Specification: Typed Token System

**Feature Branch**: `010-typed-token-system`
**Created**: 2026-08-15
**Status**: Seeded; run `/speckit.analyze` before implementation
**Program/Milestone**: 0.5 Architecture Foundation
**Master-plan IDs**: THEME-002, THEME-003, THEME-004, THEME-005, THEME-006, THEME-007, THEME-008, THEME-009, ARCH-012

## Problem Statement
Resolvers and component-token code still contain textual component-key lists/string conversion overloads. This weakens refactoring/type safety and violates ThemeIO ownership.

## Scope
### In Scope
- Define typed token identities/categories.
- Migrate resolver key lists to typed IDs.
- Provide stable ThemeIO string mappings.
- Remove runtime string-key APIs.
- Test uniqueness/category/round-trip.
### Out of Scope
- Predefining every future Material token.
- Changing visual values except proven bug.
- Conformance scoring.

## User / Maintainer Scenarios
### US1 - Resolvers use typed identities (P1)

**Value**: Eliminates fragile string lookup.

**Independent Test**: Compile after string API removal.

**Acceptance Scenarios**
1. **Given** resolver code, **When** architecture scan runs, **Then** no component-key-list text violation exists.

### US2 - Persistence keeps stable text representation (P2)

**Value**: JSON/tooling still need names.

**Independent Test**: Round-trip ID through ThemeIO.

**Acceptance Scenarios**
1. **Given** known ID, **When** text->ID->text runs, **Then** mapping is deterministic.


## Requirements
### Functional Requirements
- **FR-001**: Define typed token ID/category types.
- **FR-002**: Migrate existing resolver/component token access.
- **FR-003**: Move text conversion to ThemeIO.
- **FR-004**: Remove runtime string-key overloads.
- **FR-005**: Reject unknown/duplicate mapping explicitly.
### Architecture Requirements
- **AR-001**: Typed IDs live in ThemeModel ownership.
- **AR-002**: ThemeIO owns text conversion.
- **AR-003**: Specs consume typed IDs without ThemeIO dependency.
### Compatibility Requirements
- **CR-001**: C++17/Qt5 compatible.
- **CR-002**: Persisted text keys remain stable unless schema migration explicitly changes them.
### Quality Requirements
- **QR-001**: New or changed behavior has automated evidence.
- **QR-002**: Fixed architecture debt is removed from the baseline in the same change.
- **QR-003**: Affected docs/registry/ADR artifacts are synchronized.

## Edge Cases
- A partial migration must not leave an installable public header whose implementation is not linkable.
- Optional backends/features must fail or fall back explicitly, never silently change semantics.
- CI must distinguish unsupported environment/setup failure from product regression.

## Success Criteria
- **SC-001**: ThemeIO/text-key architecture category reaches 0 with 008.
- **SC-002**: All known IDs round-trip.
- **SC-003**: Resolvers compile without runtime string lookup.

## Assumptions
- enum class or equivalent strongly typed IDs preferred.
- Strings remain allowed in ThemeIO/tooling/schema fixtures.

## Dependencies
- 006 split.
- 007 model.
- 008 IO.

## Traceability
See `tasks.md` and `../MASTER-PLAN-MAPPING.md`.
