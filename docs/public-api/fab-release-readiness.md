# Floating Action Button release readiness

This note defines the release contract for `QtMaterialFab` and `QtMaterialExtendedFab`.

## `QtMaterialFab`

`QtMaterialFab` is icon-only by design. Because there is no visible text, it requires a usable accessible name by default.

Supported naming sources, in priority order:

1. `accessibleName()`
2. `iconAccessibleName()`
3. `toolTip()`
4. `"Floating action button"` only when `requiresAccessibleName` is disabled

Recommended usage:

```cpp
auto* fab = new QtMaterial::QtMaterialFab(QIcon(":/icons/add.svg"), parent);
fab->setIconAccessibleName(QObject::tr("Create item"));
```

## `QtMaterialExtendedFab`

`QtMaterialExtendedFab` uses its visible text as the default accessible name. It may also use an explicit `accessibleName()` or tooltip when no visible text is present.

```cpp
auto* fab = new QtMaterial::QtMaterialExtendedFab(
    QIcon(":/icons/edit.svg"),
    QObject::tr("Compose"),
    parent);
```

## Keyboard contract

Both widgets use button semantics:

- `Return` activates the button.
- `Enter` activates the button.
- `Space` activates the button.
- the focus policy remains keyboard-focusable.

## Release scope

Both widgets are marked `usable` in `component-registry.json` once these conditions hold:

- constructors preserve their text/icon arguments;
- touch target size is at least Material's minimum;
- activation works via keyboard;
- accessibility naming is deterministic;
- public tests `tst_fab` and `tst_extendedfab` pass.
