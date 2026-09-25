# ADR-005: ThemeManager Is an Optional Global Fallback
- **Status**: Accepted
- **Date**: 2026-08-15

## Context
Widgets need local/inherited theme contexts without forcing every renderer to
depend directly on the global ThemeManager singleton.

## Decision
Effective resolution is explicit context -> inherited context -> optional
global/default provider. Renderers never query ThemeManager directly.

## Consequences
Context-aware widgets resolve through ThemeContext boundaries; the global
manager remains an application convenience rather than a rendering dependency.

## Verification
Context lifecycle tests prove explicit/inherited/global precedence.
