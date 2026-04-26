# Component token overrides

Component-local overrides are applied after the global resolved theme has produced a component spec. This keeps widgets rendering from immutable specs while still allowing local design deviations.

## Resolution order

Each factory method resolves tokens in this order:

1. Global `Theme` tokens.
2. Family-level override, for example `button`, `selection`, `input`, `surface`.
3. Component-level override, for example `button.filled`, `checkbox`, `textField.outlined`, `dialog`.
4. Legacy class-name aliases, for example `FilledButton`.

Later entries win.

## Supported component keys

| Component | Keys |
|---|---|
| Text button | `button`, `button.text`, `TextButton` |
| Filled button | `button`, `button.filled`, `FilledButton` |
| Filled tonal button | `button`, `button.filledTonal`, `FilledTonalButton` |
| Outlined button | `button`, `button.outlined`, `OutlinedButton` |
| Elevated button | `button`, `button.elevated`, `ElevatedButton` |
| FAB | `fab`, `Fab` |
| Extended FAB | `fab`, `fab.extended`, `ExtendedFab` |
| Icon button | `iconButton`, `IconButton` |
| Checkbox | `selection`, `checkbox`, `Checkbox` |
| Radio button | `selection`, `radioButton`, `RadioButton` |
| Switch | `selection`, `switch`, `Switch` |
| Outlined text field | `input`, `textField`, `textField.outlined`, `OutlinedTextField` |
| Filled text field | `input`, `textField`, `textField.filled`, `FilledTextField` |
| Card | `surface`, `card`, `Card` |
| Dialog | `surface`, `dialog`, `Dialog` |

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
        "elevationRole": "Level1",
        "motionToken": "Short4"
      }
    }
  }
}
```

The override layer intentionally lives in `SpecFactory`, not inside individual widget paint paths. Widgets continue to render from resolved specs.
