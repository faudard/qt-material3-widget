# Desktop and productivity components

The desktop components extend the Qt Widgets surface for large desktop applications. These widgets
are Qt desktop adaptations: they preserve native Model/View, focus, selection, drag/drop
and persistence contracts instead of introducing a parallel application framework.

## Tree View

`QtMaterialTreeView` derives from `QTreeView`. Applications retain ownership of their
`QAbstractItemModel`, delegates and MIME/drop implementation. The Material layer adds
explicit dense, multi-selection and drag/drop policies while keeping
`canFetchMore()/fetchMore()` and lazy models native.

Uniform row heights are enabled by default because large models are a primary use case.
Focused certification keeps QTreeView keyboard navigation authoritative and covers RTL layout
plus DPR 2.0 rendering without materializing large models.

## Advanced Table

`QtMaterialTable` remains a `QTableView`. Version 0.9 makes desktop policies explicit:
column reordering, cell-vs-row selection and internal drag/drop can be enabled without
replacing the application's model or delegate.

Sorting, resizing, hiding columns, editing and custom delegates continue to use the
standard Qt APIs. The focused Table suite also certifies the Material accessibility summary,
native keyboard activation, RTL header/layout propagation and DPR 2.0 rendering.

## Pagination

`QtMaterialPagination` exposes a one-based page, page size and total count. It does not
own or slice a model; applications can connect `pageChanged` and `pageSizeChanged` to
local proxy models or remote/backend queries. Pagination mirrors first/previous/next/last
chevrons in RTL, keeps native focus/keyboard activation on the child controls, and exposes the
current page/range through the container accessible description.

## Split View 2.0

`QtMaterialSplitView` retains the native QSplitter layout and constraint engine. Pane
minimum/maximum extents are orientation-aware; a non-positive maximum removes the cap.
Focusable dividers support arrows, Shift for a larger step, Home/End for the physical
limits and Enter to collapse/restore the preceding pane. The configurable
`keyboardResizeStep` defaults to 16 pixels. Physical movement mirrors correctly in RTL.

`resetPaneSizes()` and a left-button double-click restore `defaultPaneSizes` when the
list matches the pane count, otherwise equal sizes. Reset also expands collapsed panes
and restores their previous collapsibility policy. Native widget constraints remain
in force.

Collapse is immediate by default. Enable `animatedCollapseEnabled` to interpolate pane
sizes over `collapseAnimationDuration` (180 ms by default). Temporary size-policy/minimum
adjustments are restored on completion, reversal, reset or hide. Disable animation or
set its duration to zero when the application's reduced-motion policy is enabled.

`rememberPaneSizes` remembers the layout across hide/show within the widget's lifetime.
`savePaneState()`/`restorePaneState()` also preserve the previous expanded extents and
collapsibility of individually collapsed panes. They use a versioned envelope around
native splitter state and reject mismatched pane counts/orientations or corrupt envelopes.
Native `saveState()`/`restoreState()` remain available; use the extended pair when
restoring an explicitly collapsed pane's policy matters.

Storage belongs to the application. Set pane constraints before restoring state and use
stable pane ordering. `paneStateChanged` publishes a snapshot after divider movement
(debounced), collapse completion, reset and hide:

```cpp
QSettings settings;
split->restorePaneState(settings.value("workspace/layout").toByteArray());
connect(split, &QtMaterial::QtMaterialSplitView::paneStateChanged,
        window, [](const QByteArray& state) {
    QSettings settings;
    settings.setValue("workspace/layout", state);
});
```

## Breadcrumb 2.0

`QtMaterialBreadcrumb` exposes a hierarchy through native tool buttons. Its accessible
description retains the full path, each segment reports its position/current state, and
separators mirror in RTL. `maximumVisibleItems` bounds visible segments; hidden ranges
remain available through native overflow menus. `responsiveElisionEnabled` automatically
collapses ranges as width shrinks. Individual long labels elide in their paint area while
retaining their complete accessible name and tooltip. Icons set with `setItemIcon` appear
both on visible segments and hidden menu actions. Current segments remain visible, even
when a two-item limit cannot also retain both root and leaf.

Enable `locationEditable`, then call `setEditingLocation(true)`, press Ctrl+L while the
breadcrumb or a child has focus, or double-click its background. The editor initially
selects the explicit `location`, or the path through `currentIndex` joined with ` / `.
Enter emits `locationSubmitted`; Escape cancels. The application validates/resolves the
submitted address before changing items or location. Exiting restores available focus.

URL drag/drop is opt-in through `setDragDropEnabled(true)`. Set a segment's URL with
`setItemUrl`; dragging uses native URL MIME data. Drops emit `urlsDropped(index, urls)`
for the target segment (or current segment on the background). They accept CopyAction
only; the application handles the requested operation. The component never moves or
deletes files. `setItems` resets segment icons/URLs, which the application can reapply.

## Command Palette 2.0

`QtMaterialCommandPalette` supports an external flat `QAbstractItemModel`, any number of
application-owned providers, or both. Ctrl+K and Ctrl+P open it in its parent window and
focus the search editor. `setActivationShortcuts` replaces these shortcuts; pass an empty
list to manage opening yourself. `openPalette()` resets the query and refreshes providers.

Fuzzy matching is enabled by default: ordered character subsequences, contiguous runs,
word boundaries and complete title matches influence ranking. Query tokens can appear in
any word order. Keywords and secondary text participate in searching; exact title matches
receive priority. `setFuzzyMatchingEnabled(false)` selects literal substring filtering.
Results are grouped into Favorites, Recent commands and explicit sections, ranked within
each group. Icons, secondary descriptions and shortcut text have dedicated display areas.

### Providers and asynchronous results

```cpp
palette->addProvider(commandProvider);
palette->addProvider(recentFilesProvider);
palette->addProvider(settingsProvider);
```

Derive from `QtMaterialCommandProvider` and implement:

- `requestCommands(query, requestId)`: publish one completed snapshot with
  `commandsReady(requestId, QList<QtMaterialCommand>)`, or report `requestFailed`.
- `activateCommand(id)`: execute the selected command.
- `cancelRequest(requestId)`: optionally stop pending work; the default is a no-op.

Providers remain application-owned and are never reparented. Calls execute in the
provider's QObject thread, and result delivery uses Qt's queued connections when needed.
That thread needs an event loop. A provider in the GUI thread must start asynchronous
work rather than block `requestCommands`. Queries are coalesced over 60 ms; explicit
`refreshProviders()` requests immediately. Superseded queries, hide, provider removal and
destruction invalidate outstanding requests. Stale replies never overwrite current
results. Loading is exposed as a property/signal and a search status; the empty state is
shown only when all requests have completed and no commands match. Provider errors emit
`providerFailed`, allowing the application to explain or retry failures.

Each command has a nonempty stable `id`, `text`, optional `secondaryText`, `section`,
`keywords`, `shortcut`, `icon` and an `enabled` state. IDs must be unique across providers
and the external model; the first source/provider snapshot wins on collisions. Empty IDs
or labels from providers are ignored. Activation emits `providerCommandActivated` and
then dispatches to the provider. Providers can be safely removed/destroyed during use.

### Models, favorites and history

External models retain ownership and emit `commandActivated` with their original index.
Existing `ShortcutRole` values continue to work. New model roles include `IdRole`,
`SecondaryTextRole`, `SectionRole` and `KeywordsRole`; standard DecorationRole supplies
icons. Enabled/selectable flags remain authoritative. `SectionHeadingRole`, `FavoriteRole`
and `RecentRole` are computed result roles. The child search/list widgets remain internal.

Favorites and MRU history use stable IDs and each command appears once. `setCommandFavorite`
toggles a favorite; `setFavoriteCommandIds` restores saved favorites. Activation moves its
ID to the front of `recentCommandIds`, a deduplicated list capped at 20 entries.
`setRecentCommandIds` restores history; `clearRecentCommands` clears it. Models without IDs
still activate normally but do not participate in persistent favorites/history. Connect
the corresponding change signals to application-owned settings storage if required.

Up/Down cycle through enabled, selectable results; Home/End select the first/last,
PageUp/PageDown move by a page, Enter activates, and Escape dismisses. Space also activates
when the result list has focus. Ctrl+D toggles the selected command's favorite. Tab and
Shift+Tab keep native dialog focus navigation within the palette. Disabled commands remain
visible but cannot activate. Accessible result text includes secondary/shortcut metadata,
and the palette describes matching results and loading status.

## Drag and drop

Tree and Table use Qt's native `QAbstractItemView` drag/drop machinery. Enabling the
Material drag/drop policy switches the view to `InternalMove`, enables drop indicators
and keeps `mimeData()` / `dropMimeData()` in the model where Qt expects them.

## Certification

The desktop-productivity test target exercises Model/View ownership, a 100000-row virtual
model, Table accessibility/keyboard/RTL/HiDPI behavior, Tree native keyboard/RTL/HiDPI
behavior, Pagination range/accessibility/RTL/HiDPI behavior, Split View collapse plus
keyboard-resize/RTL/HiDPI behavior, Breadcrumb accessibility/RTL, and Command Palette
keyboard/result accessibility. The focused Navigation Rail target additionally certifies
disabled-item skipping, visual-direction RTL keyboard behavior and HiDPI rendering.

The visual-regression harness emits `desktop_data_matrix_*` candidates for Table, Tree View,
Pagination and Split View across default, disabled and RTL states in light, dark and
high-contrast themes. These remain review candidates until approved as stable release goldens.

The release gate requires the same Windows Qt 5.14.2, Windows Qt 6, Linux Qt 6, macOS Qt 6,
sanitizer, packaging and consumer matrix used by earlier releases.

The registered `tst_desktop_navigation_v2` suite covers fuzzy matching, provider merging,
stale replies/cancellation, worker-thread dispatch, failure/destruction, favorites/history,
disabled keyboard activation, host shortcuts, location editing, overflow icons/limits,
URL drops, extended splitter persistence, animation reversal and constraint restoration.
