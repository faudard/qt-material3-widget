# Filled Text Field Hardening Checklist

This pass keeps `QtMaterialFilledTextField` aligned with the shared text-field shell while tightening the variant for real use.

## Goals

- keep the filled variant on the shared shell architecture
- add a convenience constructor for label-first construction
- ensure content-driven shell invalidation is reflected in tests
- add a focused-state render baseline alongside default and error baselines

## What should remain shared

- shell geometry caching
- supporting/error text elision
- focus/error state handling
- line-edit palette syncing
- accessibility synchronization
- `contentChangedEvent()` invalidation path from the outlined base

## What should differ by variant

- container fill treatment
- filled-shell focus and indicator visuals
- spec selection (`filledTextFieldSpec` vs `outlinedTextFieldSpec`)
