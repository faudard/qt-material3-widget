# ADR-008: ABI Policy Before and After 1.0
- **Status**: Accepted
- **Date**: 2026-08-15

## Context
QtMaterial3 is a pre-1.0 brownfield library being reorganized around enforceable architecture and measurable Material conformance.

## Decision
Breaking changes are allowed pre-1.0 for architecture/API cleanup; 0.9 freezes API candidates; 1.0 creates ABI baseline.

## Consequences
The decision is binding on new specs and migration plans. Breaking cleanup is acceptable before 1.0 when required to reach the target architecture.

## Verification
ABI checker gates 1.x after baseline.
