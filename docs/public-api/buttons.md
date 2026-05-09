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


### `QtMaterial::QtMaterialIconButton` accessibility contract
Icon buttons are icon-only controls, so they should have a usable accessible name. Use `setAccessibleName(...)` for new code; tooltip/text fallback is kept as a compatibility guardrail. See `docs/public-api/iconbutton-accessibility.md` for the detailed contract.

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

## Floating Action Buttons

See `docs/public-api/fab-release-readiness.md` for the FAB and Extended FAB release-readiness contract.

<!-- BEGIN GENERATED BUTTONS MATURITY CONTRACT -->

## Buttons maturity contract

The Buttons family is considered mature only when all public button variants satisfy the same API, rendering, interaction, accessibility, keyboard, HiDPI, RTL, test, gallery, and documentation contract.

This section is intentionally strict. It defines the acceptance criteria for moving any Button component from `usable` to `complete` in `docs/components/component-registry.json`.

### Public variants covered

| Component ID | Public class | Header | Expected test target |
|---|---|---|---|
| `button.text` | `QtMaterial::QtMaterialTextButton` | `qtmaterial/widgets/buttons/qtmaterialtextbutton.h` | `tst_textbutton` |
| `button.filled` | `QtMaterial::QtMaterialFilledButton` | `qtmaterial/widgets/buttons/qtmaterialfilledbutton.h` | `tst_filledbutton` |
| `button.filled-tonal` | `QtMaterial::QtMaterialFilledTonalButton` | `qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h` | `tst_filledtonalbutton` |
| `button.outlined` | `QtMaterial::QtMaterialOutlinedButton` | `qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h` | `tst_outlinedbutton` |
| `button.elevated` | `QtMaterial::QtMaterialElevatedButton` | `qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h` | `tst_elevatedbutton` |
| `button.icon` | `QtMaterial::QtMaterialIconButton` | `qtmaterial/widgets/buttons/qtmaterialiconbutton.h` | `tst_iconbutton` |
| `button.fab` | `QtMaterial::QtMaterialFab` | `qtmaterial/widgets/buttons/qtmaterialfab.h` | `tst_fab` |
| `button.extended-fab` | `QtMaterial::QtMaterialExtendedFab` | `qtmaterial/widgets/buttons/qtmaterialextendedfab.h` | `tst_extendedfab` |

### Completion gate

A Button component can be marked `complete` only when its `maturityAxes` entry reaches the following target:

| Axis | Required score | Required evidence |
|---|---:|---|
| `api` | `4` | Public header compiles standalone, constructors are stable, properties are coherent, and consumer usage is covered. |
| `rendering` | `4` | Light/dark theme, enabled/disabled, hover, press, focus, icon/text layout, density, typography, shape, stroke/elevation where relevant. |
| `states` | `4` | Normal, hovered, pressed, focused, disabled, combined focus/hover/press states, and non-checkable behavior where applicable. |
| `accessibility` | `4` | Role, name, enabled/disabled state, focus state, icon-only naming policy, and screen-reader-visible action semantics are validated. |
| `keyboard` | `4` | Tab focus, Space activation, Enter/Return activation, disabled non-activation, and visible focus indication are validated. |
| `hidpi` | `4` | DPR-sensitive rendering is checked for text, icon, stroke, ripple, focus ring, shape, and shadow where relevant. |
| `rtl` | `4` or `N/A` | Text alignment, icon/text order, start/end padding mirroring, and size hint stability are validated. |
| `tests` | `4` | Variant test plus shared contract test plus rendering/accessibility/keyboard/RTL/HiDPI coverage. |
| `example` | `4` | Gallery demonstrates variants, states, disabled mode, icon/text configurations, dark theme, and RTL where relevant. |
| `docs` | `4` | Usage, variants, accessibility, keyboard behavior, theming, RTL, HiDPI, and known limitations are documented. |

No Button component should be marked `complete` while any required axis is `null` / `?`.

### Shared behavioral contract

Every Button variant must satisfy these shared checks:

- The widget is enabled by default.
- `sizeHint()` and `minimumSizeHint()` are valid.
- The effective focus policy allows keyboard focus.
- `Space` activates the button.
- `Enter` or `Return` activates the button.
- Disabled buttons do not activate from keyboard or mouse input.
- The focus indicator is visible when focus comes from the keyboard.
- `accessibleName` is available for assistive technologies.
- The accessible role is button-like.
- Theme changes invalidate the resolved visual spec.
- State-layer and ripple feedback remain clipped to the resolved shape.
- Rendering remains stable at high device pixel ratios.

The transversal test target for these checks is:

```bash
ctest --test-dir <build-dir> -R tst_button_contracts --output-on-failure
```

### Text-bearing buttons

Applies to:

- `QtMaterialTextButton`
- `QtMaterialFilledButton`
- `QtMaterialFilledTonalButton`
- `QtMaterialOutlinedButton`
- `QtMaterialElevatedButton`
- `QtMaterialExtendedFab`

Additional requirements:

- Text-only construction works.
- Icon-plus-text construction works where supported.
- Visible text becomes the default accessible name unless explicitly overridden.
- Long labels have stable geometry and do not corrupt painting.
- Disabled text uses the correct disabled content color.
- RTL mode mirrors icon/text order and start/end padding where applicable.
- Size hints remain stable between LTR and RTL unless a documented exception exists.

### Icon-only buttons

Applies to:

- `QtMaterialIconButton`
- compact `QtMaterialFab`

Icon-only buttons have a stricter accessibility contract:

- Application code must provide an explicit accessible name.
- The accessible name must be action-oriented, for example `Create`, `Search`, `Open menu`, or `Delete item`.
- A tooltip is recommended for discoverability, but it is not a substitute for an accessible name in mature code.
- The icon should be treated as decorative unless richer semantics are explicitly needed.
- Disabled icon-only buttons must still expose a meaningful accessible name.

Example:

```cpp
QtMaterial::QtMaterialIconButton* button = new QtMaterial::QtMaterialIconButton(icon, parent);
button->setAccessibleName(QStringLiteral("Search"));
button->setToolTip(QStringLiteral("Search"));
```

### Variant-specific requirements

#### Text Button

- No filled container is painted in the default state.
- State layer remains visible enough on hover/focus/press.
- Disabled content color is correct.
- Text alignment and padding remain stable in RTL.

#### Filled Button

- Container and content colors are resolved from the active theme.
- Disabled container and disabled content colors are correct.
- Ripple and state layer are clipped to the container shape.
- This variant should be the first reference candidate for the Buttons family.

#### Filled Tonal Button

- Tonal container and content colors maintain accessible contrast.
- Hover/focus/pressed overlays remain visually distinct.
- Disabled state does not reuse enabled tonal emphasis.

#### Outlined Button

- Stroke color is correct in default, hover, focus, pressed, and disabled states.
- Stroke remains crisp at DPR 1.0, 1.5, and 2.0.
- State layer does not visually erase or double-paint the outline.
- Rounded outline clipping is stable.

#### Elevated Button

- Elevation is correct in default, hover, focus, pressed, and disabled states.
- Disabled elevation is suppressed or reduced according to the resolved spec.
- Shadow rendering is stable at high device pixel ratios.
- State-layer and shadow animations do not fight each other.

#### Icon Button

- Minimum visual/touch target is enforced.
- Icon is centered in all states.
- Focus ring and ripple are not clipped incorrectly.
- Accessible-name policy is enforced by tests and documentation.

#### Floating Action Button

- Shape, diameter, icon size, and touch target come from the FAB spec.
- Icon remains centered at all supported sizes.
- Elevation and shadow behavior are validated per interaction state.
- The compact FAB remains intentionally icon-only and non-checkable.
- Accessible-name policy is enforced by tests and documentation.

#### Extended Floating Action Button

- Text-only and icon-plus-text modes work.
- Width is resolved from padding, icon, spacing, and label width.
- Long labels remain readable or degrade in a documented way.
- RTL mode mirrors icon/text order where applicable.
- Visible label normally supplies the accessible name.

### Gallery requirements

The Buttons gallery should demonstrate:

- all eight variants;
- enabled and disabled states;
- text-only examples;
- icon-only examples;
- icon-plus-text examples;
- long-label examples;
- light and dark theme comparison;
- density variation when density is public;
- RTL preview for text-bearing variants;
- accessibility note for icon-only buttons and compact FABs.

### Test commands

Recommended focused validation commands:

```bash
ctest --test-dir <build-dir> -R "tst_.*button|tst_.*fab|tst_button_contracts" --output-on-failure
ctest --test-dir <build-dir> -R "tst_.*button_render|tst_button_contracts" --output-on-failure
python scripts/generate_component_status.py --check
```

Before declaring Buttons complete, run the stricter registry check:

```bash
python scripts/generate_component_status.py --check --strict
```

### Promotion checklist

Use this checklist before changing a Button component to `complete`:

- [ ] The component exists in `docs/components/component-registry.json`.
- [ ] The component has `maturityAxes`.
- [ ] No required axis is `null` / `?`.
- [ ] The component-specific test target exists and passes.
- [ ] `tst_button_contracts` passes for the component.
- [ ] Rendering tests or visual checks cover the variant-specific surface.
- [ ] Keyboard activation and disabled non-activation are tested.
- [ ] Accessibility name, role, and state are tested.
- [ ] HiDPI rendering is tested or manually validated and recorded.
- [ ] RTL behavior is tested or marked `N/A` with justification.
- [ ] Gallery demonstrates the component and its key states.
- [ ] Documentation explains usage, behavior, and limitations.

<!-- END GENERATED BUTTONS MATURITY CONTRACT -->
