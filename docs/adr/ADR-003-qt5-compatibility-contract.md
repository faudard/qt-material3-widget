# ADR-003: Qt 5.14.2 Compatibility While Advertised
- **Status**: Accepted
- **Date**: 2026-08-15

## Context
QtMaterial3 advertises Qt 5.14.2 alongside Qt 6 and therefore needs executable
coverage for both compatibility branches.

## Decision
A real Qt 5.14.2/MSVC gate is mandatory while the project advertises that
baseline. Support cannot silently drift.

## Consequences
New API and implementation work must preserve the advertised Qt 5.14.2 baseline
or explicitly change the supported-platform contract.

## Verification
CI has a pinned Qt 5.14.2 gate and documentation matches it.
