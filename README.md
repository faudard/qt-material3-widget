# QtMaterial3 Spec 003 — Build and Consumer Matrix

Prerequisite: the cumulative Spec 002 patch must already be applied.

## Apply

```powershell
python apply_spec003.py --repo E:\Project\qt-material3-widget
python apply_spec003.py --repo E:\Project\qt-material3-widget --apply
```

Then:

```powershell
python -m unittest discover -s tests/tools -p "test_*.py" -v
python tools/check_build_consumer_matrix.py
python scripts/ci/run-consumer-matrix.py --profile ci
git diff --check
git diff
```

For exhaustive validation:

```powershell
python scripts/ci/run-consumer-matrix.py --profile full
```

The CI profile is bounded/pairwise; full covers all consumption mechanism × linkage combinations.
