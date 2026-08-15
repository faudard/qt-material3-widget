# Implementation Plan

## Phase A — Prepare

1. Install manifest generator/checker.
2. Bootstrap the manifest from the actual checkout.
3. Review all automatically classified private headers.
4. Fix public→private include leaks if any.
5. Run source-only header contract.

This phase does not change install semantics yet.

## Phase B — Seal

Seal refuses to proceed unless the source header contract is clean.

It then:
1. replaces directory-based header installation with explicit manifest installation;
2. replaces recursive-glob hygiene tests with manifest-driven per-header compile tests;
3. updates the installed-consumer matrix to compare package headers exactly;
4. adds the header contract to repository health;
5. reruns the full contract.

## Review policy

The bootstrap classification is a convenience for the existing tree, not an ongoing API
policy. After the first reviewed manifest lands, additions/removals are visible as manifest
diffs and must be reviewed as API-surface changes.
