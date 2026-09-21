# Desktop and productivity components

Version 0.9 extends the Qt Widgets surface for large desktop applications. These widgets
are Qt desktop adaptations: they preserve native Model/View, focus, selection, drag/drop
and persistence contracts instead of introducing a parallel application framework.

## 0.9.1 — Tree View

`QtMaterialTreeView` derives from `QTreeView`. Applications retain ownership of their
`QAbstractItemModel`, delegates and MIME/drop implementation. The Material layer adds
explicit dense, multi-selection and drag/drop policies while keeping
`canFetchMore()/fetchMore()` and lazy models native.

Uniform row heights are enabled by default because large models are a primary use case.

## 0.9.2 — Advanced Table

`QtMaterialTable` remains a `QTableView`. Version 0.9 makes desktop policies explicit:
column reordering, cell-vs-row selection and internal drag/drop can be enabled without
replacing the application's model or delegate.

Sorting, resizing, hiding columns, editing and custom delegates continue to use the
standard Qt APIs.

## 0.9.3 — Pagination

`QtMaterialPagination` exposes a one-based page, page size and total count. It does not
own or slice a model; applications can connect `pageChanged` and `pageSizeChanged` to
local proxy models or remote/backend queries.

## 0.9.4 — Split View

`QtMaterialSplitView` derives from `QSplitter`, so native splitter state persistence
remains available through `saveState()` and `restoreState()`. The additional API makes
pane collapsibility and collapsed state explicit.

## 0.9.5 — Desktop navigation and commands

`QtMaterialBreadcrumb` represents a hierarchical path and emits the activated segment.

`QtMaterialCommandPalette` composes a search field, `QSortFilterProxyModel` and
`QListView`. The source model remains externally owned and command activation returns a
source-model index.

## 0.9.6 — Drag and drop

Tree and Table use Qt's native `QAbstractItemView` drag/drop machinery. Enabling the
Material drag/drop policy switches the view to `InternalMove`, enables drop indicators
and keeps `mimeData()` / `dropMimeData()` in the model where Qt expects them.

## 0.9.7 — Certification

The desktop-productivity test target exercises Model/View ownership, a 100000-row virtual
model, selection policies, pagination invariants, splitter behavior, navigation and
command filtering. The release gate requires the same Windows Qt 5.14.2, Windows Qt 6,
Linux Qt 6, macOS Qt 6, sanitizer, packaging and consumer matrix used by earlier releases.
