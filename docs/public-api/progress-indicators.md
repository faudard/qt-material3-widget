# Progress indicators

Progress indicators expose Material 3 style feedback for operations whose duration is known or
unknown. Widgets render from a resolved `ProgressIndicatorSpec`.

## Public widgets

- `QtMaterial::QtMaterialLinearProgressIndicator`
- `QtMaterial::QtMaterialCircularProgressIndicator`

Both are release-scoped as **usable** for 0.5.0.

## Shared behavior

Both widgets support:

- normalized determinate progress in `[0.0, 1.0]`;
- `Determinate` and `Indeterminate` modes;
- active and track color overrides;
- track gap configuration;
- asynchronous status text;
- resolved theme/spec behavior;
- accessible progress/status descriptions.

Indeterminate animation runs only while needed and stops when the widget is hidden or returns to
determinate mode.

## Linear Progress Indicator

`QtMaterialLinearProgressIndicator` additionally supports:

- `invertedAppearance`;
- a Material stop indicator;
- RTL-aware active-track rendering.

In normal LTR mode, determinate progress grows from the leading left edge. In RTL, the active
segment is mirrored and grows from the trailing right edge. `invertedAppearance` reverses that
choice explicitly.

Its accessibility value is a rounded percentage in determinate mode, optionally prefixed by
`statusText`. Indeterminate mode exposes the status text or `In progress`.

### 0.5.0 maturity evidence

`tst_progressindicators` covers:

- clamping and duplicate-signal behavior;
- public spec setters and resetters;
- spec round trips;
- determinate/indeterminate state;
- show/hide animation lifecycle;
- linear accessibility text;
- explicit LTR/RTL rendering direction;
- DPR 2.0 render smoke.

Deterministic reviewed visual references and broader animation end-state certification remain
before `complete` maturity.

## Circular Progress Indicator

The circular widget provides determinate and indeterminate rendering, stroke-width control,
accessible percentage/status reporting, and async-state integration.

## Example

```cpp
#include <qtmaterial/widgets/progress/qtmateriallinearprogressindicator.h>

auto* progress =
    new QtMaterial::QtMaterialLinearProgressIndicator(parent);
progress->setStatusText(QStringLiteral("Uploading"));
progress->setValue(0.65);
```

The gallery Progress Indicators page demonstrates determinate and indeterminate linear/circular
variants.
