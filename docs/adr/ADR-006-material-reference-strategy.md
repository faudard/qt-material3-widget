# ADR-006: Material Conformance Uses Versioned Evidence
- **Status**: Accepted
- **Date**: 2026-08-15

## Context
QtMaterial3 is a pre-1.0 brownfield library being reorganized around enforceable architecture and measurable Material conformance.

## Decision
Conformance is structural + visual + motion against a named reference version. Qt desktop adaptations are classified separately.

## Consequences
The decision is binding on new specs and migration plans. Breaking cleanup is acceptable before 1.0 when required to reach the target architecture.

## Verification
Release reports record profile/reference/version.
