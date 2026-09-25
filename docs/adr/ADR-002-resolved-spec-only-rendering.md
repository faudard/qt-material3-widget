# ADR-002: ResolvedSpec-Only Rendering
- **Status**: Accepted
- **Date**: 2026-08-15

## Context
Widget rendering needs one deterministic boundary between theme inputs and
layout/paint behavior.

## Decision
All render/paint helpers consume resolved values. Theme/ThemeContext/ThemeManager
access is restricted to resolution/runtime boundaries.

## Consequences
New component work must preserve resolved-spec rendering rather than introduce
theme lookups inside paint/layout paths.

## Verification
`ARCH-WIDGETS-RESOLVED-SPEC-ONLY` reaches zero and remains zero.
