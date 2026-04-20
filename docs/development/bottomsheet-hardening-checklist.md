# QtMaterialBottomSheet Hardening Checklist

## Surface behavior
- [ ] Spec-driven `BottomSheetSpec` resolution only
- [ ] Cached visual/content rects
- [ ] Cached rounded-top container path
- [ ] Shared surface render helper used for paint

## Interaction
- [ ] `open()` / `close()` state path works
- [ ] `Esc` closes the sheet
- [ ] focus moves to a reasonable child on open
- [ ] scrim visibility follows modal state and transition progress

## Layout
- [ ] bottom-edge anchoring is stable on host resize
- [ ] touch target and content padding come from spec
- [ ] expanded height is clamped sanely

## Accessibility / keyboard
- [ ] initial focus handoff works
- [ ] close behavior is keyboard reachable
- [ ] accessible name/description strategy reviewed

## Testing
- [ ] widget behavior test
- [ ] render baseline scaffold
- [ ] gallery example integrated
