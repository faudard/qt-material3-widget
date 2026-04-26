# Review checklist for PR I

## API honesty
- `ThemeOptions` documents which fields are consumed today.
- `expressive` and `useMaterialColorUtilities` are no longer presented as fully implemented.
- The studio UI does not imply that `expressive` already changes generation.

## Functional behavior
- `ContrastMode::Standard` should preserve current visual output.
- `ContrastMode::Medium` should slightly reinforce readable foreground and outline roles.
- `ContrastMode::High` should reinforce those roles more strongly.

## Suggested tests
- Build the same seed with Standard / Medium / High and verify visible changes in:
  - `OnPrimary`
  - `OnSecondary`
  - `OnSurface`
  - `OnSurfaceVariant`
  - `Outline`
  - `OutlineVariant`
- Ensure `ThemeSerializer` still round-trips all option fields.

## Non-goals
- No full expressive theme generation
- No full Material Color Utilities backend
- No change to JSON structure beyond comments/documentation
