# Buttons API

This page describes the public button family exposed by `qt-material3-widget`.

## Button family overview

The public include tree currently exposes the following button classes:

- `QtMaterialTextButton`
- `QtMaterialFilledButton`
- `QtMaterialFilledTonalButton`
- `QtMaterialOutlinedButton`
- `QtMaterialElevatedButton`
- `QtMaterialIconButton`
- `QtMaterialFab`
- `QtMaterialExtendedFab`

These classes live under:

```cpp
#include <qtmaterial/widgets/buttons/...>
```

## Base classes

### `QtMaterial::QtMaterialAbstractButton`

The main reusable button base. It provides:

- density handling
- interaction-state tracking
- theme access
- theme-change hooks
- content-change hooks
- keyboard and pointer event integration
- accessibility synchronization hooks

### `QtMaterial::QtMaterialTextButton`

A higher-level base used by text-bearing button variants. It adds:

- spec resolution for button visuals
- size hint computation
- ripple integration
- paint handling for text-button style content

## Notable concrete classes

### `QtMaterial::QtMaterialFilledButton`

A filled Material-style button built on top of `QtMaterialTextButton`.

The current public header shows responsibilities for:

- custom painting
- resize-time layout invalidation
- theme updates
- content updates
- button-spec resolution
- cached geometry/path layout

### `QtMaterial::QtMaterialIconButton`

A button centered around icon content, built directly on `QtMaterialAbstractButton`.

The current public header exposes:

- `icon()`
- `setIcon(...)`
- `sizeHint()`
- custom painting
- theme updates
- spec invalidation
- ripple integration

## Recommended documentation conventions

For each button class, the generated API page should eventually answer five questions clearly:

1. what visual Material role the widget represents
2. what content it accepts
3. what states it supports
4. what theme tokens it consumes
5. which behaviors differ from the Qt base class it extends

## Example includes

```cpp
#include <qtmaterial/widgets/buttons/qtmaterialfilledbutton.h>
#include <qtmaterial/widgets/buttons/qtmaterialiconbutton.h>
#include <qtmaterial/widgets/buttons/qtmaterialtextbutton.h>
```

## Material 3 references

Use these upstream references for behavior and semantics:

- buttons overview: <https://m3.material.io/components/buttons/overview>
- shape guidance: <https://m3.material.io/styles/shape>
- motion guidance: <https://m3.material.io/styles/motion/overview/how-it-works>

## Next documentation step

Add Doxygen comments to each button header explaining:

- intended Material 3 variant
- supported icon/text combinations
- state-layer and ripple behavior
- size policy and density interaction
- theming hooks and spec resolution behavior
