# ADR-004: Typed Runtime Token Identifiers
- **Status**: Accepted
- **Date**: 2026-08-15

## Context
QtMaterial3 is a pre-1.0 brownfield library being reorganized around enforceable architecture and measurable Material conformance.

## Decision
Runtime/spec resolution uses typed IDs. Text key conversion belongs to ThemeIO/tooling.

## Consequences
The decision is binding on new specs and migration plans. Breaking cleanup is acceptable before 1.0 when required to reach the target architecture.

## Verification
`ARCH-THEME-IO-OWNS-TEXT-KEYS` reaches zero.
