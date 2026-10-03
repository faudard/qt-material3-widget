# Consumer component documentation standard

Every release-scope public component must have consumer documentation that is useful without reading implementation code.

## Required sections

A component entry must cover these headings or their clearly equivalent content:

### Screenshot

Point to a maintained Gallery route or a reviewed deterministic visual reference. Prefer generated/reproducible imagery over manually copied screenshots.

### When to use

Explain the user interaction the component represents and, where useful, when a sibling component is a better choice.

### API

Name the installed public header and public widget type. Document the small set of properties, signals and extension points application developers normally need. Exact signatures belong in the generated C++ API reference.

### States

Describe only states the component actually supports: enabled, disabled, hover, focus, pressed, selected, checked, indeterminate, expanded, read-only, error, loading, empty, or other component-specific states.

### Keyboard

Document activation, navigation, dismissal and focus keys exercised by the component. State when native Qt behavior remains authoritative.

### Accessibility

Describe naming requirements and meaningful role/state/value semantics. Icon-only actions must document their accessible-name requirement.

### RTL

State whether content order, navigation direction, start/end padding, arrows or geometry mirror in right-to-left layouts. If RTL is not applicable, say why.

### Example

Link the canonical Gallery route or focused executable example and include a minimal code sample when construction is not obvious.

## Source of truth

`docs/components/component-registry.json` owns component identity, family, public header, widget type, focused test target, Gallery route, docs path and maturity evidence.

Family guides may document several related widgets on one page. They do not need one physical Markdown file per class, but each release-scope component must be discoverable by name and have the required consumer information.

## Screenshots

Do not commit arbitrary screenshots just to satisfy the Screenshot section. Use this priority:

1. reviewed deterministic visual-regression reference;
2. reproducible Gallery/dashboard capture;
3. Gallery route when no stable reference image exists yet.

This keeps documentation imagery synchronized with the implementation and avoids stale screenshots.

## Review checklist

Before merging a new public component:

- it exists in the component registry;
- its installed header and public type are named in consumer docs;
- the docs explain when to use it;
- supported states are described;
- keyboard behavior is documented;
- accessibility requirements are documented;
- RTL behavior is documented or explicitly not applicable;
- a Gallery route or executable example is provided;
- exact API signatures remain delegated to generated reference documentation.

This documentation contract complements, rather than replaces, the component maturity gates.
