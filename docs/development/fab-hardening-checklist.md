# QtMaterialFab Hardening Checklist

- Derive from `QtMaterialFilledButton`
- Override spec resolution through `SpecFactory::fabSpec()`
- Map `FabSpec` into the shared `ButtonSpec` rendering path
- Keep the FAB icon-only and circular/full-shape
- Respect the FAB touch target in `sizeHint()` and `minimumSizeHint()`
- Reuse filled-button caching, ripple, and focus-ring behavior
- Add keyboard activation coverage
- Add default and disabled render baselines
