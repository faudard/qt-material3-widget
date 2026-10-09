# Python / PySide6 bindings

QtMaterial3 1.17 adds an optional **Qt 6 only** Python surface generated with
Shiboken6. Qt5 remains a native C++ compatibility target and is intentionally
not part of the Python binding contract.

## Install from a wheel

The Python package imports as:

```python
import QtMaterial3
from QtMaterial3 import Widgets
```

A source checkout can build a wheel with:

```bash
python -m pip install --upgrade build
PIP_EXTRA_INDEX_URL=https://download.qt.io/official_releases/QtForPython/ \
PIP_TRUSTED_HOST=download.qt.io \
python -m build bindings/python --wheel --outdir wheelhouse
python -m pip install wheelhouse/qtmaterial3_widgets-*.whl
```

The extra Qt package index is required for the matching `shiboken6_generator`
wheel. Qt publishes that generator separately from the normal PyPI package set.

The PEP 517 build installs the matching PySide6/Shiboken6 generator toolchain,
configures the native project with `QTMATERIAL3_BUILD_PYTHON_BINDINGS=ON`, and
packages the generated extension together with the small Python facade.

## Direct CMake development build

Install CMake 3.22+ and matching PySide6/Shiboken6 packages in the Python interpreter used by CMake, then configure:

```bash
python -m pip install PySide6==6.4.2 shiboken6==6.4.2 shiboken6-generator==6.4.2
cmake -S . -B build-python \
  -DQTMATERIAL3_BUILD_PYTHON_BINDINGS=ON \
  -DQTMATERIAL3_BUILD_TESTS=ON \
  -DQTMATERIAL3_BUILD_EXAMPLES=OFF
cmake --build build-python
ctest --test-dir build-python -R python_bindings_contract --output-on-failure
```

Enabling the option with a Qt5 toolchain fails at configure time by design.

## Themes and Expressive

```python
from PySide6.QtGui import QColor
import QtMaterial3

options = QtMaterial3.ThemeOptions()
options.sourceColor = QColor("#6750A4")
options.variant = QtMaterial3.ThemeVariant.Expressive
options.motionScheme = QtMaterial3.MotionScheme.Expressive

theme = QtMaterial3.ThemeBuilder().build(options)
context = QtMaterial3.ThemeContext(theme)
```

`Theme`, `ThemeBuilder`, `ThemeContext`, the core theme enums and the
Expressive motion/theme enums are generated directly from the C++ API. There is
no Python-only theme state.

## Widgets

The first binding surface intentionally starts with representative widget
families rather than promising all 60 components in one ABI step:

- text, filled, outlined, filled-tonal and elevated buttons;
- Checkbox and Switch;
- outlined and filled text fields;
- Card;
- Navigation Bar and Navigation Rail;
- linear, circular and Expressive loading indicators.

They are available under `QtMaterial3.Widgets`. Existing Qt properties and
signals are the binding contract, so normal PySide6 patterns such as
`widget.setProperty(...)`, property assignment and signal connections call
the native C++ implementation.

## Adaptive enums

`WindowSizeClass`, `WindowWidthSizeClass`, `WindowHeightSizeClass` and
`Density` are exposed alongside the theme enums:

```python
size_class = QtMaterial3.WindowSizeClass.fromLogicalSize(520, 700)
assert size_class.width == QtMaterial3.WindowWidthSizeClass.Compact
```

## QObject ownership

QObject ownership follows Qt rather than Python reference counting. Adaptive size/density enums are bound in 1.17; pointer-owning Adaptive Shell composition remains outside the initial Python ABI until replacement/unparenting semantics have a dedicated binding contract.


- Constructors with a conventional `parent` argument use Shiboken's
  parent-constructor heuristic.
- QObject-derived QtMaterial3 wrappers request deletion on their owning thread.
- Pointer-owning Adaptive Shell composition is intentionally not bound in 1.17; it remains outside the initial Python ABI until replacement/unparenting ownership semantics have dedicated tests.
- Tests destroy a C++ parent with `Shiboken.delete()`, verify the child wrapper
  becomes invalid, then force Python garbage collection. This guards the
  double-free case as well as stale-wrapper use.

Application code should still prefer ordinary Qt parent/child ownership for
widgets and use `deleteLater()` when object lifetime crosses queued event-loop
work.

See `bindings/python/examples/basic_theme.py` for a complete runnable example.
