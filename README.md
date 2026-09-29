# qt-material3-widget

Material 3 widgets for Qt Widgets, with a typed theme/spec architecture and support for Qt 5.14.2+ and Qt 6.4+.

> **Status:** 1.0.0 stable.
> Documented installed public headers follow the 1.x source-compatibility policy. Binary compatibility remains best-effort unless a stricter ABI policy is published.

## Requirements

- Qt 5.14.2+ or Qt 6.4+ ([support policy](docs/compatibility/qt5-qt6-policy.md))
- CMake 3.21+
- C++17

## Build

```bash
git clone https://github.com/faudard/qt-material3-widget.git
cd qt-material3-widget
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Material Color Utilities is optional and disabled by default. The built-in fallback backend remains available without an external MCU checkout.

## Examples

The repository includes complementary examples for both component-level inspection and real application composition:

- `qtmaterial3_gallery` — component gallery and interaction coverage.
- `qtmaterial3_dashboard_demo` — responsive analytics dashboard using cards, navigation rail, search, segmented controls, tables, chips, dialogs, snackbars, command palette, live theme switching, and theme-aware private chart widgets.
- `qtmaterial3_theme_studio` — interactive theme authoring and inspection.
- `qtmaterial3_theming_workflows` — public theming API workflows.

The dashboard intentionally keeps its line and donut charts private to the example. They demonstrate composing custom application visuals from Material 3 theme tokens without expanding the public widget API.

## Consume from CMake

```cmake
find_package(QtMaterial3Widgets REQUIRED)

target_link_libraries(my_app PRIVATE
    QtMaterial3::ThemeModel
    QtMaterial3::ThemeRuntime
    QtMaterial3::Widgets
)
```

The repository validates source, `add_subdirectory`, FetchContent and installed-package consumers in CI.

## Architecture

Widgets render from resolved specs rather than performing ad hoc theme lookups during paint/layout.

Supported CMake package components:

- `QtMaterial3::ThemeModel` — theme values, tokens and generation
- `QtMaterial3::ThemeIO` — theme serialization
- `QtMaterial3::ThemeRuntime` — runtime contexts and system integration
- `QtMaterial3::Integration` — optional Qt palette/theme integration helpers
- `QtMaterial3::Widgets` — public Material 3 widgets

Foundation, Core, Specs and Effects remain exported dependency targets required
by the installed target graph, but they are not supported `find_package(... COMPONENTS ...)`
entry points for applications.

## Documentation

- [Public API](docs/public-api/index.md)
- [Theming](docs/public-api/theming.md)
- [Architecture](docs/architecture/)
- [Roadmap](ROADMAP.md)
- [Release process](docs/release-process.md)
- [Changelog](CHANGELOG.md)

Component maturity is generated from `docs/components/component-registry.json` into `STATUS.md`.

## License

GNU Lesser General Public License v3.0 only (`LGPL-3.0-only`). See [LICENSE](LICENSE).
