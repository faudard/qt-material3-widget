# Material 3 references

This page maps the most useful upstream Material 3 references to the corresponding areas of `qt-material3-widget`.

These links are **design-system references**, not the project API reference. Use them to understand Material semantics, token naming, and component behavior. Then use this repository’s own guides and generated C++ API pages to see how those ideas are implemented in Qt.

## Start here

- Material 3 get started: <https://m3.material.io/get-started>
- Material 3 styles overview: <https://m3.material.io/styles>
- Material 3 usability foundations: <https://m3.material.io/foundations/usability>
- Material customization: <https://m3.material.io/foundations/customization>

## Theme and design tokens

### Color

Use these pages when working with `ThemeOptions`, `Theme`, `ColorScheme`, and color-role mapping in widgets.

- Color overview: <https://m3.material.io/styles/color/overview>
- Color roles: <https://m3.material.io/styles/color/roles>
- How the color system works: <https://m3.material.io/styles/color/system/how-the-system-works>
- Choosing a color scheme: <https://m3.material.io/styles/color/choosing-a-scheme>
- Static and branded color guidance: <https://m3.material.io/styles/color/static>

### Typography

Use these pages when working with `TypographyScale`, `TypographyStyle`, and text hierarchy in components.

- Typography overview: <https://m3.material.io/styles/typography>
- Type scale tokens: <https://m3.material.io/styles/typography/type-scale-tokens>
- Applying type: <https://m3.material.io/styles/typography/applying-type>
- Fonts: <https://m3.material.io/styles/typography/fonts>

### Shape

Use these pages when working with `ShapeScale` and component corner-radius decisions.

- Shape overview: <https://m3.material.io/styles/shape>
- Corner radius scale: <https://m3.material.io/styles/shape/corner-radius-scale>

### Motion

Use these pages when working with `MotionTokens`, transitions, and interaction timing.

- Motion overview: <https://m3.material.io/styles/motion/overview/how-it-works>
- Motion specifications: <https://m3.material.io/styles/motion/overview/specs>
- Easing and duration: <https://m3.material.io/styles/motion/easing-and-duration>
- Transitions: <https://m3.material.io/styles/motion/transitions>

## Components

Use these pages to cross-check component family behavior when designing or reviewing widgets.

- Buttons overview: <https://m3.material.io/components/buttons/overview>
- Cards specs: <https://m3.material.io/components/cards/specs>

Add more component-specific links here as new widget families mature.

## How to use these references in this repository

A practical rule of thumb:

- use **Material 3 docs** for visual semantics, token meaning, and interaction expectations
- use **`docs/public-api/*`** for package-level guidance
- use **`docs/api/index.md`** for exact C++ classes and members

## Documentation policy suggestion

When a Qt API page claims that a class implements a Material 3 behavior, add one upstream Material 3 reference link near the top of the page. That keeps the project docs grounded in the design system without replacing the project’s own responsibility to document its public API.
