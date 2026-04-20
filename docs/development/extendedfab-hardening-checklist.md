# QtMaterialExtendedFab Hardening Checklist

- Derive from `QtMaterialFilledButton`
- Override spec resolution through `SpecFactory::extendedFabSpec()`
- Map `FabSpec` into the shared `ButtonSpec` rendering path
- Keep icon + label layout on the shared filled-button path
- Respect the extended FAB touch target in `sizeHint()` and `minimumSizeHint()`
- Reuse filled-button caching, ripple, and focus-ring behavior
- Add keyboard activation coverage
- Add default and disabled render baselines
