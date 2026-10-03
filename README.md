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

### Qt Designer

Build the optional custom-widget collection to drag Qt Material 3 controls directly into `.ui` forms:

```bash
cmake -S . -B build -DQTMATERIAL3_BUILD_DESIGNER_PLUGIN=ON
cmake --build build --target qtmaterial3_designer_plugin
```

The palette is grouped into Buttons, Inputs, Selection, Navigation, Surfaces and Data. See [Qt Designer plugin](docs/designer-plugin.md) for installation and ABI/toolchain requirements.

## Examples

The repository includes complementary examples for both component-level inspection and real application composition:

- `qtmaterial3_gallery` — component gallery and interaction coverage.
- `qtmaterial3_dashboard_demo` — responsive multi-page application showcase using sidebar/rail/drawer navigation, analytics, orders, profile, pricing, application states, live theme/contrast/RTL controls, command palette, dialogs, banners, progress indicators, tables, chips and theme-aware private chart widgets.
- `qtmaterial3_theme_studio` — interactive theme authoring and inspection.
- `qtmaterial3_theming_workflows` — public theming API workflows.

The dashboard intentionally keeps its line and donut charts private to the example. They demonstrate composing custom application visuals from Material 3 theme tokens without expanding the public widget API.

### Dashboard showcase

The dashboard is intended to be exercised as a small desktop product rather than a static screenshot. It includes responsive navigation, real table filtering/pagination, profile and pricing workflows, loading/empty/error/offline states, and a **Showcase Settings** page for changing seed color, light/dark mode, contrast, Tonal/Expressive variants and LTR/RTL direction.

For reproducible documentation captures, the demo also supports command-line page selection and PNG output:

```bash
# Light dashboard
./qtmaterial3_dashboard_demo --page dashboard --size 1440x920 --screenshot dashboard-light.png

# Dark dashboard
./qtmaterial3_dashboard_demo --dark --page dashboard --size 1440x920 --screenshot dashboard-dark.png

# RTL profile example
./qtmaterial3_dashboard_demo --rtl --page profile --size 1200x800 --screenshot profile-rtl.png
```

See [Dashboard showcase guide](docs/examples/dashboard-showcase.md) for the page map and capture options.

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

- [Getting started in 5 minutes](docs/getting-started.md)
- [Installation](docs/installation.md)
- [Themes](docs/themes.md)
- [Widgets](docs/widgets/index.md)
- [Complete component reference](docs/widgets/component-reference.md)
- [Examples](docs/examples/index.md)
- [Migration](docs/migration/index.md)
- [Public API](docs/public-api/index.md)
- [Theming API](docs/public-api/theming.md)
- [Architecture](docs/architecture/)
- [Roadmap](ROADMAP.md)
- [Release process](docs/release-process.md)
- [Changelog](CHANGELOG.md)

Component maturity is generated from `docs/components/component-registry.json` into `STATUS.md`.

## License

GNU Lesser General Public License v3.0 only (`LGPL-3.0-only`). See [LICENSE](LICENSE).
