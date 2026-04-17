# qt-material3-widgets

Material 3 widget toolkit for Qt Widgets.

## Layered modules

- `qtmaterial3_theme` — tokens, schemes, builder, manager
- `qtmaterial3_legacy` — temporary migration bridge
- `qtmaterial3_core` — widget bases and interaction state
- `qtmaterial3_specs` — immutable component specs and factory
- `qtmaterial3_effects` — ripple, focus, shadow, transitions, scrim
- `qtmaterial3_widgets` — public widget implementations

## Phase 5 focus

- packaging and install/export
- accessibility and keyboard correctness
- performance and memory review
- public API stabilization
- documentation and release readiness


## Phase 6 additions

This scaffold extends the Phase 5 tree with:
- advanced inputs (`DatePicker`, autocomplete maturity)
- navigation (`NavigationRail`, `Menu`)
- compact controls (`Chip`, `SegmentedButton`)
- data widgets (`Table`, `GridList`, `Carousel`)
- gallery, docs, benchmarks, and test scaffolding for the new families

This remains a design skeleton. Several files are intentionally stubs so the repo structure stays clear before detailed implementation work begins.
