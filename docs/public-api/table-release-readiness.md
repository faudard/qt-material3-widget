
# QtMaterialTable release readiness

`QtMaterialTable` is the Material 3 wrapper around `QTableView`.

## Release contract

The component is considered `usable` when it provides:

- stable model/view integration through `QAbstractItemModel`;
- regular and dense density modes;
- row-oriented selection by default;
- keyboard activation with `Return`, `Enter`, and `Space`;
- accessible table summaries including row/column counts;
- accessible current-cell and row summaries;
- a `rowActivated(int)` signal for row-level flows;
- tests covering construction, density, accessibility, row text, activation, and model updates.

## Intentional scope

The component keeps Qt's native model/view architecture. It does not replace
`QSortFilterProxyModel`, custom item delegates, or domain-specific table models.
Applications should continue to use those standard Qt extension points.

## Validation checklist

```bash
python3 tools/api-check/check_table_release.py
cmake --build build --target tst_table
ctest --test-dir build -R tst_table --output-on-failure
```
