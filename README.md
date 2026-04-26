# qt-material3-widget

Material 3 widget toolkit for Qt Widgets.

> **Status:** experimental / design-in-progress.
> The repository already exposes the intended module layout, public API families,
> and consumer examples, but several components are still under active
> implementation and API stabilization.

## Why this project exists

`qt-material3-widget` brings Material 3 concepts to the Qt Widgets stack with a layered, typed architecture.

Core rule:

> Widgets render from resolved specs, not from ad hoc theme lookups.

That design makes widget behavior more predictable, easier to test, and easier to extend consistently across component families.

## Relationship to Material 3

This project is guided by the official Material 3 design system, but it is a Qt Widgets library, not an Android or Jetpack Compose port.

That means:

- component behavior should align with Material 3 intent where practical
- theming should use Material 3 concepts such as color roles, typography, shape, and motion
- public C++ APIs should remain idiomatic for Qt Widgets
- some interactions and rendering details require Qt-specific adaptation

## Material 3 references

For upstream design guidance, see:

- [Material 3 overview](https://m3.material.io/)
- [Get started with Material 3](https://m3.material.io/get-started)
- [Styles overview](https://m3.material.io/styles)
- [Color overview](https://m3.material.io/styles/color/overview)
- [Color roles](https://m3.material.io/styles/color/roles)
- [Typography](https://m3.material.io/styles/typography)
- [Type scale tokens](https://m3.material.io/styles/typography/type-scale-tokens)
- [Shape](https://m3.material.io/styles/shape)
- [Motion](https://m3.material.io/styles/motion/overview/how-it-works)
- [Usability foundations](https://m3.material.io/foundations/usability)

These pages describe Material 3 design intent.

The generated API reference in this repository documents the actual C++ API exposed by `qt-material3-widget`.

## Module layout

- `qtmaterial3_theme` — tokens, schemes, builder, serializer, manager
- `qtmaterial3_core` — widget bases and interaction state
- `qtmaterial3_specs` — immutable component specs and factory
- `qtmaterial3_effects` — ripple, focus, shadow, transitions, scrim
- `qtmaterial3_widgets` — public widget implementations

## Current direction

### Focus areas

- packaging and install/export
- accessibility and keyboard correctness
- performance and memory review
- public API stabilization
- documentation and release readiness

### Advanced families planned in the current tree

- advanced inputs (`DatePicker`, autocomplete maturity)
- navigation (`NavigationRail`, `Menu`)
- compact controls (`Chip`, `SegmentedButton`)
- data widgets (`Table`, `GridList`, `Carousel`)

## Project maturity

This repository currently contains both working building blocks and design scaffolding.
Some files are intentionally skeletal so the public architecture and module boundaries remain clear while implementation is still progressing.

## Requirements

- Qt >5.14.x or Qt 6.x
- CMake 3.21+
- C++ toolchain compatible with the selected Qt release

## Build

```bash
git clone https://github.com/faudard/qt-material3-widget.git
cd qt-material3-widget
cmake -S . -B build
cmake --build build
```

## Consume from CMake

```cmake
find_package(QtMaterial3Widgets REQUIRED)

target_link_libraries(my_app PRIVATE
    QtMaterial3::Theme
    QtMaterial3::Widgets
)
```

A minimal consumer example already exists in `examples/minimal-consumer`.

## Theming highlights

The theme layer is now useful as a standalone consumer surface, not only as an internal widget dependency.

Main theming capabilities:

- deterministic theme generation from `ThemeOptions`
- light and dark theme generation from a seed color
- direct color-scheme generation from a seed color
- runtime theme application through `ThemeManager`
- JSON import/export for theme persistence and tooling workflows

Typical consumer scenarios:

- ship a default Material 3 theme generated from a single brand seed color
- let users customize and persist the active theme as JSON
- export a resolved theme snapshot for debugging or design review
- build a theme editor or playground around `ThemeManager`

See the dedicated guide: [docs/public-api/theming.md](docs/public-api/theming.md)

## Examples

- `examples/gallery` — showcase / component gallery
- `examples/minimal-consumer` — smallest integration example
- `examples/install-consumer` — install/export validation
- `examples/theme-playground` — theme experimentation, seed color workflow, JSON import/export
- `examples/core-playground` — low-level behavior validation

## Documentation map

This repository documentation should cover:

- getting started
- build and installation
- theming guide
- widget-family guides
- architecture overview
- migration from `qt-material-widgets`
- generated C++ API reference
- Material 3 reference mapping

Start here for public usage guidance:

- [Public API guide](docs/public-api/index.md)
- [Theming guide](docs/public-api/theming.md)

## Public API families

- Buttons
- Inputs
- Navigation
- Selection
- Surfaces
- Compact controls
- Data widgets
- Theming

## Contributing

Contributions are especially useful in:

- widget implementation
- accessibility
- documentation
- tests and examples
- API review and cleanup

## License

Add the project license here.
