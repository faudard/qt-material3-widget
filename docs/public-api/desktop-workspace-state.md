# Desktop workspace state

QtMaterial3 desktop applications can persist user workspace preferences without
serializing application data or introducing a framework-specific settings
service.

The 1.12 workspace-state contract is deliberately small and additive. Existing
widgets expose a versioned `QByteArray` that applications may store in
`QSettings`, a project/session file, or another application-owned persistence
layer.

## Supported widgets

| Widget | Persisted state |
| --- | --- |
| `QtMaterialTable` | column order/size/visibility, sort presentation, dense mode, row/cell selection policy, multi-selection, column reordering, internal drag/drop, inline editing and context-menu policy |
| `QtMaterialTreeView` | header order/size/visibility, dense/multi-selection/drag-drop/editing/context-menu policies, current item and the visible expanded hierarchy |
| `QtMaterialCommandPalette` | favorites, recent commands and fuzzy-matching preference |
| `QtMaterialNavigationSuite` | selected destination, adaptive width class and destination enabled states |
| `QtMaterialSplitView` | existing pane geometry/collapse schema through the uniform workspace-state naming |

No widget serializes the application model, command providers, command source
model, icons or application business data.

## Basic use

The returned state is a `QByteArray`, so it can be stored directly in
`QSettings`.

```cpp
QSettings settings;

settings.setValue(
    "workspace/table",
    table->saveWorkspaceState());
settings.setValue(
    "workspace/tree",
    tree->saveWorkspaceState());
settings.setValue(
    "workspace/palette",
    palette->saveWorkspaceState());
settings.setValue(
    "workspace/navigation",
    navigation->saveWorkspaceState());
settings.setValue(
    "workspace/split",
    splitView->saveWorkspaceState());
```

Restore after the application has rebuilt the corresponding structure:

```cpp
table->setModel(tableModel);
tree->setModel(treeModel);

navigation->addDestination("Home");
navigation->addDestination("Projects");
navigation->addDestination("Settings");

table->restoreWorkspaceState(
    settings.value("workspace/table").toByteArray());
tree->restoreWorkspaceState(
    settings.value("workspace/tree").toByteArray());
palette->restoreWorkspaceState(
    settings.value("workspace/palette").toByteArray());
navigation->restoreWorkspaceState(
    settings.value("workspace/navigation").toByteArray());
splitView->restoreWorkspaceState(
    settings.value("workspace/split").toByteArray());
```

Applications should treat a `false` result as a normal compatibility miss and
continue with their current/default workspace.

Table and Tree View keep editing and menu ownership native: `inlineEditingEnabled`
only selects native edit triggers (DoubleClick/F2), while `contextMenuEnabled` enables a
`contextMenuRequested(index, globalPosition)` signal for application-owned menus. Those
preferences are part of the workspace snapshot.

## Compatibility and fail-safe restore

Each payload carries its own magic value and schema version.

Restore validates the complete payload before applying application-visible
state. Truncated data, unsupported schema versions and incompatible component
structure are rejected.

### Table

Table restore requires the current horizontal section count to match the saved
table. The native header state is validated before the Material interaction
policies are applied.

This means applications should install the model before restoring.

### Tree View

Tree View saves row paths relative to the current `rootIndex()`. It records
only expanded branches that are part of the visible expanded hierarchy and the
current item/column.

Restore requires:

- the same column count;
- a compatible root index;
- every saved row path to resolve against the current model.

All row paths are resolved before the header, expansion or current item is
changed. Applications with model identities that can move independently of row
position should persist their own stable IDs and rebuild the desired expansion
state at the application layer instead.

### Command Palette

Palette state contains user preferences only. It intentionally excludes:

- the transient search query;
- providers and outstanding requests;
- source models;
- activation shortcuts.

Recent commands are normalized through the same bounded recent-list policy used
by the live palette.

### Navigation Suite

Navigation state embeds destination text/order as a compatibility signature.
Restore is rejected when the current destination list does not match the list
that produced the state.

This avoids silently applying a saved selected index or disabled flag to a
different destination after an application navigation redesign.

### Split View

`saveWorkspaceState()` and `restoreWorkspaceState()` are aliases for the
established, versioned `savePaneState()` / `restorePaneState()` schema.
There is intentionally only one Split View persistence format.

## Testing contract

`tst_desktop_workspace_state` verifies:

- full round-trip for every supported widget;
- Table/Tree column order, size and visibility persistence;
- Table column topology rejection;
- Tree hierarchy/topology rejection;
- Command Palette preference normalization;
- Navigation Suite destination compatibility;
- Split View schema aliasing;
- malformed/truncated state rejection without changing the current state.

`tst_desktop_productivity_112` complements persistence with the runtime desktop
contracts: advanced keyboard selection, inline edit commit/cancel, proxy
sort/filter compatibility, indexed context-menu hooks, native drag/drop policy,
exact Split View size restoration, large asynchronous provider cancellation and
repeated Navigation Bar ↔ Rail transitions with state/focus preservation.

The contracts are exercised on the normal Qt 5.14.2 / Qt 6 cross-platform CI
matrix and remain additive to the stable 1.x API.
