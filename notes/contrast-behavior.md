# Minimal contrast behavior

This PR intentionally implements only a **small post-generation contrast pass**.

## Why this scope
The current builder already generates a full fallback scheme from:
- `sourceColor`
- `mode`

The cheapest way to make `contrast` real without destabilizing the generator is to adjust a few
foreground and divider-like roles after scheme generation.

## Roles adjusted
- `OnPrimary`
- `OnSecondary`
- `OnSurface`
- `OnSurfaceVariant`
- `Outline`
- `OutlineVariant`

## Strategy
For light themes:
- move selected roles slightly toward black

For dark themes:
- move selected roles slightly toward white

This is intentionally conservative and easy to test.
