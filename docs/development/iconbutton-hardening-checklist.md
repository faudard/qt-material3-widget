# QtMaterialIconButton hardening checklist

This checklist tracks the quality bar for `QtMaterialIconButton`.

## Implementation
- [x] spec-driven rendering
- [x] icon-only geometry with touch target separation
- [x] selected / unselected state
- [x] disabled state colors
- [x] focus ring rendering
- [x] ripple clipped to container shape
- [x] icon tinting path

## Caching
- [x] cached visual container rect
- [x] cached icon rect
- [x] cached rounded container path
- [x] cache invalidation on resize / theme / style / content change

## Interaction
- [x] keyboard activation
- [x] focusable by keyboard
- [x] selected state API

## Validation
- [x] basic widget test
- [x] render baseline scaffolding

## Still to verify locally
- [ ] compile cleanly on the integrated branch
- [ ] verify icon tinting across platforms/themes
- [ ] verify ripple repaint region behavior
- [ ] verify accessibility metadata on the integrated branch
