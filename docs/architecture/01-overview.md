# Architecture overview

`qt-material3-widget` is organized as a layered Qt Widgets framework.

The core rule is:

> Widgets render from resolved specs, not from ad hoc theme lookups.

## Core architecture

```mermaid
flowchart TB
    app["Applications / Consumers<br/><small>Use QtMaterial3::Theme and QtMaterial3::Widgets</small>"]

    inputs["Theme Inputs<br/><small>Seed color<br/>ThemeOptions<br/>JSON import / export</small>"]
    m3["Material 3 Guidance<br/><small>Color roles<br/>Typography<br/>Shape<br/>Motion</small>"]

    legacy["qtmaterial3_legacy<br/><small>Temporary migration bridge<br/>Legacy alias map<br/>Legacy theme bridge</small>"]

    theme["qtmaterial3_theme<br/><small>Tokens, schemes, builder, manager<br/>ColorScheme · TypographyScale · ShapeScale<br/>Shape · ElevationScale · MotionTokens<br/>ThemeBuilder · ThemeManager</small>"]

    rule["Core rule:<br/>Widgets render from resolved specs,<br/>not from ad hoc theme lookups."]

    core["qtmaterial3_core<br/><small>Widget bases and interaction state<br/>Widget / Surface bases<br/>AbstractButton<br/>SelectionControl<br/>InputControl<br/>InteractionState</small>"]

    specs["qtmaterial3_specs<br/><small>Immutable component specs + factory<br/>SpecFactory<br/>Button / Dialog / TextField<br/>Card / NavigationDrawer<br/>Banner / AppBar / List / DateField</small>"]

    effects["qtmaterial3_effects<br/><small>Visual effects and motion<br/>Ripple · State layer painter · Focus indicator<br/>Shadow · Transition controller · Scrim</small>"]

    widgets["qtmaterial3_widgets<br/><small>Public widget implementations<br/>Buttons · Selection · Inputs · Surfaces · Data<br/>TextButton, FilledButton, FAB, Checkbox, Switch, RadioButton,<br/>Dialog, NavigationDrawer, BottomSheet, Banner, Card,<br/>AppBars, TextFields, List, Divider</small>"]

    qt["Qt Platform<br/><small>Qt Core · Qt Gui · Qt Widgets</small>"]

    inputs --> theme
    m3 --> theme
    legacy --> theme

    theme --> app
    widgets --> app

    theme --> rule
    rule --> core
    rule --> specs
    rule --> effects

    theme --> core
    theme --> specs
    theme --> effects
    theme --> widgets

    core --> specs
    core --> effects
    core --> widgets

    specs -->|"resolved specs"| widgets
    effects --> widgets

    qt -. supports .-> theme
    qt -. supports .-> core
    qt -. supports .-> specs
    qt -. supports .-> effects
    qt -. supports .-> widgets

    classDef consumer fill:#eef5ff,stroke:#2563eb,stroke-width:1.5px,color:#0f172a;
    classDef theme fill:#eff6ff,stroke:#1d4ed8,stroke-width:1.5px,color:#0f172a;
    classDef core fill:#ecfdf5,stroke:#059669,stroke-width:1.5px,color:#0f172a;
    classDef specs fill:#f5f3ff,stroke:#7c3aed,stroke-width:1.5px,color:#0f172a;
    classDef effects fill:#faf5ff,stroke:#9333ea,stroke-width:1.5px,color:#0f172a;
    classDef widgets fill:#f3e8ff,stroke:#6d28d9,stroke-width:1.8px,color:#0f172a;
    classDef support fill:#f8fafc,stroke:#64748b,stroke-width:1px,color:#0f172a;
    classDef note fill:#fff7ed,stroke:#f59e0b,stroke-width:1.5px,color:#431407;

    class app consumer;
    class inputs,m3,legacy,qt support;
    class theme theme;
    class core core;
    class specs specs;
    class effects effects;
    class widgets widgets;
    class rule note;
```

## Theme-to-widget flow

```mermaid
sequenceDiagram
    participant App as Application
    participant Manager as ThemeManager
    participant Theme as Theme
    participant Factory as SpecFactory
    participant Spec as Component Spec
    participant Widget as Material Widget
    participant Effects as Effects

    App->>Manager: setTheme(...) / applyTheme(...)
    Manager->>Theme: resolve color, typography, shape, elevation, motion tokens
    Widget->>Factory: request component spec
    Factory->>Theme: read resolved tokens
    Factory-->>Spec: build immutable spec
    Spec-->>Widget: resolved rendering contract
    Widget->>Effects: configure ripple, state layer, focus, shadow, transition
    Widget-->>App: render Qt Widget
```