# ADR-007: Material 3 Classic and Expressive Are Separate Profiles
- **Status**: Accepted
- **Date**: 2026-08-15

## Context
QtMaterial3 is a pre-1.0 brownfield library being reorganized around enforceable architecture and measurable Material conformance.

## Decision
Expressive support must not silently overwrite classic behavior.

## Consequences
The decision is binding on new specs and migration plans. Breaking cleanup is acceptable before 1.0 when required to reach the target architecture.

## Verification
Reference manifests and feature switches/capabilities remain separate.
