# Theme JSON schema

`qt-material3-widget` currently ships one theme JSON schema:

```text
docs/schema/theme.schema.json
```

This schema describes the current pre-release export format used by `QtMaterial::ThemeSerializer`.

## Format version

The current format version is:

```cpp
QtMaterial::ThemeSerializer::kCurrentFormatVersion == 1
```

The JSON document must contain:

```json
{
  "formatVersion": 1,
  "source": {},
  "resolved": {},
  "metadata": {}
}
```

`formatVersion` is intentionally `1` because this is the first official theme JSON contract. The project no longer exposes separate public `theme-v1.schema.json` or `theme-v2.schema.json` files.

## Root object

The root object contains three logical sections.

### `source`

`source` stores the inputs used to build the theme.

Required fields:

```json
{
  "seedColor": "#6750A4FF",
  "mode": "Light",
  "preference": "Light",
  "contrast": "Standard",
  "variant": "TonalSpot",
  "colorBackendPolicy": "Auto"
}
```

Field meanings:

- `seedColor`: ARGB/RGBA-style color string in the schema format `#RRGGBBAA`;
- `mode`: resolved mode, either `Light` or `Dark`;
- `preference`: user preference, one of `Light`, `Dark`, or `FollowSystem`;
- `contrast`: one of `Standard`, `Medium`, or `High`;
- `variant`: one of `TonalSpot` or `Expressive`;
- `colorBackendPolicy`: one of `Auto`, `PreferMaterialColorUtilities`, `ForceMaterialColorUtilities`, or `ForceFallback`.

`preference` and `mode` are intentionally separate. `preference` may be `FollowSystem`; `mode` must always be the resolved concrete mode stored in the built theme.

### `resolved`

`resolved` stores the concrete tokens used by widgets and renderers.

Required groups:

```text
colorScheme
typographyScale
shapeScale
elevationScale
motionTokens
stateLayer
accessibility
interactions
density
iconSizes
componentOverrides
```

The serializer should write resolved values rather than relying on consumers to reconstruct them from `source` alone. This keeps imports deterministic even if theme-generation internals change later.

### `metadata`

`metadata` stores diagnostic information about the generator and runtime.

Required fields:

```json
{
  "generatorVersion": "qt-material3-widget",
  "libraryVersion": "0.1.0",
  "qtVersion": "6.x"
}
```

Optional fields:

```json
{
  "timestamp": "2026-04-29T12:00:00Z"
}
```

`timestamp` is optional so deterministic golden snapshots can omit it.

## Read modes

`ThemeSerializer` exposes two read modes:

```cpp
enum class ThemeReadMode {
    Lenient,
    Strict
};
```

Use `Strict` when reading user-provided files, test fixtures, golden snapshots, or release-contract examples. Strict mode should reject unsupported versions, missing required groups, and unknown fields that are not part of the current schema.

Use `Lenient` when application code intentionally wants to tolerate forward-compatible extra data. Lenient mode should still reject malformed JSON and unsupported fundamental structures.

There is no `UpgradeIfPossible` mode in the current public API. The project has not shipped an official historical schema yet, so there is no public migration contract to maintain.

## Minimal example

A structurally valid document has this shape:

```json
{
  "formatVersion": 1,
  "source": {
    "seedColor": "#6750A4FF",
    "mode": "Light",
    "preference": "Light",
    "contrast": "Standard",
    "variant": "TonalSpot",
    "colorBackendPolicy": "Auto"
  },
  "resolved": {
    "colorScheme": {},
    "typographyScale": {},
    "shapeScale": {},
    "elevationScale": {},
    "motionTokens": {},
    "stateLayer": {
      "color": "#000000FF",
      "hoverOpacity": 0.08,
      "focusOpacity": 0.12,
      "pressOpacity": 0.12,
      "dragOpacity": 0.16
    },
    "accessibility": {
      "highContrast": false,
      "reducedMotion": false,
      "minimumTextContrastRatio": 4.5,
      "minimumUiContrastRatio": 3.0,
      "focusRing": {
        "width": 2,
        "offset": 2,
        "radiusAdjustment": 0,
        "color": "#6750A4FF",
        "opacity": 1.0
      }
    },
    "interactions": {
      "keyboardFocusVisible": true,
      "strongFocusIndicators": false,
      "hoverFeedbackEnabled": true,
      "pressFeedbackEnabled": true,
      "dragFeedbackEnabled": true
    },
    "density": {},
    "iconSizes": {},
    "componentOverrides": {}
  },
  "metadata": {
    "generatorVersion": "qt-material3-widget",
    "libraryVersion": "0.1.0",
    "qtVersion": "6.x"
  }
}
```

Real exports are expected to contain populated token groups rather than empty objects.

## Validation policy

Before accepting a schema change, update these together:

1. `docs/schema/theme.schema.json`;
2. serializer read/write tests;
3. golden JSON fixtures;
4. this documentation page;
5. the public theming guide.

The schema, serializer, and documentation must describe the same contract.

## Compatibility policy

Before the first official release, the schema may change as part of API stabilization.

After the first official release:

- keep `formatVersion: 1` stable for compatible additions;
- add optional fields instead of renaming or deleting required fields;
- introduce a new `formatVersion` only for breaking changes;
- document migration behavior before adding a new read mode or compatibility path.

## See also

- [Theming guide](theming.md)
- `docs/schema/theme.schema.json`
- `QtMaterial::ThemeSerializer`
- `QtMaterial::ThemeOptions`
