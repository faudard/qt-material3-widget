# Spec Resolution Architecture

QtMaterial3 uses **dedicated component resolvers**, not a central factory facade.

The supported conceptual pipeline is:

```text
Authored component spec
        +
SpecContext / state / explicit resolution inputs
        ↓
family/component resolver
        ↓
ResolvedSpec
        ↓
widget/layout/render code
```

Examples of the active resolver surface include:

- `ButtonSpecResolver`;
- `ActionButtonSpecResolver`;
- `SelectionSpecResolver`;
- `TextFieldSpecResolver`;
- `DialogSpecResolver`;
- `AppBarSpecResolver`;
- `TabsSpecResolver`;
- `NavigationRailSpecResolver`;
- `DataSpecResolver`.

## Why there is no central factory facade

A generic facade hides dependencies and encourages unrelated component rules to
accumulate behind one global-looking API. Dedicated resolvers make the dependency
graph, inputs, tests and component ownership explicit.

## Pre-1.0 cleanup policy

Pre-1.0 compatibility shims are intentionally not retained. Retired facades,
forwarding headers and duplicate resolver surfaces are removed before the 1.0
public API is frozen.

## Guardrail

`tools/check_no_legacy_specfactory.py` scans active public/API/build/test/example
surfaces and fails if the retired facade or filename is reintroduced.

Historical ADR/spec/migration material may name the retired facade when documenting
why it was removed; that is not a supported API surface.
