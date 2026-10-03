# Material 3 Expressive foundation

Expressive motion is opt-in and independent of the existing Expressive color variant.
Choose it during construction or while authoring a theme snapshot:

```cpp
QtMaterial::ThemeOptions options;
options.motionScheme = QtMaterial::MotionScheme::Expressive;
QtMaterial::Theme theme = QtMaterial::ThemeBuilder().build(options);

// Switching schemes resets the six semantic profiles to their defaults.
theme.setMotionScheme(QtMaterial::MotionScheme::Standard);
```

`theme.motionScheme()` reports the selected scheme. Existing Short1–Long4 duration
tokens retain their values and numeric identifiers. New tokens append stable IDs and
participate in theme equality, component motion overrides, ThemeIO and reduced motion.

| Token | Standard duration | Expressive duration | Intended use |
| --- | ---: | ---: | --- |
| SpatialFast | 160 ms | 240 ms | Small movements, shape changes |
| SpatialDefault | 300 ms | 380 ms | Normal container transitions |
| SpatialSlow | 450 ms | 500 ms | Large layout movements |
| EffectsFast | 150 ms | 150 ms | Small color/opacity changes |
| EffectsDefault | 250 ms | 250 ms | Normal visual effects |
| EffectsSlow | 350 ms | 350 ms | Large visual effects |

These are duration/easing profiles for Qt Widgets. Standard spatial profiles use
OutCubic; Expressive spatial profiles use OutBack. Effects use bounded OutCubic in both
schemes so color and opacity do not overshoot. They are an adaptation of the
[Material motion scheme API](https://developer.android.com/reference/kotlin/androidx/compose/material3/MotionScheme),
not an implementation of Compose's physical spring solver. Components choose the semantic
profile appropriate to each property, and consumers of geometry/opacity must enforce
valid bounds. The existing transition controller supports the new tokens through
`applyMotionToken(theme, token)`; its progress remains normalized to [0, 1].

`applyReducedMotion` covers all 18 tokens. Changing a scheme also reapplies a theme's
active reduced-motion policy. Theme JSON stores optional `source.motionScheme`; existing
version-1 files without that member default to Standard. Missing new resolved profiles
are filled from the selected scheme. Strict reading rejects unknown scheme values.
Applications can continue overriding individual motion profiles after choosing a scheme.

## Internal shape morphing

The effects module contains `QtMaterialShapeMorph`, declared in a private header and
excluded from installed public headers. It handles finite single-contour QPainterPath
outlines in a shared coordinate system, including shapes with different topology and
winding. Source/target paths are closed, sampled by arc length, aligned by winding and
cyclic correspondence, and cached when shapes change. `pathAt(progress)` interpolates
the cached points without repeating correspondence work on each animation frame.

Endpoints preserve the source and target paths. Intermediate outlines use 16–256 samples
(default 96) to approximate the silhouette; progress is bounded and invalid/degenerate or
multi-contour inputs are rejected. `roundedRectangle(rect, radius)` supplies a bounded
corner-radius helper for pressed/selected button shapes. Timing is deliberately separate:
widget internals can drive the primitive with the existing transition controller and use
its reduced-motion final state. No widget/theme lookup or event handling belongs to the
morph primitive.

This foundation prepares shape transitions for buttons, toggles, FAB expansion and future
morphing loading indicators. New Expressive widget families and reviewed reference imagery
will be added separately. The registered `tst_expressive_foundation` suite covers schemes,
legacy IDs, serialization/defaults, reduced motion, theme identity, outline interpolation,
winding alignment and invalid contour rejection.

The geometry approach is inspired by the
[Compose shape-morphing guide](https://developer.android.com/develop/ui/compose/graphics/draw/shapes).
