# Feature Specification: ThemeRuntime

**Feature Branch**: `009-theme-runtime`
**Created**: 2026-08-15
**Status**: Seeded; run `/speckit.analyze` before implementation
**Program/Milestone**: 0.5 Architecture Foundation
**Master-plan IDs**: THEME-020, THEME-021, THEME-022, THEME-023, THEME-024, THEME-025

## Problem Statement
Runtime theming must stay convenient without turning ThemeManager into a hidden global dependency. ThemeContext, inheritance, observation, system theme and revisioning need explicit lifecycle semantics.

## Scope
### In Scope
- Own ThemeContext/Host, ThemeManager, Observer and SystemTheme runtime roles.
- Define explicit -> inherited -> optional global precedence.
- Define monotonic revision/change propagation.
- Test reparenting/destruction/runtime switching.
- Remove persistence responsibilities to ThemeIO.
### Out of Scope
- Theme value-model internals.
- Serialization.
- Per-widget cache strategy beyond runtime change signals.

## User / Maintainer Scenarios
### US1 - Local context works without global manager (P1)

**Value**: Needed for deterministic theming/tests.

**Independent Test**: Construct explicit context tree without global manager.

**Acceptance Scenarios**
1. **Given** explicit child context, **When** global theme changes, **Then** explicit context remains effective.
2. **Given** no explicit context and parent context exists, **When** resolved, **Then** parent is effective.

### US2 - Global fallback remains convenient (P2)

**Value**: App-wide theme is useful.

**Independent Test**: Switch global/default theme.

**Acceptance Scenarios**
1. **Given** no explicit/inherited context, **When** global revision changes, **Then** effective theme updates exactly as policy defines.


## Requirements
### Functional Requirements
- **FR-001**: Define/test context precedence.
- **FR-002**: Keep ThemeManager as optional fallback only.
- **FR-003**: Add monotonic revision semantics.
- **FR-004**: Define observer/reparent/destruction lifecycle.
- **FR-005**: Support system light/dark changes in runtime layer.
- **FR-006**: Remove IO responsibilities.
### Architecture Requirements
- **AR-001**: ThemeRuntime -> ThemeModel only for design data.
- **AR-002**: Renderer/paint does not query runtime directly.
- **AR-003**: Serialization absent after 008.
### Compatibility Requirements
- **CR-001**: Qt5/Qt6 event semantics supported.
- **CR-002**: No dangling QObject observer/filter relationships.
### Quality Requirements
- **QR-001**: New or changed behavior has automated evidence.
- **QR-002**: Fixed architecture debt is removed from the baseline in the same change.
- **QR-003**: Affected docs/registry/ADR artifacts are synchronized.

## Edge Cases
- A partial migration must not leave an installable public header whose implementation is not linkable.
- Optional backends/features must fail or fall back explicitly, never silently change semantics.
- CI must distinguish unsupported environment/setup failure from product regression.

## Success Criteria
- **SC-001**: Context tests cover explicit/inherited/global + reparent/destruction.
- **SC-002**: ThemeManager not required for local correctness.
- **SC-003**: Runtime owns no persistence API.

## Assumptions
- Core ThemeContextBinding remains widget-facing bridge.
- Platform system-theme capability may degrade explicitly.

## Dependencies
- 006 split.
- 007 model.
- 008 IO cleanup.

## Traceability
See `tasks.md` and `../MASTER-PLAN-MAPPING.md`.
