# Implementation Plan: Remove Legacy SpecFactory

## Nature of this migration

This is no longer primarily a code rewrite. The audited `main` branch already has
resolver-oriented sources/public headers/tests. The remaining work is:

1. delete any legacy files that may still exist on a local/older branch;
2. correct stale current documentation;
3. document the replacement architecture;
4. add a permanent negative guardrail.

## Forbidden active surfaces

The checker scans:
- `include/`;
- `src/`;
- `tests/`;
- `examples/`;
- `cmake/`;
- `packaging/`;
- selected current top-level docs.

It intentionally does **not** ban historical discussion in Spec Kit records,
ADRs, changelogs or migration notes.

## Install-surface reasoning

Root CMake installs the full `include/` directory. Therefore the most reliable
way to guarantee that the facade is not installed is to guarantee that no legacy
header exists in the public include tree.

## Migration policy

Because the library is still pre-1.0, no deprecated compatibility facade is kept.
Downstream code migrates directly to the appropriate dedicated resolver.
