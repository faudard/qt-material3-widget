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

The main reusable button base.

It provides:

- density handling
- interaction-state tracking
- theme access
- theme-change hooks
- content-change hooks
- keyboard and pointer event integration
- accessibility synchronization hooks

### `QtMaterial::QtMaterialTextButton`

A higher-level base used by text-bearing button variants.

It adds:

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

### `QtMaterial::QtMaterialFab`

The floating action button is the compact promoted-action variant.

Expected behavior in this library:

- icon-first content model
- circular visual container sized from the FAB spec
- touch target enforced independently from the visible diameter
- inherits filled-button interaction, ripple, and theming behavior
- intentionally non-checkable

Accessibility guidance:

- because the compact FAB is icon-only, always set an accessible name
- use a tooltip when the action is not already obvious from the surrounding UI
- keep the accessible name action-oriented, for example `Create`, `Compose`, or `Add task`

Example:

```cpp
QtMaterial::QtMaterialFab *fab = new QtMaterial::QtMaterialFab(this);
fab->setIcon(QIcon(QStringLiteral(":/icons/add.svg")));
fab->setAccessibleName(QStringLiteral("Create"));
fab->setToolTip(QStringLiteral("Create"));
```

### `QtMaterial::QtMaterialExtendedFab`

The extended floating action button is the promoted-action variant that carries a label,
optionally paired with an icon.

Expected behavior in this library:

- accepts text-only or icon-plus-text content
- derives height from the resolved extended FAB spec
- derives width from padding, optional icon, optional spacing, and label width
- keeps the Material touch target guarantees through the resolved button spec
- inherits filled-button interaction, ripple, and theming behavior
- intentionally non-checkable

Accessibility guidance:

- the visible label normally communicates the action to assistive technologies
- set an explicit accessible name when the visible label is abbreviated or ambiguous
- the leading icon should be treated as decorative unless the application needs a richer description

Example:

```cpp
QtMaterial::QtMaterialExtendedFab *fab =
    new QtMaterial::QtMaterialExtendedFab(QIcon(QStringLiteral(":/icons/edit.svg")),
                                          QStringLiteral("Compose"), this);
fab->setAccessibleName(QStringLiteral("Compose a new message"));
fab->setToolTip(QStringLiteral("Compose"));
```

## FAB accessibility checklist

For every compact FAB used by an application:

- provide an icon
- provide `setAccessibleName(...)`
- consider `setToolTip(...)` for discoverability
- avoid relying on hidden text because `QtMaterialFab` intentionally clears text content

For every extended FAB used by an application:

- prefer a concise visible action label
- add `setAccessibleName(...)` only when the visible text is not descriptive enough
- keep disabled examples named so assistive technologies can still explain unavailable actions

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
#include <qtmaterial/widgets/buttons/qtmaterialfab.h>
#include <qtmaterial/widgets/buttons/qtmaterialextendedfab.h>
```

## Material 3 references

Use these upstream references for behavior and semantics:

- buttons overview:
- shape guidance:
- motion guidance:

## Next documentation step

Add Doxygen comments to each button header explaining:

- intended Material 3 variant
- supported icon/text combinations
- state-layer and ripple behavior
- size policy and density interaction
- theming hooks and spec resolution behavior


### FAB interaction and motion

FAB and Extended FAB reuse the filled-button interaction pipeline:

- state-layer opacity is animated through the existing button motion token
- ripple clipping follows the resolved FAB or Extended FAB container path
- focus ring is painted on the resolved visual container
- elevated FAB specs receive an animated interaction shadow on hover, focus and press
- disabled FABs suppress interaction elevation and keep the disabled container/content colors

The interaction elevation is intentionally internal: applications should continue to use
`setFabVariant(...)`, theme tokens and density settings rather than overriding paint behavior.

Suggested manual checks in the gallery:

- hover each FAB and verify the elevation transition is smooth
- press and release each FAB and verify the pressed feedback is stronger than hover
- keyboard-focus FABs and verify the focus ring remains visible
- verify disabled FABs do not animate
- verify Extended FAB labels remain readable while the state layer and ripple are active

### FAB color variants

`QtMaterialFab` and `QtMaterialExtendedFab` expose a shared `QtMaterialFabVariant` enum for Material 3 color families:

| Variant | Container token | Content token | Intended use |
| --- | --- | --- | --- |
| `Primary` | `PrimaryContainer` | `OnPrimaryContainer` | Default promoted action. |
| `Secondary` | `SecondaryContainer` | `OnSecondaryContainer` | Alternative promoted action when primary emphasis is too strong. |
| `Tertiary` | `TertiaryContainer` | `OnTertiaryContainer` | Supporting promoted action with tertiary color emphasis. |
| `Surface` | `SurfaceContainerHigh` | `Primary` | Lower-emphasis FAB placed on surface-heavy layouts. |

Changing the variant affects colors only. Diameter, touch target, icon size, padding, spacing, shape, elevation and motion remain resolved from the same FAB or extended FAB spec.

```cpp
QtMaterial::QtMaterialFab* fab = new QtMaterial::QtMaterialFab(icon, parent);
fab->setFabVariant(QtMaterial::QtMaterialFabVariant::Secondary);

QtMaterial::QtMaterialExtendedFab* extended =
    new QtMaterial::QtMaterialExtendedFab(icon, QStringLiteral("Compose"), parent);
extended->setFabVariant(QtMaterial::QtMaterialFabVariant::Surface);
```
