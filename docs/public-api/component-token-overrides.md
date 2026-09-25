# Component token overrides

Component-local overrides are applied after the global resolved theme has produced a component spec. This keeps widgets rendering from immutable specs while still allowing local design deviations.

## Resolution order

Each component resolver applies tokens in this order:

1. Global `Theme` tokens.
2. Family-level override, for example `button`, `selection`, `input`, `surface`.
3. Component-level override, for example `button.filled`, `checkbox`, `textField.outlined`, `dialog`.

Later entries win.

## Supported component keys

| Component | Keys |
|---|---|
| Text button | `button`, `button.text` |
| Filled button | `button`, `button.filled` |
| Filled tonal button | `button`, `button.filledTonal` |
| Outlined button | `button`, `button.outlined` |
| Elevated button | `button`, `button.elevated` |
| FAB | `fab` |
| Extended FAB | `fab`, `fab.extended` |
| Icon button | `iconButton` |
| Checkbox | `selection`, `checkbox` |
| Radio button | `selection`, `radioButton` |
| Switch | `selection`, `switch` |
| Outlined text field | `input`, `textField`, `textField.outlined` |
| Filled text field | `input`, `textField`, `textField.filled` |
| Card | `surface`, `card` |
| Dialog | `surface`, `dialog` |

## Custom scalar keys

The typed token maps are preferred for colors, shapes, elevation, motion, density, icon sizes, and state layer. Component-specific geometry that does not exist in the generic token model uses the `custom` object.

Examples:

```json
{
  "componentOverrides": {
    "button.filled": {
      "colors": {
        "Primary": "#6750A4FF",
        "OnPrimary": "#FFFFFFFF"
      },
      "iconSizes": {
        "Small": 20
      },
      "custom": {
        "containerHeight": 44,
        "horizontalPadding": 28,
        "iconSpacing": 10,
        "shapeRole": "Full",
        "elevationRole": "Level0",
        "hoverElevationRole": "Level1",
        "motionToken": "Short4"
      }
    }
  }
}
```

Buttons resolve `elevationRole` and `hoverElevationRole` into two concrete
`ElevationStyle` values before paint. Filled and Filled Tonal use Level0 →
Level1; Elevated uses Level1 → Level2. Component elevation-map overrides
therefore remain exact at both endpoints instead of being approximated from one
style. Filled Tonal also preserves its distinct disabled semantics when colors
are overridden: `OnSurface` at 12% for the container and 38% for content.
Outlined preserves `OnSurfaceVariant` content and `OutlineVariant` borders,
including the disabled 38%/10% alpha split. Its `outlineWidth` custom scalar is
resolved into `ButtonSpec` before layout or paint.

The override layer intentionally lives in dedicated spec resolution, not inside
individual widget paint paths. Widgets continue to render from resolved specs.
