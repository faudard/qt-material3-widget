# ADR-005: ThemeManager Is an Optional Global Fallback
- **Status**: Accepted
- **Date**: 2026-08-15

## Context
QtMaterial3 is a pre-1.0 brownfield library being reorganized around enforceable architecture and measurable Material conformance.

## Decision
Effective resolution is explicit context -> inherited context -> optional global/default provider. Renderers never query ThemeManager.

## Consequences
The decision is binding on new specs and migration plans. Breaking cleanup is acceptable before 1.0 when required to reach the target architecture.

## Verification
Context lifecycle tests prove explicit/inherited/global precedence.
