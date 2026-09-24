# ADR-004: Typed Runtime Token Identifiers
- **Status**: Accepted
- **Date**: 2026-08-15

## Context
Runtime/spec resolution needs stable typed identifiers without leaking persisted
string keys into ThemeModel or Specs.

## Decision
Runtime/spec resolution uses typed IDs. Conversion between typed identifiers and
persisted text belongs to ThemeIO internals.

## Consequences
ThemeModel and Specs remain string-key free. The public persistence boundary is
`ThemeSerializer`; internal codecs may evolve before 1.0 without becoming an
application-facing API.

## Verification
Architecture checks reject textual theme-key manipulation outside ThemeIO.
