# Requirements Quality Checklist: Component Registry

- [x] Existing generator remains authoritative; no validation fork for current mechanics.
- [x] Schema is versioned independently.
- [x] Current registry file does not need a mass envelope migration.
- [x] `complete` has objective evidence requirements.
- [x] IDs/header/widget/route collisions are blocked.
- [x] Reference-candidate semantics are deterministic.
- [x] Legacy implicit metadata is warning-only in normal mode.
- [x] Strict mode is available.
- [x] Generated documents are checked, not hand-maintained.
- [x] Public helper headers are explicitly out of scope for registry completeness.
- [ ] Applied repository validation is green.
- [ ] GitHub quality job is green.
