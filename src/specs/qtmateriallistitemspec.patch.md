# ListItemSpec + SpecFactory Patch Direction

Add `QtMaterial::ListItemSpec` to the public specs surface.

## SpecFactory direction

Add:

```cpp
ListItemSpec listItemSpec(const Theme& theme, Density density) const;
```

Recommended mapping:
- `containerColor` -> `Surface`
- `hoveredContainerColor` -> derived surface container tint
- `selectedContainerColor` -> `SecondaryContainer` or selection surface role
- `pressedContainerColor` -> stronger state layer over surface
- `headlineColor` -> `OnSurface`
- `supportingTextColor` -> `OnSurfaceVariant`
- `leadingIconColor` -> `OnSurfaceVariant`
- `trailingIconColor` -> `OnSurfaceVariant`
- `dividerColor` -> `OutlineVariant`
- `stateLayerColor` -> `OnSurface`
- `focusRingColor` -> `Primary`
- disabled colors from the same disabled policy used elsewhere

## Density direction

- Compact -> `compactMinHeight`
- Standard -> `minHeight`
- Large -> `largeMinHeight`

## Family direction

`QtMaterialListItem` should be the reusable row primitive for:
- simple navigation lists
- settings rows
- one-line and two-line rows
- future list containers

Do not make it depend on a list container to be useful.
