# Widget migration branching pass

This patch wires the release architecture abstractions into existing widgets instead of keeping them as unused helper types.

## Migrations

- `QtMaterialInputControl` now derives from `QtMaterialFormFieldControl`.
- Existing input APIs remain source-compatible:
  - `labelText()` / `setLabelText()` map to `label()` / `setLabel()`.
  - `supportingText()` / `setSupportingText()` map to `helperText()` / `setHelperText()`.
  - `hasErrorState()` / `setHasErrorState()` map to `invalid`.
- Linear and circular progress indicators now derive from `QtMaterialWidget` and expose `QtMaterialAsyncState` through `busy`, `indeterminate`, `statusText`, and `asyncState()`.
- `QtMaterialTabs` now binds to `QtMaterialNavigationModel` and synchronizes routes, ids, labels, enabled state, test ids, and selection.
- `QtMaterialAbstractButton` now exposes the same automation metadata contract as `QtMaterialWidget` through dynamic properties powered by `QtMaterialAutomation`.
- `tst_widget_migration_contracts` verifies that the migration is actually connected.

## Validation

```bash
cmake -S . -B build -DQTMATERIAL3_BUILD_TESTS=ON -DQTMATERIAL3_BUILD_EXAMPLES=ON
cmake --build build --parallel
ctest --test-dir build -R "widget_migration|formfield|asyncstate|navigationmodel|automation" --output-on-failure
```
