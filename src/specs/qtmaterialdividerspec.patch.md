# SpecFactory Patch Direction for DividerSpec

Add a divider-spec builder once the data/display family is integrated.

Suggested API:

```cpp
QtMaterial::DividerSpec dividerSpec(const Theme& theme, Density density,
                                    Qt::Orientation orientation) const;
```

Suggested population:
- `color` from outline variant or surface variant line role
- `thickness` default 1
- `orientation` from caller
- `margins` from container spacing guidance
- `leadingInset` and `trailingInset` default 0
```
