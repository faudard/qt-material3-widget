# Progress indicators

Progress indicators expose Material 3 compatible determinate and indeterminate feedback widgets for Qt Widgets.

The implementation follows the project rule: widgets render from resolved specs, not from ad hoc theme lookups. `ProgressIndicatorSpec` carries the visual contract shared by linear and circular indicators.

## Public widgets

- `QtMaterial::QtMaterialLinearProgressIndicator`
- `QtMaterial::QtMaterialCircularProgressIndicator`

Both widgets expose:

- `value`: normalized progress in the `[0.0, 1.0]` range.
- `mode`: `Determinate` or `Indeterminate`.
- `spec`: immutable-style visual input for color, sizing and motion defaults.

The linear indicator also exposes `invertedAppearance` for right-to-left or trailing-edge progress rendering.

## Usage

```cpp
#include <qtmaterial/widgets/progress/qtmateriallinearprogressindicator.h>
#include <qtmaterial/widgets/progress/qtmaterialcircularprogressindicator.h>

using namespace QtMaterial;

ProgressIndicatorSpec spec;
spec.linearHeight = 4;
spec.circularSize = QSize(48, 48);
spec.circularStrokeWidth = 4;

auto* linear = new QtMaterialLinearProgressIndicator(spec, parent);
linear->setValue(0.65);

鈥 auto* circular = new QtMaterialCircularProgressIndicator(spec, parent);
circular->setMode(QtMaterialCircularProgressIndicator::Mode::Indeterminate);
```

## Behavior

Determinate mode paints a stable progress value. Indeterminate mode starts an internal `QVariantAnimation` while the widget is visible and stops it when hidden or switched back to determinate mode.

Color resolution intentionally remains conservative. Explicit spec colors are used first. If no explicit colors are provided, the widgets fall back to the current Qt palette so they remain usable before deeper theme-manager integration.

## Tests

`tst_progressindicators` covers:

- value clamping for linear and circular indicators;
- duplicate mode changes not emitting duplicate signals;
- size hints driven by `ProgressIndicatorSpec`;
- spec round-trip behavior.
