# ThemeModel Value Semantics

`ThemeModel` is a pure design-value layer. It must not know about runtime ownership,
QObject trees, QWidget, platform observation or JSON/XML persistence.

## Theme identity

Theme identity is **structural**:

```text
Theme A == Theme B
```

means all semantically exposed model values are equal:
- ThemeOptions/mode/contrast;
- colors;
- typography;
- shapes;
- elevation;
- motion;
- state-layer;
- accessibility and interaction policy;
- density;
- icon sizes;
- component overrides.

Identity does not depend on:
- JSON key order;
- serializer schema version;
- QHash iteration order;
- QObject/pointer identity;
- ThemeManager;
- process-global state.

`ThemeContext` therefore compares Theme values directly rather than serializing them.

## Snapshot semantics

`Theme` remains mutable while being authored/built. Runtime owners copy the completed
Theme and expose it through const access, so a published runtime Theme is treated as a
snapshot.

This deliberately avoids introducing a second `ThemeSnapshot` type before the need is
proven.

## Why no stable textual fingerprint here

A canonical external fingerprint is an interchange/versioning concern and must have an
explicit versioned canonicalization contract. Spec 007 does not smuggle the current JSON
format back into ThemeModel merely to get a hash.

If a cache/provenance fingerprint is later needed, it must be based on typed structural
identity and an explicit canonical version, not ThemeSerializer output.
