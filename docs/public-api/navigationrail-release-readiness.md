# NavigationRail release readiness

`QtMaterialNavigationRail` is promoted from planned to usable once the widget exposes a stable destination model, keyboard navigation and an accessibility contract.

## Public contract

- Destinations are ordered and addressable by index.
- `currentIndex` is `-1` when no enabled destination is selected.
- Disabled destinations cannot become current and are skipped by keyboard navigation.
- `destinationActivated(int)` is emitted only for the current enabled destination.
- `labelsVisible` affects presentation but not the accessible destination text.

## Keyboard contract

| Key | Behavior |
|---|---|
| Up / Left | Select previous enabled destination, wrapping around |
| Down / Right | Select next enabled destination, wrapping around |
| Home | Select first enabled destination |
| End | Select last enabled destination |
| Space / Return / Enter | Activate current enabled destination |

## Accessibility contract

The rail has a default accessible name of `Navigation rail` and keeps `accessibleDescription` synchronized with `accessibilitySummary()`.

`destinationAccessibleText(index)` includes:

- destination label;
- 1-based position;
- selected state when applicable;
- disabled state when applicable.

This is intentionally separate from automation metadata. Tests should use object names or dynamic properties for automation, not localized accessible strings.
