# Spec Resolution Architecture

QtMaterial3 uses **dedicated component resolvers** internally, not a central factory facade.

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

Examples of the internal resolver layer include:

- `ButtonSpecResolver`;
- `ActionButtonSpecResolver`;
- `SelectionSpecResolver`;
- `ChipSpecResolver`;
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

## API boundary

Dedicated component resolvers are implementation-owned and are not part of the installed 0.9 API candidate. Applications configure widgets through widget APIs, Theme/ThemeContext, and intentionally authored public spec types.

## Guardrail

Dedicated component resolvers remain the only supported internal resolution boundary. New components must add or extend the appropriate family resolver instead of introducing a central factory facade.
