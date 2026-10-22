# Desktop Productivity 3.0 (1.29) — application-owned workflows

This is an additive desktop-workflow layer, **not** a replacement for native
Qt Model/View or an announcement that 1.29 has been released. Existing 1.12
workspace payloads remain unchanged.

## Multi-key sorting and token search

`QtMaterialMultiSortFilterProxy` is an optional `QSortFilterProxyModel`
subclass. The application creates and owns the source model and each proxy.
Tables and trees only consume the final proxy:

```cpp
auto* model = new MyTableModel(this); // application model
auto* multi = new QtMaterial::QtMaterialMultiSortFilterProxy(this);
multi->setSourceModel(model);
multi->setSortKeys({{0, Qt::AscendingOrder}, {3, Qt::DescendingOrder}});
multi->setSearchColumns({0, 2, 3});
multi->setSearchQuery("open customer");

auto* permissions = new QSortFilterProxyModel(this);
permissions->setSourceModel(multi); // a second app-owned filter

auto* table = new QtMaterial::QtMaterialTable(this);
table->setSortingEnabled(false); // multi's keys are authoritative
table->setModel(permissions);
```

Sorting is lexicographic by key, applying each key's independent ascending
or descending order, with source row order as the deterministic tie-breaker.
The comparison uses Qt's native proxy `lessThan` for type-sensitive cells.
Negative, duplicated or (when source is installed) out-of-range column keys
are rejected without altering existing keys. The proxy does not alter a
source model or its delegates.

The query is split on whitespace into case-folded tokens. Every token must
occur in at least one configured search column; different tokens may match
different columns. The application can still set Qt's native filter rules,
which are combined using AND. Empty search columns means all columns. The
proxy searches already-materialized rows and does not call `fetchMore()`.

**Integration note:** avoid calling `table->sortByColumn` as the source of
truth for a multi-key order; use `setSortKeys` and install explicit header
click/Shift+click policies in the application if they are desired. A proxy
inside an existing chain can also be sorted by an outer proxy: outer sort
ordering then takes precedence over its inner source order.

## Persist current item and selected cells across sorting

The existing `saveWorkspaceState()` remains responsible for header layout,
sort presentation, pane geometry and (for the tree) row-path-based expansion.
New `QtMaterialSelectionState` is a separate, opt-in selection snapshot.

```cpp
constexpr int StableIdRole = Qt::UserRole + 42;
// The model must return a unique, nonempty string ID in column zero
// at StableIdRole for every loaded row, including tree children.
const QByteArray state =
    QtMaterial::QtMaterialSelectionState::save(table, StableIdRole);

// Recreate data, source/proxy chains, header settings and tree expansion first.
bool restored = QtMaterial::QtMaterialSelectionState::restore(
    table, state, StableIdRole);
```

The versioned state stores the selected cell IDs/columns and current ID/column,
not row numbers, pointers, raw model contents or Qt internal indexes. It works
across reordered/filtered proxy chains, including nested proxies, as long as
the selected items remain visible and have unique IDs. Failures, including
missing IDs, duplicate/ambiguous IDs, invalid columns, truncated streams and
oversize snapshots, leave selection and current index unchanged. Saving at
most 8,192 selected cells is supported. Restore performs a bounded, read-only
scan of up to 250,000 loaded rows by default; pass a higher explicit limit
for larger models if an O(N) selection restore is acceptable. No background
fetch, `fetchMore()`, editor activation or focus stealing is performed.

Use the existing tree `saveWorkspaceState()` for expanded row paths when the
model topology is compatible. For models whose node positions change, keep
expanded-node IDs in the application alongside selection IDs; do not assume
the 1.12 row-path expansion state can remap reordered hierarchies.

## Commands and persistent reference workspaces

The existing `QtMaterialCommandPalette` already supports grouped results,
favorites, recents, shortcuts, provider cancellation and stale-result rejection.
Its providers and source model are application-owned. The 1.29 explorer
reference demonstrates command-driven navigation and persisted favorites and
history without duplicating those features.

Run the source-tree reference target:

```sh
cmake -S . -B build -DQTMATERIAL3_BUILD_EXAMPLES=ON
cmake --build build --target qtmaterial3_desktop_productivity_3
```

The example composes an Explorer (multi-proxy Table + Tree + search + Split
View), a Dashboard, Settings, and a Command Palette, and persists the view,
selection, panes and last page through application-owned `QSettings`.
Ctrl+K opens the palette. No runtime widgets depend on the example.

## Tests and certification boundaries

`tst_desktop_productivity_129` tests independent multi-key sort directions,
source/proxy ownership, token filtering with a second proxy, invalid keys,
stable-ID selection through re-sorting, rejection of filtered/duplicate IDs,
tree state without lazy fetch, palette preference roundtrip and a 100k-row
virtual-model navigation smoke contract.

The existing `tst_desktop_productivity_112`, `tst_commandpalette_stress`,
`tst_desktop_scale_2_contracts` and 1.14/1.21 benchmarks remain the
non-regression baselines. Native NVDA, Orca and VoiceOver verification and
approved visual-golden coverage are **separate** release gates; automated
QAccessible/keyboard tests cannot certify spoken AT output. Build and
runtime CI outcomes must be reviewed before treating 1.29 as qualified.
