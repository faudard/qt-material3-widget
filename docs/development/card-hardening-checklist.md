# QtMaterialCard Hardening Checklist

Use this checklist when integrating the `QtMaterialCard` implementation.

## Architecture
- [ ] derives from `QtMaterialSurface`
- [ ] resolves only `QtMaterial::CardSpec`
- [ ] does not perform raw theme lookups in `paintEvent()`
- [ ] uses shared surface rendering helpers

## Caching
- [ ] caches visual rect
- [ ] caches content rect
- [ ] caches rounded container path
- [ ] invalidates cache on resize
- [ ] invalidates cache on theme/spec changes
- [ ] invalidates cache on font/style/layout-direction changes

## Rendering
- [ ] paints from resolved spec only
- [ ] supports enabled and disabled colors
- [ ] focus ring is spec-driven
- [ ] shadow is delegated to shared surface helper

## Behavior
- [ ] `sizeHint()` is spec-driven
- [ ] title/body setters invalidate layout and geometry
- [ ] focus policy is keyboard-correct
- [ ] accessible name sync is sane

## Tests
- [ ] widget test builds
- [ ] render baseline test builds
- [ ] gallery card page or card sample exists
