# Native Runtime 3.0 — incremental adoption reference

The `qtmaterial3_native_adoption` executable demonstrates progressive
styling of **native** Qt widgets, without replacing their C++ types or
transferring application-owned models, validators, menus or signals.

## Build and run

```bash
cmake -S . -B build -DQTMATERIAL3_BUILD_EXAMPLES=ON
cmake --build build --target qtmaterial3_native_adoption
./build/examples/native-runtime-adoption/qtmaterial3_native_adoption
```

On Windows, use the configuration-specific binary directory where generated.

## What to inspect

- The `adoptionform.ui` file supplies typed dynamic properties with
  `stdset="0"`, including `qtm3MaterialAdapt`, variant, density and
  text-field variant. AUTOUIC persists and compiles them.
- The original `QPushButton`, `QToolButton`, `QCheckBox`,
  `QRadioButton`, `QSlider`, `QComboBox`, `QLineEdit` and
  `QProgressBar` remain native; the extra button is intentionally unadapted.
- `QStandardItemModel` remains application-owned for the combo; the
  `QIntValidator` remains application-owned for the line edit; the tool
  button continues to own and open its ordinary `QMenu`.
- Clicking Save preserves its native `clicked()` connection and advances
  the progress bar. The Opt-out checkbox removes/reapplies the adapter
  without rebuilding the control.
- A late-created button is declared programmatically and discovered by the
  watcher on a queued event-loop pass. Teardown calls `unwatch()`.

`DeclaredOnly` is the default and safest migration path. For a deliberate
whole-form migration, switch to `AllSupported` and provide options, while
retaining per-widget overrides and opt-out decisions.

The eight adapted control types deliberately keep their original input,
popup, action, model and signal contracts. Complete Material text-field
supporting text and custom combo popup behavior require dedicated widgets.

## Qualification boundary

Run `tst_nativeadapterfacade`, `tst_nativeruntimewatcher` and all eight
specialized native adapter test executables with CTest. The watcher tests
cover live override updates, fallback changes, opt-out restoration,
independent roots, reparenting, lifetime churn, style replacement and
native signals. Run the suite with the Qt 5.14.2/MSVC v142 and supported Qt 6
CI matrix and sanitizers before claiming 1.24 release readiness.

This is a functional example, **not** proof of cross-platform certification
or a tagged 1.24.0 release.
