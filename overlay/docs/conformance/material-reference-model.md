# Material Reference Model

QtMaterial3 conformance is not the same thing as internal visual regression.

Internal golden tests answer:

> Did QtMaterial3 change relative to itself?

The Material reference model answers:

> Does QtMaterial3 implement the pinned Material profile within declared platform
> adaptations and tolerances?

## Versioned profiles

The initial profiles are:

```text
Classic:
  androidx.compose.material3:material3:1.4.0
  release: 2025-09-24
  stability: stable

Expressive:
  androidx.compose.material3:material3:1.5.0-alpha26
  release: 2026-08-12
  stability: experimental
```

They are independent profiles. A Classic result can never be compared against an Expressive
reference by accident.

## Evidence hierarchy

The profile records Material guideline pages, the AndroidX executable implementation and
AndroidX release provenance. Numeric expectations are not invented from memory: an assertion
must carry `sourceRefs`.

## Classifications

### MUST_MATCH

Material semantics expected to match across implementations, within an explicitly declared
tolerance where exact raster identity is inappropriate.

Examples:
- component geometry in dp;
- semantic token role;
- state-layer semantics;
- typography role/size/line-height;
- motion duration/easing when platform-independent.

### PLATFORM_ADAPTATION

A deliberate Qt/desktop/platform adaptation.

It requires an explicit assertion/rationale. This classification is not a way to hide a
failed MUST_MATCH result.

Potential examples:
- native font rasterization differences;
- desktop focus affordance required by platform/accessibility conventions;
- platform-specific shadow rasterization where semantic elevation still matches.

### QTMATERIAL_EXTENSION

Behavior or surface intentionally added by QtMaterial3 beyond Material.

Extensions are reported separately and excluded from the Material conformance score. They
cannot replace a missing Material requirement.

## Comparison domains

The v1 model defines:

```text
geometry
tokens
colors
typography
states
motion
accessibility
behavior
```

Accessibility is pass/fail evidence rather than a cosmetic score.

## Typography policy

The reference model compares typography **specification**, not raw glyph pixels. Different
font engines, hinting and platform fonts make byte/pixel equality inappropriate.

## Initial POC matrix

Classic Buttons:

```text
5 variants
  text
  filled
  filled-tonal
  outlined
  elevated

x 5 states
  enabled
  disabled
  hovered
  focused
  pressed

x 2 themes
  light
  dark

= 50 cases
```

Spec 013 intentionally leaves their `assertions` empty and marks the suite `seeded`.
Spec 014 converts those identities into evidence-backed structural expectations.
