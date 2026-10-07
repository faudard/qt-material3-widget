# Widgets

This is the consumer entry point for the widget catalog. Detailed family pages remain the canonical behavioral guides and exact signatures are available in the generated C++ API reference.

## Component documentation contract

Every public registry component gets the same documentation shape:

1. **Screenshot** — reviewed golden when one can be matched automatically, otherwise the maintained Gallery deep link.
2. **When to use** — the interaction problem the component solves.
3. **API** — public header, widget type and important consumer-facing extension points.
4. **States** — enabled, disabled, hover, focus, pressed, selected/checked/error where applicable.
5. **Keyboard** — supported keys and native Qt behavior retained by the widget.
6. **Accessibility** — accessible naming, role/state/value semantics where applicable.
7. **RTL** — mirroring/direction behavior.
8. **Example** — Gallery route, live property inspection and copyable snippets.

The component registry remains the source of truth for component identity, public header, widget type, test target, Gallery route, documentation path and maturity evidence.

## Generated component pages

The [component pages](components/index.md) are generated for the complete public registry, currently **60 components**, including release-scoped and newer catalogue entries. Each page includes Previous/Next navigation, Gallery deep link, maturity evidence, keyboard, accessibility and RTL contracts.

## Release component reference

See the [release component reference](component-reference.md) for the stricter release-scoped certification set. Release certification and full-catalog documentation intentionally remain separate concepts.

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
- [Expressive catalogue](../public-api/expressive-catalogue.md)

## Gallery 2.0

Run `qtmaterial3_gallery` for global component search, family navigation, deep links, Light/Dark/High Contrast/Expressive preview, LTR/RTL, density switching, common state preview, live properties and copyable C++/`.ui` snippets.

Examples:

```bash
qtmaterial3_gallery --route /buttons/filled
qtmaterial3_gallery /navigation/command-palette
```

Reviewed deterministic visual-regression captures are automatically reused by generated documentation when the documentation pipeline can match them to a component.

## Exact API

Use the [generated C++ API reference](../api/index.md) for member signatures. Consumer guides intentionally focus on usage contracts rather than duplicating generated declarations.

## Maturity

See [Component status](../component-status.md) for release maturity. Maturity is evidence-driven and generated from `docs/components/component-registry.json`.
