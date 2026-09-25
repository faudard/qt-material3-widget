# Material 3 upstream references

Use the upstream Material 3 documentation as the design-system reference when
reviewing QtMaterial3 behavior. This repository documents the Qt Widgets API and
its platform adaptations; it does not maintain a second, partially implemented
Material reference model.

## Foundations

- Color system: <https://m3.material.io/styles/color/overview>
- Typography: <https://m3.material.io/styles/typography/overview>
- Shape: <https://m3.material.io/styles/shape/overview>
- Motion: <https://m3.material.io/styles/motion/overview/how-it-works>
- Accessibility: <https://m3.material.io/foundations/accessible-design/overview>

## Components

Use the corresponding upstream component page when changing a widget family.
Examples:

- Buttons: <https://m3.material.io/components/buttons/overview>
- Cards: <https://m3.material.io/components/cards/overview>
- Dialogs: <https://m3.material.io/components/dialogs/overview>
- Navigation: <https://m3.material.io/components/navigation-bar/overview>

## Repository guidance

- use upstream Material 3 docs for visual semantics and interaction intent;
- use `docs/public-api/` for supported QtMaterial3 usage;
- use `docs/api/index.md` for exact C++ signatures;
- use the component registry and executable tests for repository maturity evidence.
