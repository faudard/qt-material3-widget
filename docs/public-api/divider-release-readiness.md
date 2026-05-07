# Divider release readiness

`QtMaterialDivider` is considered release-ready when it behaves as a lightweight visual separator by default, while still allowing explicit accessibility exposure when the separator carries semantic meaning.

## Public contract

- Horizontal and vertical orientation.
- Leading/trailing insets.
- Thickness clamped to at least `1`.
- Optional explicit color, otherwise palette-resolved.
- Decorative by default.
- Optional non-decorative accessibility label.
- Testable `lineRect()` geometry.
- RTL-aware horizontal leading/trailing insets.

## Accessibility

Most dividers are decorative. The default is therefore:

```cpp
divider->isDecorative() == true;
divider->accessibleName().isEmpty();
divider->accessibleDescription().isEmpty();
```

When a separator is meaningful for assistive technology:

```cpp
divider->setDecorative(false);
divider->setAccessibilityLabel(QStringLiteral("Sidebar separator"));
```

## RTL behavior

For horizontal dividers, `leadingInset` follows the layout direction:

- LTR: leading inset is applied from the left.
- RTL: leading inset is applied from the right.

Vertical dividers keep leading/trailing mapped to top/bottom.
