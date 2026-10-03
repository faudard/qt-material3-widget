# Widgets

This is the consumer entry point for the widget catalog. Detailed family pages remain the canonical component documentation and exact signatures are available in the generated C++ API reference.

## Component documentation contract

Every public component documentation entry must answer the same questions:

1. **Screenshot** — where the component can be seen in the Gallery or deterministic visual reference.
2. **When to use** — the interaction problem the component solves.
3. **API** — public header, widget type and the important consumer-facing properties/signals.
4. **States** — enabled, disabled, hover, focus, pressed, selected/checked/error where applicable.
5. **Keyboard** — supported keys and native Qt behavior retained by the widget.
6. **Accessibility** — accessible naming, role/state/value semantics where applicable.
7. **RTL** — mirroring/direction behavior.
8. **Example** — Gallery route or focused example code.

The component registry is the source of truth for public header, widget type, test target, Gallery route, documentation path and maturity evidence. See the [component documentation standard](component-documentation-standard.md) for the release checklist and screenshot policy.

## Generated component pages

The [component pages](components/index.md) provide one searchable page per release widget, with embedded generated C++ API, Previous/Next navigation, Gallery route, maturity evidence, keyboard, accessibility and RTL contracts.

## Complete component reference

See the [release component reference](component-reference.md) for all 49 release-scoped public widgets, installed headers, Gallery routes, maturity and family guides.

## Families

- [Buttons](../public-api/buttons.md)
- [Selection](../public-api/selection.md)
- [Inputs](../public-api/inputs.md)
- [Navigation](../public-api/navigation.md)
- [Surfaces](../public-api/surfaces.md)
- [Data widgets](../public-api/data-widgets.md)
- [Progress indicators](../public-api/progress-indicators.md)
- [Compact controls](../public-api/compact-controls.md)
- [Desktop productivity and layouts](../public-api/desktop-productivity.md)

## Screenshots and live inspection

Run `qtmaterial3_gallery` for component-level inspection. The Gallery is preferred over stale hand-maintained screenshots because it exposes current states, theme modes, contrast and direction against the exact code being consumed.

Deterministic visual-regression captures complement the Gallery where a component has reviewed reference imagery.

## Exact API

Use the [generated C++ API reference](../api/index.md) for member signatures. Consumer guides intentionally focus on usage contracts rather than duplicating generated declarations.

## Maturity

See [Component status](../component-status.md) for release maturity. Maturity is evidence-driven and generated from `docs/components/component-registry.json`.
