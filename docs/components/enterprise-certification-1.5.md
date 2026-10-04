# QtMaterial3 1.5 Enterprise certification closure

This document records the reviewed visual promotion performed from the successful
PR #72 CI run 643 on the pinned Qt 6.4.0 / Fusion / xcb rendering profile.

## Reviewed visual evidence

The following release families are promoted from candidate-only evidence to committed
stable goldens in light standard, dark standard and light high-contrast variants:

- Selection: `selection_matrix`.
- Inputs: `input_field_matrix`, `input_composite_matrix`, `input_slider_matrix`.
- Surfaces: `surface_bar_matrix`, `surface_overlay_matrix`.
- Data: `desktop_data_matrix`, `data_extended_matrix`.
- Progress + Compact: `progress_compact_matrix`.
- Layouts: `layout_matrix`.
- Navigation/Desktop: primary, desktop, split and component focus matrices.

The Navigation/Desktop evidence was captured twice independently. Its repeatability
report records Qt 6.4.0, Fusion, xcb, scale factor 1 and font DPI 96 and reports
byte-identical PNGs for all 27 Navigation/Desktop cases.

## Current promotion state

After visual promotion, **43/49** release-scoped components are `complete` with
all numeric maturity axes at 4/4. Navigation Rail, Tabs, Menu, Breadcrumb,
Command Palette and Split View have reviewed rendering at 4/4 but remain
`usable` with accessibility at 3/4.

The remaining blocker is intentionally manual: validate traversal, state announcements,
activation and focus behavior with **NVDA, Orca and VoiceOver**. Automated
`QAccessible` contract tests prove the Qt accessibility surface but do not prove spoken
output or every native OS bridge.

Until that evidence is recorded, `base.stable_release.enterprise_complete` remains
`false`; the release gate therefore stays fail-closed rather than claiming a false
49/49 Enterprise certification.

## Native screen-reader evidence gate

The canonical manual evidence ledger is
`docs/components/enterprise-accessibility-1.5.json`. It is intentionally checked in
with all results set to `pending` until an operator performs the real platform review.

The release checker validates the ledger on every stable-release check. While
`enterprise_complete` is `false`, pending records are structurally valid. Once
`enterprise_complete` is set to `true`, the checker requires all three platform
records to be `pass`, every component check to be `pass`, and non-empty reviewer,
reviewedAt and evidence fields.

Required platform sessions:

| Platform | Reader | Required environment |
| --- | --- | --- |
| Windows | NVDA | Primary Qt 5.14.2 / MSVC v142 lane |
| Linux | Orca / AT-SPI | Supported Qt 6 desktop lane |
| macOS | VoiceOver | Supported Qt 6 desktop lane |

For each reader, verify all six components: Navigation Rail, Tabs, Menu, Breadcrumb,
Command Palette and Split View. Each component must pass traversal, state announcements,
activation and focus behavior. Record the reviewer, date and a durable evidence reference
(log, recording, CI/manual artifact or review record) in the ledger. A failing bridge
behavior remains a registry gap and must not be converted to `pass`.

## Final closure procedure

1. Execute the NVDA, Orca and VoiceOver sessions and fill the evidence ledger with real results.
2. Require every one of the 72 checks (6 components × 4 behaviors × 3 readers) to be `pass`.
3. Raise the six accessibility axes from 3/4 to 4/4, clear their final gaps and promote them to `complete`.
4. Set `base.stable_release.enterprise_complete` to `true`.
5. Regenerate `STATUS.md`, `docs/component-status.md` and
   `docs/components/maturity.md`.
6. Run the strict release checker, package/consumer lanes, documentation,
   sanitizers and the full cross-platform CI matrix before tagging the release.
