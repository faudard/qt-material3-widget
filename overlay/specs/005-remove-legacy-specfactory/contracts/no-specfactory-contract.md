# No-SpecFactory Contract v1

The supported spec-resolution architecture is:

```text
AuthoredSpec + explicit context/state
        ↓
Dedicated component/family resolver
        ↓
ResolvedSpec
```

Forbidden on active surfaces:
- `SpecFactory`;
- `QtMaterialSpecFactory`;
- `qtmaterialspecfactory`;
- filenames representing a spec factory;
- documentation that presents a spec factory/factory path as current architecture.

Historical migration/spec/ADR text may mention the retired facade.

This is a pre-1.0 hard removal, not a deprecation shim.
