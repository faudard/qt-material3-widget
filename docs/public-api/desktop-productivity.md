# Desktop and productivity components

Version 0.9 extends the Qt Widgets surface for large desktop applications. These widgets
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

## Split View

`QtMaterialSplitView` derives from `QSplitter`, so native splitter state persistence
remains available through `saveState()` and `restoreState()`. The additional API makes
pane collapsibility and collapsed state explicit. Material split handles are focusable and
keyboard-resizable: Left/Right resize horizontal splits, Up/Down resize vertical splits, and
Shift applies a larger step. Horizontal physical arrow direction is mirrored against pane order
under RTL so the handle still moves in the direction of the pressed arrow.

`setPaneMinimumExtent()` and `setPaneMaximumExtent()` provide orientation-aware convenience
wrappers over the native child-widget size constraints. Passing a non-positive maximum removes
the explicit cap. `resetPaneSizes()` redistributes panes through the native QSplitter sizing
engine, and double-clicking a Material split handle invokes the same reset path.

## Desktop navigation and commands

`QtMaterialBreadcrumb` represents a hierarchical path and emits the activated segment. Each
segment keeps native `QToolButton` keyboard behavior, exposes its position/current-location
state to accessibility, and the breadcrumb exposes the complete path as its accessible
description. Separator glyphs mirror automatically when layout direction changes between LTR
and RTL.

`QtMaterialCommandPalette` composes a search field, `QSortFilterProxyModel` and
`QListView`. The source model remains externally owned and command activation returns a
source-model index. While focus remains in the search editor, Up/Down cycle through results,
Home/End jump to the first/last result, Enter activates the current result and Escape dismisses
the palette. Filtering keeps the first valid result selected and synchronizes an accessible
matching-command count; the result view is exposed as `Command results`.

The composed search/list widgets remain implementation details rather than public API;
applications interact through the query, source-model and activation contracts.

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
