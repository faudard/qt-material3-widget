# QtMaterialCard hardening pass 21

This archive is a focused patch-style scaffold for `QtMaterialCard`.

It assumes the existing Phase 5 refactored architecture:
- `QtMaterialSurface`
- `QtMaterial::CardSpec`
- `QtMaterial::SpecFactory`
- shared surface paint helper in `src/widgets/surfaces/private/qtmaterialsurfacerenderhelper_p.*`

## Intent

`QtMaterialCard` should become a real spec-driven surface widget, not a placeholder paint stub.

This pass includes:
- a real public header / implementation sketch for `QtMaterialCard`
- cached surface geometry
- spec-driven `sizeHint()` / `minimumSizeHint()`
- focus and accessibility basics
- widget and render test scaffolding
- a hardening checklist

## Notes

This archive is **not** a compiled validation result.
It is a focused best-effort patch scaffold designed to be merged into your integrated branch and then reconciled with local build/test feedback.
