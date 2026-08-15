# PR 31 — GridList release readiness

This patch turns `QtMaterialGridList` from an empty/public placeholder into a usable Material-style grid/list component.

## Files written

- `include/qtmaterial/widgets/data/qtmaterialgridlist.h`
- `src/widgets/data/qtmaterialgridlist.cpp`
- `tests/widgets/data/tst_gridlist.cpp`
- `docs/public-api/gridlist-release-readiness.md`
- `tools/api-check/check_gridlist_release.py`

## Files updated by the apply script

- `tests/CMakeLists.txt`
- `docs/components/component-registry.json`
- `STATUS.md`

## Validation commands

```bash
python3 gridlist_release_readiness_patch/apply_gridlist_release_readiness_patch.py
python3 tools/api-check/check_gridlist_release.py
cmake --build build --target tst_gridlist
ctest --test-dir build -R tst_gridlist --output-on-failure
```
