# Feature Specification: Typed Token System

**Feature Branch**: `010-typed-token-system`  
**Status**: Static application complete / Qt build gates pending
**Milestone**: 0.5 Architecture Foundation  
**Created**: 2026-08-15

## Audited state

The repository already uses typed domain enums such as `ColorRole`, `TypeRole`,
`ShapeRole`, `ElevationRole`, `MotionToken`, `DensityRole` and `IconSizeRole`. Their enum
ordinals are implicit, however, and ThemeSerializer currently duplicates textual name maps
for those token families. Current `ComponentId` is also defined in the Theme component-token
header rather than Foundation.

Spec 008 moved component textual aliases toward ThemeIO. Spec 010 generalizes identity
without replacing strongly typed domain APIs.

## Requirements

- **TTS-FR-001** Add a dependency-free 32-bit `TokenId` to Foundation.
- **TTS-FR-002** TokenId encoding is versioned and category/local-id based.
- **TTS-FR-003** Category numeric assignments are explicit and stable.
- **TTS-FR-004** Every current keyed token role has an explicit TokenId mapping independent of enum ordinal.
- **TTS-FR-005** Accessibility/interaction policy fields have reserved typed identity for provenance.
- **TTS-FR-006** Existing domain APIs continue to use their domain enums.
- **TTS-FR-007** No QVariant/TokenId universal runtime lookup is introduced.
- **TTS-FR-008** Move ComponentId to Foundation with explicit stable numeric IDs.
- **TTS-FR-009** ComponentId and TokenId remain distinct semantic concepts.
- **TTS-FR-010** ThemeModel token identity mapping contains no textual token names.
- **TTS-FR-011** ThemeTextCodec is the sole token ID ↔ persisted-name boundary.
- **TTS-FR-012** ThemeSerializer removes duplicated color/type/shape/elevation/motion/density/icon name arrays.
- **TTS-FR-013** Existing JSON canonical token names remain unchanged.
- **TTS-FR-014** Token-name parsing requires an explicit TokenCategory where leaf names are ambiguous.
- **TTS-FR-015** Raw IDs must never be silently renumbered or reused.
- **TTS-FR-016** Compile-time regression tests pin representative raw IDs.
- **TTS-FR-017** Tests verify uniqueness across all current role families.
- **TTS-FR-018** Repository health enforces the stable typed-token contract.
- **TTS-FR-019** Qt 5.14.2/Qt6 and shared/static remain mandatory gates.

## Non-goals

No generic QVariant token store. No DTCG parser yet. No generated token registry yet. No ABI
freeze before the dedicated API/ABI milestones.
