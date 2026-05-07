# IconButton accessibility contract

`QtMaterialIconButton` is icon-only, so applications must provide a usable accessible label unless the button is explicitly decorative.

Recommended usage:

```cpp
auto* button = new QtMaterial::QtMaterialIconButton(QIcon(QStringLiteral(":/icons/search.svg")), parent);
button->setAccessibleName(QStringLiteral("Search"));
button->setToolTip(QStringLiteral("Search"));
```

The widget now exposes a small guardrail API:

```cpp
bool requiresAccessibleName() const noexcept;
void setRequiresAccessibleName(bool required);
bool hasUsableAccessibleName() const;
QString effectiveAccessibleName() const;
```

Fallback order for `effectiveAccessibleName()`:

1. explicit `accessibleName()`;
2. tooltip;
3. button text, if a subclass or test sets one.

Keyboard contract:

- `Space` activates the button through Qt's button behavior;
- `Return` and `Enter` also trigger `clicked()` through `QtMaterialAbstractButton`;
- checkable icon buttons mirror `checked` into `selected` for Material 3 selected-state rendering.

Release checklist:

- icon-only actions have an accessible name;
- tooltip fallback is available for legacy call sites;
- touch target stays at least 48 x 48 logical pixels;
- checked icon buttons render the selected state and expose stable keyboard activation.
