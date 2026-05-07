# QtMaterialCard release readiness

`QtMaterialCard` is now treated as a usable surface component.

## Public contract

The card exposes:

- `titleText`
- `bodyText`
- `variant`
- `interactive`
- `accessibilitySummary`

Supported variants:

- `Elevated`
- `Filled`
- `Outlined`

## Interaction contract

Cards are non-interactive by default.

When `interactive` is enabled:

- the card receives strong keyboard focus;
- `Space`, `Return`, and `Enter` activate the card;
- mouse press/release emits `pressed`, `released`, and `clicked`;
- hover/pressed/focus states are painted.

## Accessibility contract

The accessible name defaults to the title when available, otherwise to `Card`.

The accessible description is synchronized from `accessibilitySummary()`:

```text
<title>. <body>
```

Application code may still override `accessibleName` after setting the title.
