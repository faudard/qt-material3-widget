# Spec-first architecture contract

The widget layer must not make ad hoc theme decisions while painting.

The stable rendering flow is:

```text
public widget API + theme + density + interaction state
        ↓
resolved spec
        ↓
layout, paint, accessibility, automation metadata
```

## Authored state

Authored state is the user-facing widget configuration: text, icons, checked state, selected item, error text, density, variant, and optional component overrides.

## Resolved spec

A resolved spec is an immutable snapshot consumed by rendering code. A widget may cache it, but the cache must be invalidated when any of these change:

* theme revision;
* density;
* interaction state;
* authored widget content;
* component override tokens.

## Forbidden pattern

Painting code should not directly perform fresh theme lookups such as:

```cpp
painter.setPen(theme.colorScheme().color(ColorRole::Primary));
```

That decision belongs in a spec factory or resolver.

## Required pattern

```cpp
ensureSpecResolved();
painter.setPen(m_spec.labelColor);
```

## Validation

Specs should be validated before release-level tests assert visual, accessibility, or automation behavior. Runtime validation can be disabled later for performance, but test builds should keep it enabled.
