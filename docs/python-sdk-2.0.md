# Python / PySide6 SDK 2.0 — implementation scope for 1.28

> Status: development milestone; **not a published 1.28 wheel or certified release**. See [roadmap](roadmap-2.0.md).

## C++/Python parity matrix

| Workflow | Native C++ implementation | Python exposure | Contract |
| --- | --- | --- | --- |
| Themes | Theme, ThemeBuilder, ThemeContext | Existing | One native implementation; borrowed ThemeContext where documented |
| Controls | Buttons, Checkbox, Radio, Switch, Chip, Text Fields | Existing | Native QWidget properties and signals |
| Navigation | Tabs, Route, NavigationModel, controllers, Bar, Rail | Existing | AsyncLazyTabs/NavigationSession are optional Python orchestration |
| Responsive navigation | NavigationSuite, Breadcrumb | **New in 1.28** | Native destinations, adaptive size, breadcrumb items and signals |
| Modal feedback | Dialog, Snackbar, Tooltip, Badge | Existing | Application-owned actions; complex callback/queue APIs deferred |
| Desktop data | Table, TreeView | **New in 1.28** | External QAbstractItemModel and proxy/delegate ownership preserved |
| Workspace view state | Table, TreeView, NavigationSuite | **New typed helpers** | Native versioned QByteArray serialization and validation |
| Advanced command providers | CommandPalette callback providers | Not included | Requires separate lifetime/cancellation qualification |
| Full C++ catalogue | Remaining widget families | Not claimed | Per-workflow ABI and ownership testing required |

These are direct Shiboken6 bindings to existing C++ widgets. No Python-painted clones are introduced.

## Developer workflow

Install a wheel built with the exact supported matrix: 64-bit CPython 3.10–3.12, PySide6/Shiboken6 **6.6.3**, matching Qt **6.6.3** development SDK. The Python bindings remain **Qt 6-only**; C++ retains Qt 5.14.2 compatibility.

Run `python bindings/python/pyside_config.py --verify-runtime-abi` before launching `python bindings/python/examples/desktop_workspace.py`. For contracts, run `python -m unittest discover -s bindings/python/tests -p 'test_*.py' -v` against the installed wheel.

The desktop example combines NavigationSuite, Breadcrumb, an application-owned QStandardItemModel in both Table and TreeView, a native Material Dialog and live ThemeContext updates.

## Model lifetimes and snapshot contract

- Table and TreeView derive from QTableView and QTreeView. Keep models, QSortFilterProxyModel chains, delegates and business actions in application ownership. Assign a separate QObject parent or retain explicit Python references; calling setModel does **not** transfer ownership.
- Table.setThemeContext accepts a borrowed context. Keep that context alive independently of the view, and run all QWidget operations on the Qt GUI thread.
- Deleting a Qt parent invalidates Python Shiboken wrappers for its child widgets. Never dereference stale wrappers from background callbacks.
- Native saveWorkspaceState and restoreWorkspaceState persist view geometry and policies only. Data models, business state, delegates and QObject pointers are not serialized.
- The typed Python WorkspaceStateful protocol, save_workspace_state(view) -> bytes and restore_workspace_state(view, snapshot: bytes) -> bool are thin pass-throughs to the native implementation. Empty, corrupt or incompatible snapshots fail closed.
- The PEP 561 py.typed marker covers authored Python helpers. Complete type stubs for every Shiboken-generated class are **not** claimed.

## Qualification gate

The existing wheel CI builds and installs **one wheel per runner**: Linux CPython 3.10, 3.11, 3.12; macOS 3.11; Windows 3.11. Each advertised target must pass pinned ABI preflight, wheel-tag and payload checks, import from the installed wheel, lifecycle/ownership tests and async navigation cancellation tests. The 1.28 desktop tests join that matrix and CTest.

Passing these checks is necessary but does **not** prove manylinux repair, notarization, universal relocatability or PyPI publication. Until final PR checks pass, this remains an unqualified 1.28 development increment.
