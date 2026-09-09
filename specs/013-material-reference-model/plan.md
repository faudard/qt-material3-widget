# Implementation Plan

## Phase 013.A — active seeded model

1. Audit the registry, reference candidate and `ButtonSpec` surface.
2. Publish schemas for profiles, component models and suites.
3. Pin independent Classic stable and Expressive experimental profiles.
4. Define the Classic Buttons variants, slots, states, semantic token roles,
   behaviors and exhaustive `ButtonSpec` binding.
5. Derive a deterministic 50-case seed from the required model axes.
6. Validate catalog completeness, provenance, registry linkage, field coverage and
   suite identity using only the Python standard library.
7. Add negative tests and integrate the checker into repository health.

## Phase 013.B — evidence population

1. Pin the AndroidX 1.4.0 release-tip source revision.
2. Extract source-backed structural expectations for the small Filled Button.
3. Record immutable file/line locations for every assertion.
4. Generate a separate 10-case evidence vertical across all required states and
   both themes.
5. Preserve the full 50-case seed without partial promotion.
6. Hand the completed vertical to the phase 014 structural comparator.

All six steps are complete. No numerical expectation is populated without its
upstream evidence locator.
