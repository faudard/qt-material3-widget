# ADR-003: Qt 5.14.2 Compatibility While Advertised
- **Status**: Accepted
- **Date**: 2026-08-15

## Context
QtMaterial3 is a pre-1.0 brownfield library being reorganized around enforceable architecture and measurable Material conformance.

## Decision
A real Qt 5.14.2/MSVC gate is mandatory while the project advertises that baseline. Support cannot silently drift.

## Consequences
The decision is binding on new specs and migration plans. Breaking cleanup is acceptable before 1.0 when required to reach the target architecture.

## Verification
CI has a pinned Qt5 gate and documentation matches it.
