# Accessibility completion policy

This policy defines the evidence required before a 1.x component can claim complete accessibility maturity.

## Semantic contract

Every interactive component must expose the semantics that apply to it through Qt accessibility APIs and widget state:

- accessible name;
- accessible description when the name alone is insufficient;
- an appropriate accessible role;
- checked state for checkable controls;
- selected state for selectable items;
- expanded/collapsed state for disclosure and hierarchical controls;
- disabled and read-only state;
- value, minimum and maximum for value controls;
- current keyboard focus.

Icon-only controls must have a non-empty accessible-name strategy. A tooltip is useful visible
help, but is not a substitute for an accessible name.

## Keyboard contract

Components must preserve native Qt keyboard conventions and add explicit tests where the
Material wrapper changes interaction:

- deterministic tab order;
- Enter and Space activation where applicable;
- Escape dismissal for dismissible overlays;
- arrow navigation for composite controls;
- Home and End for ordered navigation;
- PageUp and PageDown for paged/range-oriented controls where applicable;
- focus containment for modal surfaces;
- focus restoration to the invoker after a modal surface closes.

The exact key set is capability-driven. A component is not required to implement keys that
have no semantic meaning for that component.

## Visual accessibility

High Contrast and Reduced Motion are release-level accessibility modes, not example-only
features. Components that animate must reach the same deterministic end state when Reduced
Motion is enabled. Focus indicators must remain visible in Light, Dark, and High Contrast
themes.

## Completion evidence

Accessibility maturity is evidence-based. Before raising a component to complete, its tests
must cover the relevant semantic state and keyboard contract above. Visual-only evidence does
not replace accessibility tests, and an accessibility score must not be raised solely because
the gallery supplies labels.

Family tests should be preferred over one-off widget tests when the behavior is shared. This
keeps the suite broad without duplicating identical contracts across every concrete class.
