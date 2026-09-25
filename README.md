# qt-material3-widget

Material 3 widgets for Qt Widgets, with a typed theme/spec architecture and support for Qt 5.14.2 and Qt 6.

> **Status:** 0.8.0, pre-1.0 API cleanup.
> Public APIs may still change before 1.0. Pre-1.0 compatibility shims are intentionally removed rather than deprecated.

## Requirements

- Qt 5.14.2+ or Qt 6.x
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
