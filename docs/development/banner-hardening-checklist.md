# QtMaterialBanner hardening checklist

## Rendering
- Resolve `BannerSpec` only through `SpecFactory`
- Cache container path and layout rects
- Paint only from resolved spec values
- Keep dismiss button geometry outside paint branching

## Behavior
- `Esc` dismisses when dismissible
- Dismiss button emits `dismissed()`
- Focus remains keyboard reachable
- Accessible name should follow title text when unset

## Layout
- Leading icon is optional
- Title/body text are elided or wrapped predictably
- Minimum height respects touch and visual spacing
- Dismiss button does not overlap text content

## Tests
- construction
- API round-trip
- dismissed signal
- default render scaffold
