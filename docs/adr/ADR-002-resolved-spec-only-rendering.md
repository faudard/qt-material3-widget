# ADR-002: ResolvedSpec-Only Rendering
- **Status**: Accepted
- **Date**: 2026-08-15

## Context
QtMaterial3 is a pre-1.0 brownfield library being reorganized around enforceable architecture and measurable Material conformance.

## Decision
All render/paint helpers consume resolved values. Theme/ThemeContext/ThemeManager access is restricted to resolution/runtime boundaries.

## Consequences
The decision is binding on new specs and migration plans. Breaking cleanup is acceptable before 1.0 when required to reach the target architecture.

## Verification
`ARCH-WIDGETS-RESOLVED-SPEC-ONLY` reaches zero and remains zero.
