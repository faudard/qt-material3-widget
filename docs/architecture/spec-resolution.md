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

## Guardrail

Dedicated component resolvers are the only supported resolution surface. New components must add or extend the appropriate family resolver instead of introducing a central factory facade.
