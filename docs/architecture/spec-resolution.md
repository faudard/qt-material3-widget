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
- `DataComponentSpecResolver`.

## Why there is no central factory facade

A generic facade hides dependencies and encourages unrelated component rules to
accumulate behind one global-looking API. Dedicated resolvers make the dependency
graph, inputs, tests and component ownership explicit.

## Public compatibility policy

The old factory facade is retired before 1.0. It is not:
- compiled;
- installed;
- exported;
- documented as supported API;
- retained as a compatibility alias.

If downstream pre-1.0 code used the old facade, it must migrate to the appropriate
dedicated resolver.

## Guardrail

`tools/check_no_legacy_specfactory.py` scans active public/API/build/test/example
surfaces and fails if the retired facade or filename is reintroduced.

Historical ADR/spec/migration material may name the retired facade when documenting
why it was removed; that is not a supported API surface.
