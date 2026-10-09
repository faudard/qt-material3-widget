# qt-material3-widget

Material 3 widget toolkit for Qt Widgets.

This documentation is split into two complementary parts:

- **narrative guides** for users and contributors
- **generated C++ API reference** for the public headers

Use the narrative pages to understand concepts, layering, and supported workflows. Use the API reference when you need exact class and member names.

## Start here

### For application developers

- [Getting started in 5 minutes](getting-started.md)
- [Installation](installation.md)
- [Themes](themes.md)
- [Python / PySide6 bindings](python-bindings.md)
- [Widgets](widgets/index.md)
- [Reviewed visual references](visual-reference.md)
- [Examples](examples/index.md)
- [Migration](migration/index.md)
- [Documentation versions](versioning.md)
- [Public API guide](public-api/index.md)
- [C++ API reference](api/index.md)

### For contributors

- [Architecture overview](architecture/01-overview.md)
- [Development rules](development/coding-rules.md)
- [Accessibility and keyboard rules](development/accessibility-keyboard-rules.md)
- [Release checklist](release-process.md)
- [Final 60/60 certification](components/final-certification-60-60.md)
- [API and ABI policy](release/api-abi-policy.md)

## Installation and downstream usage

The project exports a CMake package named `QtMaterial3Widgets`.

```cmake
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)
find_package(QtMaterial3Widgets REQUIRED)

add_executable(my-app main.cpp)
target_link_libraries(my-app
  PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    QtMaterial3::ThemeModel
    QtMaterial3::ThemeRuntime
    QtMaterial3::Widgets
)
```

## Documentation structure

```{toctree}
:maxdepth: 2
:caption: User guide

getting-started
installation
themes
python-bindings
widgets/index
widgets/components/index
visual-reference
examples/index
migration/index
versioning
public-api/index
api/index
material3/references
release-process
components/final-certification-60-60
components/final-certification-55-55
release/api-abi-policy
```

```{toctree}
:maxdepth: 2
:caption: Architecture

architecture/01-overview
architecture/02-theme-layer
architecture/03-core-layer
architecture/04-spec-layer
architecture/05-effects-layer
architecture/06-widgets-layer
architecture/07-advanced-components
```


```{toctree}
:maxdepth: 2
:caption: Development

development/coding-rules
development/accessibility-keyboard-rules
development/advanced-component-rules
development/rendering-rules
development/testing-rules
```


## Material 3 note

This project follows Material 3 guidance, but the upstream Material 3 site remains the source for design-system semantics and visual rationale. The pages in this repository describe how those ideas are mapped into the Qt Widgets implementation.
