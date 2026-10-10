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
python -m build bindings/python --wheel --outdir wheelhouse
python -m pip install wheelhouse/qtmaterial3_widgets-*.whl
```

The build requires a Qt 6.6.3 development SDK matching the PySide6/Shiboken6
minor version. The dedicated CI lane installs that SDK before invoking PEP 517.

The PEP 517 build installs the matching PySide6/Shiboken6 generator toolchain,
configures the native project with `QTMATERIAL3_BUILD_PYTHON_BINDINGS=ON`, and
packages the generated extension together with the small Python facade.

## Wheel reliability / ABI contract (1.17.4)

The **supported build matrix** is CPython **3.10–3.12**, 64-bit, with
**PySide6, PySide6-Essentials, PySide6-Addons, shiboken6 and
shiboken6-generator all exactly at 6.6.3**, and a matching **Qt 6.6.3
development SDK**. PySide6 6.6.3 itself declares Python < 3.13, so Python
3.13/3.14 are intentionally rejected rather than advertising unusable wheels.

The CMake configure step now checks Python, the installed distribution versions,
the loaded Qt runtime (`PySide6.QtCore.qVersion()`) and the selected Qt SDK
(`Qt6Core_VERSION`). A mismatch stops the build before Shiboken generates code.
To inspect the installed wheel toolchain:

```bash
# Build environment (generator installed by PEP 517)
python bindings/python/pyside_config.py --verify-abi
# Installed wheel environment (generator is not a runtime dependency)
python bindings/python/pyside_config.py --verify-runtime-abi
```

The GitHub Actions `Python bindings` workflow builds wheels for Ubuntu
(CPython 3.10, 3.11 and 3.12), macOS (3.11) and Windows (3.11), audits wheel tags,
packaged Python facade/native extension and the pinned dependency, installs the
wheel, runs `pip check`, imports in Python's isolated mode and executes all
ownership contracts. In particular the stress tests check:

- parent-owned QObject destruction invalidates every child wrapper;
- unparenting permits a widget to outlive its former parent;
- Tooltip's target is borrowed and goes null when its parent is destroyed;
- Tabs never assumes ownership of its NavigationModel, including after detaching
  or the model's external Qt parent is destroyed.

**Build-time wheel is not a universal binary:** the wheel tag must match the
build host/CPython ABI and architecture. The CI audit verifies local wheel
integrity, **not** `manylinux` repair, redistribution portability,
code-signing, notarization, release publication or compatibility with arbitrary
system Qt versions. Do not publish wheels as cross-machine compatible without a
separate deployment and binary-dependency audit.

## Direct CMake development build

Install CMake 3.22+ and matching PySide6/Shiboken6 packages in the Python interpreter used by CMake, then configure:

```bash
python -m pip install PySide6==6.6.3 shiboken6==6.6.3 shiboken6-generator==6.6.3
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

1.17.1 additionally exposes `QtMaterialRadioButton`, `QtMaterialChip` and
`QtMaterialTooltip`. `QtMaterialChip` uses the native `QtMaterial3.ChipVariant`
enum (`Assist`, `Filter`, `Input`, `Suggestion`); the tooltip exposes its
nested `Placement` enum. Its `targetWidget` is a **non-owning** C++ `QPointer`:
assigning a target does not make the tooltip its owner.

```python
from QtMaterial3 import Widgets, ChipVariant

chip = Widgets.QtMaterialChip("Only favorites")
chip.setVariant(ChipVariant.Filter)
chip.setChecked(True)

tooltip = Widgets.QtMaterialTooltip()
tooltip.setText("Filter the list")
tooltip.setTargetWidget(chip)
```

They are available under `QtMaterial3.Widgets`. Existing Qt properties and
signals are the binding contract, so normal PySide6 patterns such as
`widget.setProperty(...)`, property assignment and signal connections call
the native C++ implementation.

## Python Catalogue 2.0 (1.17.5)

Five additional native widgets are bound, **Qt 6 / PySide6 only**:

| Widget | Supported surface | Explicitly deferred |
| --- | --- | --- |
| `QtMaterialDialog` | title/supporting text, dismiss/restore-focus flags, open/close/reject signals, borrowed focus/default-button pointers | `setBodyWidget` / `bodyWidget` (reparent/replacement) |
| `QtMaterialSnackbar` | text/action/duration/dismiss controls, lifecycle/action signals, progress/interaction properties | `SnackbarRequest` value payload and queue/host composition |
| `QtMaterialBadge` | count, maximum, dot, display text and signals | — |
| `QtMaterialPagination` | page/pageSize/totalCount, options, range, signals, borrowed theme context | `PaginationSpec` |
| `QtMaterialDivider` | orientation, insets, thickness, color, decorative/label properties and signals | static allocating factories |

`QtMaterialSnackbar` uses native `SnackbarDuration` and
`SnackbarDismissReason` enums, exported under `QtMaterial3`.
`QtMaterialOverlaySurface.setHostWidget()`, Dialog focus/default-button
setters and Pagination theme-context setters accept **borrowed pointers**;
they do not reparent or acquire the supplied QObject. Keep the referents alive
while in use. When destroyed, C++ `QPointer` becomes null.

```python
from QtMaterial3 import Widgets, SnackbarDuration

badge = Widgets.QtMaterialBadge()
badge.setCount(12)

pagination = Widgets.QtMaterialPagination()
pagination.setTotalCount(150)
pagination.setPageSize(25)

snackbar = Widgets.QtMaterialSnackbar()
snackbar.setText("Saved")
snackbar.setDuration(SnackbarDuration.Indefinite)
snackbar.setActionText("Undo")

dialog = Widgets.QtMaterialDialog()
dialog.setTitleText("Confirm changes")
dialog.rejected.connect(lambda: print("Cancelled"))
```

The wheel-contract lane covers imports, native properties/signals,
QObject parent destruction, Shiboken wrapper invalidation and repeated
allocation/collection for all five classes. See
`bindings/python/tests/test_catalogue_2.py` and
`bindings/python/examples/catalogue_2.py`.

NavigationController, custom Tabs factories and C++ callback wrappers remain
**outside 1.17.5** pending explicit ownership/disconnect/cancellation tests.

## Adaptive enums

`WindowSizeClass`, `WindowWidthSizeClass`, `WindowHeightSizeClass` and
`Density` are exposed alongside the theme enums:

```python
size_class = QtMaterial3.WindowSizeClass.fromLogicalSize(520, 700)
assert size_class.width == QtMaterial3.WindowWidthSizeClass.Compact
```

## Tabs and Menu (1.17.2)

`Widgets.QtMaterialTabs` wraps the native `QtMaterialTabs` subclass of
`QTabWidget`, preserving standard PySide6 tab pages and selection signals.
The enums `TabsVariant`, `TabsDensity`, `TabsAlignment` and
`TabsOverflowMode` are exported at package level.

`Widgets.QtMaterialMenu` is exported from the *global* C++ namespace
(the native class is not in `QtMaterial`). It supports native item
management, accessibility descriptions and the `activated` and
`expressiveChanged` signals.

```python
from PySide6.QtWidgets import QWidget
from QtMaterial3 import Widgets, TabsVariant

tabs = Widgets.QtMaterialTabs()
tabs.setVariant(TabsVariant.Secondary)
tabs.addTab(QWidget(), "Overview")
tabs.setTabId(0, "overview")

menu = Widgets.QtMaterialMenu()
menu.addItem("Overview")
menu.activated.connect(tabs.setCurrentIndex)
```

**Ownership boundary:** Tabs owns page widgets through Qt's standard
`QTabWidget` parenting. Menu items are C++ value entries. A parent widget
owns and destroys its Tabs/Menu children. The binding intentionally defers
custom tab factories, route values, navigation-model/controller bindings and
custom `TabsSpec`/`MenuSpec` structures until those conversions and
pointer-transfer rules have separately verified tests.

See `bindings/python/examples/tabs_menu.py` and
`bindings/python/tests/test_navigation_bindings.py`.

## Routes and navigation model (1.17.3)

The native `QtMaterialRoute` value type normalizes paths, and
`QtMaterialNavigationItem` and `QtMaterialNavigationModel` expose the
C++ `QAbstractListModel` surface to PySide6. The route overloads on
`QtMaterialTabs` and its `navigationModel` property now accept the native
bindings.

```python
from QtMaterial3 import QtMaterialRoute, QtMaterialNavigationModel, QtMaterialNavigationItem, Widgets

tabs = Widgets.QtMaterialTabs()
route = QtMaterialRoute("settings//profile/")
# tabs.setRoute(existing_tab_index, route)

model = QtMaterialNavigationModel()
item = QtMaterialNavigationItem()
item.id, item.route, item.label = "settings", "/settings", "Settings"
model.addItem(item)
tabs.setNavigationModel(model)
```

**Ownership contract:** `QtMaterialTabs::setNavigationModel()` stores
only a C++ `QPointer` and does not take ownership of the model. Assign a
Qt parent to the model or retain a strong Python reference while tabs use
it. `navigationModel()` returns a borrowed reference, and nulls on model
destruction. Python-facing tests validate detaching, destruction and the
native model's `selectedIdChanged`/`selectedRouteChanged` signals.

The abstract navigation controller and callback-based lazy tab factories
remain outside this version; wrapping them requires separate tests of
references, QObject destruction, disconnects and cross-widget ownership.

See `bindings/python/examples/navigation_model_routes.py`.

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

See `bindings/python/examples/basic_theme.py` and
`bindings/python/examples/selection_chips_tooltip.py` for runnable examples.
The dedicated wheel CI covers these additional Q_PROPERTY, signal, enum and
QObject lifetime contracts.
