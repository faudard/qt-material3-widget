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

## Final closure procedure

1. Record NVDA, Orca and VoiceOver evidence for the six remaining components.
2. Raise their accessibility axes from 3/4 to 4/4 and clear the final gaps.
3. Set `base.stable_release.enterprise_complete` to `true`.
4. Regenerate `STATUS.md`, `docs/component-status.md` and
   `docs/components/maturity.md`.
5. Run the strict release checker, package/consumer lanes, documentation,
   sanitizers and the full cross-platform CI matrix before tagging the release.
