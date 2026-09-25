# ADR-008: ABI Policy Before and After 1.0
- **Status**: Accepted
- **Date**: 2026-08-15

## Context
The project is still reducing its public C++ surface before the first stable
release, so premature compatibility shims would freeze accidental API.

## Decision
Breaking source and ABI changes are allowed before 1.0 for architecture/API
cleanup. 0.9 is the API freeze candidate; 1.0 establishes the first supported
public-header/API baseline.

## Consequences
Pre-1.0 obsolete APIs are removed directly. Post-1.0 compatibility and
deprecation policy applies only after the stable baseline is published.

## Verification
The 0.9 audit freezes the intended public-header/package surface. The 1.0
release process records the stable baseline before 1.x compatibility guarantees
begin.
