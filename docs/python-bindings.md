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

## Python Navigation Session 3.0 (1.17.9)

`NavigationSession` coordinates native `QtMaterialTabs` and a
`QtMaterial3.AsyncLazyTabs` instance. It manages route history, JSON
snapshots and bounded prefetching. **It never creates QWidget instances
from background workers and does not own tab pages.**

```python
import json
from QtMaterial3 import AsyncLazyTabs, NavigationSession, Widgets

tabs = Widgets.QtMaterialTabs()
pages = AsyncLazyTabs(tabs, max_workers=2, cancel_on_leave=False)
# Register native tabs with distinct routes using pages.addAsyncTab(...)
session = NavigationSession(pages, prefetch_radius=1, max_pending=2)

session.navigate("/settings")
session.back()
session.forward()

session.setPageState("/settings", {"filter": "active"})
serialized = json.dumps(session.saveState())
# On the next launch, register all tabs/routes before restoring:
session.restoreState(serialized)
```

**History uses canonical route paths**, not tab indices. Moving tab positions
does not change history. When a route disappears, back/forward skip it;
restoring a session filters removed routes. Duplicate or missing destinations
cannot be selected with `navigate()`. Normal navigation after `back()`
truncates the old forward branch.

**Portable session schema:** `saveState()` returns a detached JSON-compatible
dictionary: `version=1`, `history`, `cursor`, `currentRoute` and
per-route `pages`. `restoreState()` accepts a dictionary or JSON string,
validates it before modifying navigation and rejects incompatible versions.
Stored page values must be JSON-serializable plain data (not QObjects);
the serialized snapshot limit is 256 KiB. Restore saved values in application
logic using `pageState(route)`. The helper does not serialize QWidget
instances, pending jobs, model pointers or live Python callbacks.

**Bounded prefetch:** `prefetch_radius=1` schedules loaders for adjacent
tabs after navigation, while `max_pending=2` limits how many data loads may
be running or queued. `prefetchNeighbors()` may also be called explicitly.
As in 1.17.8, completed data for inactive tabs remains cached and is
rendered only on activation. For aggressive prefetch reuse, prefer
`AsyncLazyTabs(cancel_on_leave=False)`; with `True`, active requests are
cancelled on departure. Prefetch cannot force arbitrary blocking worker code
to stop and does not impose a global CPU/memory budget on user data.

All operations on `NavigationSession` must run on the Qt GUI thread;
destruction of Tabs destroys the session. It never takes ownership of the
AsyncLazyTabs manager or QWidget page content. `close()` disables session
navigation and timers but leaves existing native tabs/pages untouched.

Cross-platform installed-wheel tests cover route history, history truncation,
tab reorderings, invalid route/snapshot validation, state deep-copying,
prefetch limits and QObject destruction. See
`bindings/python/tests/test_navigation_session.py` and
`bindings/python/examples/navigation_session.py`.

## Async Python Lazy Tabs (1.17.8)

`AsyncLazyTabs` adds **background data loading** to native
`QtMaterialTabs`, while keeping all `QWidget` creation on the Qt GUI
thread. It is a separate opt-in `QObject` helper from 1.17.7
`LazyTabs` and does **not** bind C++ `std::function<QWidget*()>`.

```python
from PySide6.QtWidgets import QLabel
from QtMaterial3 import AsyncLazyTabs, Widgets

tabs = Widgets.QtMaterialTabs()
pages = AsyncLazyTabs(tabs, max_workers=2, cancel_on_leave=True)

def load(cancel):
    # Worker thread: ordinary Python I/O or computation; check cancel often.
    if cancel.is_set():
        return None
    return {"message": "Loaded"}

def render(data):
    # GUI thread only: it is safe to create and parent QWidgets here.
    return QLabel(data["message"])

pages.addAsyncTab("Home", load, render, route="/home")
pages.loadFailed.connect(lambda index, error: print(index, error))
pages.pageReady.connect(lambda index, widget: print("Ready", index))
```

Each page's loader receives a `threading.Event` cancellation token and must
return **plain Python data**, not a `QWidget` or other `QObject`. The
renderer executes only on the Qt GUI thread and must return a live `QWidget`
without an unrelated parent. The returned widget is reparented to the native
tab placeholder and follows Qt ownership.

By default `cancel_on_leave=True`: moving away while loading sets the
token and invalidates that request's generation. Cancellation is
**cooperative**: it cannot forcibly stop arbitrary Python I/O or CPU work.
Late results from cancelled, superseded or removed tabs are discarded, not
rendered. `cancel_on_leave=False` allows background data to finish and cache
until the user selects that page again, still rendering only the active page.
The bounded worker pool defaults to two threads (configurable 1–8).

- `addAsyncTab`, `registerAsyncTab`, `unregisterAsyncTab` and
  `removeAsyncTab` manage registrations by placeholder identity, so tab
  reorderings cannot remap callbacks.
- `requestPage(index)` starts/retries data loading, `cancelPage(index)`
  invalidates an outstanding request, `isLoading(index)` and
  `isReady(index)` report progress, and `lastError(index)` exposes the
  latest traceback.
- `pageReady`, `loadFailed` and `pageCancelled` are delivered on the
  GUI thread. A failing renderer can be retried using cached data without
  re-running the loader.
- `close()` cancels tasks and releases callbacks without waiting for
  blocking workers; callers must still ensure their worker functions terminate
  promptly. Destruction of the owning Tabs has the same nonblocking cleanup.

Never perform QWidget work in the background loader. This is a threaded data
pipeline, **not asyncio coroutine integration**. Threaded work may still
outlive its cancellation briefly if an I/O operation ignores the token. The
installed-wheel CI checks thread identity, cancellation, stale results, retries,
tab reorderings and QObject deletion on Ubuntu, Windows and macOS.
See `bindings/python/tests/test_async_lazy_tabs.py` and
`bindings/python/examples/async_lazy_tabs.py`.

## Python lazy tab pages (1.17.7)

`LazyTabs` is a Qt-owned Python `QObject` helper for the native
`Widgets.QtMaterialTabs`. It implements lazy page factories without passing
Python closures into Shiboken's unsupported
`std::function<QWidget*()>` binding.

```python
from PySide6.QtWidgets import QLabel, QWidget
from QtMaterial3 import LazyTabs, Widgets

tabs = Widgets.QtMaterialTabs()
lazy = LazyTabs(tabs)  # QObject child of tabs
lazy.addLazyTab("Home", lambda: QLabel("Home"))
lazy.addLazyTab("Settings", lambda: QLabel("Settings"), route="/settings")
tabs.setCurrentIndex(1)  # constructs Settings exactly once
assert lazy.isLoaded(1)
```

The currently selected first tab is built immediately. Other pages are built
upon their first selection. Factories return a **live QWidget** without a
different owning parent. After creation the widget is reparented into the
tab's placeholder layout; Qt owns it and destroys it with its tab/container.
Factory failures are recorded in `lastError(index)` and emitted through
`loadFailed(index, traceback)`, **not thrown across Qt signal callbacks**.
Calling `ensureLoaded(index)` retries a failed factory; reentrant calls cannot
start duplicate creation. `pageLoaded(index, widget)` signals success.

Use `registerTab(index, factory)` for an existing **empty** tab,
`unregisterTab(index)` to release a callback while preserving an already
created widget, or `removeLazyTab(index)` to remove an entry while returning
its detached native tab page. As with `QTabWidget.removeTab`, the caller
must decide what to do with a removed QWidget (for example `deleteLater()`).
Registrations track **page identity**, not tab indices, so moves do not
accidentally attach content to another page.

The native `QtMaterialTabs.setTabFactory(..., std::function)` remains
excluded from the Shiboken ABI. `LazyTabs` provides equivalent page-on-first-
visit behavior in Python, not the same C++ `isTabLoaded()` state. In
particular use `lazy.isLoaded()`, not `tabs.isTabLoaded()`, for Python
registered factories. Asynchronous coroutines and background/threaded
QWidget construction are not supported: factories run on Qt's UI thread.

The installed-wheel suite covers Linux 3.10–3.12, macOS and Windows and tests
lazy activation, error reporting/retry, reentrant calls, ownership, deletion,
and factory reference release. See
`bindings/python/tests/test_lazy_tabs.py` and
`bindings/python/examples/lazy_tabs.py`.

## Navigation controller bridge (1.17.6)

`QtMaterialStackedWidgetController` is a native QObject controller for a
PySide6 `QStackedWidget`. `QtMaterialNavigationController` is an abstract
C++ interface: it can be used for native type checks, not instantiated or
implemented as a Python subclass.

```python
from PySide6.QtWidgets import QStackedWidget, QWidget
from QtMaterial3 import QtMaterialStackedWidgetController, Widgets

stack = QStackedWidget()
stack.addWidget(QWidget())
stack.addWidget(QWidget())

tabs = Widgets.QtMaterialTabs()
tabs.addTab(QWidget(), "Overview")
tabs.addTab(QWidget(), "Settings")

controller = QtMaterialStackedWidgetController(stack)
tabs.bindToController(controller)
stack.setCurrentIndex(1)
assert tabs.currentIndex() == 1
tabs.unbindController(controller)
```

**Borrowed pointers and ownership:** The controller holds its `QStackedWidget`
through a native `QPointer`; the stack is **not** transferred or reparented.
Tabs holds the controller through `QPointer`, so binding also does **not**
make Tabs its owner. Keep the stack/controller alive with Qt parents or
Python references for as long as navigation is needed. Destroying the
stack makes `stackedWidget()` return `None` and `currentIndex()` return
`-1`. Destroying the controller disconnects Qt signals automatically;
explicit `unbindController()` also stops index propagation. Do not rely on
`boundControllers()` from Python; its C++ `QVector` pointer conversion is
deferred together with callback-based tab factories.

The installed-wheel CI executes `tests/test_navigation_controller.py` on
Ubuntu, macOS and Windows, including repeated connect/disconnect and
parent destruction.

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
