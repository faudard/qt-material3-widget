# AppBar release readiness

`QtMaterialTopAppBar` and `QtMaterialBottomAppBar` are release-scope surface/navigation containers.

## Contract

Both app bars provide:

- stable non-empty `sizeHint()` and `minimumSizeHint()`;
- title text synchronized to `accessibleName`;
- `accessibilitySummary()` for screen-reader context;
- `navigationAccessibleName` for icon-only navigation buttons;
- per-action accessible names via `setActionAccessibleName()`;
- keyboard activation of the navigation action with `Space`, `Return`, or `Enter`;
- stable object names for navigation and action buttons used by tests and automation.

`QtMaterialBottomAppBar` additionally exposes `fabAccessibleText()` for the attached FAB.

## Recommended usage

```cpp
QtMaterialTopAppBar* bar = new QtMaterialTopAppBar("Inbox", this);
bar->setNavigationIcon(menuIcon);
bar->setNavigationAccessibleName("Open navigation drawer");
const int search = bar->addActionButton(searchIcon, "Search");
bar->setActionAccessibleName(search, "Search mail");
```

Icon-only controls must always expose a usable accessible name. Tooltip text is accepted as a fallback, but explicit accessible names are preferred for navigation and high-value actions.
