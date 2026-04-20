# SpecFactory Patch Direction for ListSpec

Add a list-spec builder once the data/display family is integrated.

Suggested API:

```cpp
QtMaterial::ListSpec listSpec(const Theme& theme, Density density) const;
```

Suggested population:
- `containerColor` from surface container role
- `focusRingColor` from primary or dedicated focus-ring role
- `dividerColor` from outline variant
- `selectionOverlayColor` from state-layer color path
- `shapeRole` from surface/list container role
- `minimumViewportSize` from design-system sizing guidance
- `showDividers` default false
```
