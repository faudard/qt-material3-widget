# Navigation model

Navigation components should share a route/item model instead of duplicating selection logic.

The shared model is `QtMaterial::QtMaterialNavigationModel`.

Each item contains:

- `id`
- `route`
- `label`
- `icon`
- `testId`
- `enabled`
- `selected`

Tabs, navigation drawer, navigation rail, navigation bar and future menu surfaces can render different views of the same route model.

Release rule: route selection must be exclusive and disabled navigation items must not become selected.
