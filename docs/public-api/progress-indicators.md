# Progress indicators

Progress indicators expose Material 3 compatible feedback widgets for operations whose duration is known or unknown.

The implementation follows the project layering rule: widgets render from a resolved `ProgressIndicatorSpec`, while the public widgets expose safe convenience setters for application code and the gallery.

## Public widgets

- `QtMaterial::QtMaterialLinearProgressIndicator`
- `QtMaterial::QtMaterialCircularProgressIndicator`

## Shared API

Both widgets expose:

- `value`: normalized progress in `[0.0, 1.0]`; invalid or NaN input is clamped to `0.0`.
- `mode`: `Determinate` or `Indeterminate`.
- `activeColor`: explicit indicator color override.
- `trackColor`: explicit track color override.
- `trackGap`: visual gap between active and inactive track portions.
- `spec`: full immutable-style visual input for colors, dimensions, margins and motion defaults.

`QtMaterialLinearProgressIndicator` additionally exposes:

- `invertedAppearance`: paints progress from the trailing edge.
- `stopIndicatorSize`: size of the Material 3 stop indicator in determinate mode.

`QtMaterialCircularProgressIndicator` additionally exposes:

- `strokeWidth`: circular stroke width.

## Usage

```cpp
#include <qtmaterial/widgets/progress/qtmateriallinearprogressindicator.h>
#include <qtmaterial/widgets/progress/qtmaterialcircularprogressindicator.h>

using namespace QtMaterial;

ProgressIndicatorSpec spec;
spec.linearHeight = 4;
spec.circularSize = QSize(48, 48);
spec.circularStrokeWidth = 4;
spec.trackGap = 4;
spec.stopIndicatorSize = 4;

auto* linear = new QtMaterialLinearProgressIndicator(spec, parent);
linear->setValue(0.65);
linear->setActiveColor(QColor("#6750A4"));

auto* circular = new QtMaterialCircularProgressIndicator(spec, parent);
circular->setMode(QtMaterialCircularProgressIndicator::Mode::Indeterminate);
```

## Behavior

Determinate mode paints a stable progress value. Indeterminate mode starts an internal animation while the widget is visible and stops it when hidden or switched back to determinate mode.

Explicit spec colors are used first. If no explicit colors are provided, the widgets fall back to the current Qt palette, which keeps them usable before deeper theme-manager integration.

## Gallery

The overlay adds a `ProgressIndicatorsPage` with determinate linear/circular indicators, a slider-driven value demo, and indeterminate linear/circular indicators.

If the local gallery uses a central navigation registry, run the patch script and then add `ProgressIndicatorsPage` to the registry if the automatic insertion marker was not found.

## Tests

`tst_progressindicators` covers:

- value clamping for linear and circular indicators;
- duplicate mode changes not emitting duplicate signals;
- public API setters and resetters;
- size hints driven by `ProgressIndicatorSpec`;
- full spec round-trip behavior;
- show/hide safety for indeterminate mode.
