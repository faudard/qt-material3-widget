# GridList release readiness

`QtMaterialGridList` is a Material-style grid/list control for compact, selectable item collections.

## Release contract

The component is considered `usable` when it provides:

- item insertion/removal through the public API;
- title and supporting text for each item;
- optional icons;
- disabled item state;
- single, multi, and no-selection modes;
- keyboard navigation with Left/Right/Up/Down/Home/End;
- keyboard activation with Space/Return/Enter;
- RTL-aware horizontal navigation;
- accessible summary for the whole grid;
- accessible text for each item;
- deterministic cell sizing for visual regression tests.

## Accessibility

The widget exposes a default accessible name of `Grid list`. Applications should replace it with a domain-specific label when the context is ambiguous.

Each item exposes a composed text:

```text
<title>, <supporting text>, Item n of total, Selected/Disabled
```

The widget-level accessible description summarizes item count, current item, and selected count.
