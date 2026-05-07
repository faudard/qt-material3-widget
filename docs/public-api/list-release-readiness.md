# QtMaterialList release readiness

`QtMaterialList` is a lightweight Material 3 list container for `QtMaterialListItem` rows.

## Contract

The release-ready list contract covers:

- item ownership and insertion/removal;
- current index tracking;
- `NoSelection`, `SingleSelection` and `MultiSelection`;
- keyboard navigation with Up, Down, Left, Right, Home and End;
- activation with Space, Return and Enter;
- disabled-item skipping;
- accessible summary text for the list container;
- accessible text for each row.

## Keyboard behavior

| Key | Behavior |
| --- | --- |
| Up / Left | Move to previous enabled item |
| Down / Right | Move to next enabled item |
| Home | Move to first enabled item |
| End | Move to last enabled item |
| Space / Return / Enter | Activate the current item |

## Accessibility

`accessibilitySummary()` reports list size, current position and selected count.

`itemAccessibleText(index)` combines headline text, supporting text, selected state and disabled state.
