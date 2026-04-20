# Filled Text Field Hardening Checklist

This pass finalizes `QtMaterialFilledTextField` as a real variant of the shared
text-field shell architecture.

## Goals

- reuse the shared shell from `QtMaterialOutlinedTextField`
- resolve `filledTextFieldSpec(...)` through `SpecFactory`
- keep focus forwarding on the hosted `QLineEdit`
- ensure accessibility metadata flows to the internal editor
- add render baseline scaffolding for the filled variant

## What should remain shared

- shell geometry caching
- supporting/error text elision
- focus/error state handling
- line-edit palette syncing
- accessibility synchronization

## What should differ by variant

- container fill treatment
- outline/bottom-line visual behavior
- spec selection (`filledTextFieldSpec` vs `outlinedTextFieldSpec`)
