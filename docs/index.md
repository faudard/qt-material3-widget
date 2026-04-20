# qt-material3-widget

Material 3 widget toolkit for Qt Widgets.

This documentation is split into two complementary parts:

- **narrative guides** for users and contributors
- **generated C++ API reference** for the public headers

Use the narrative pages to understand concepts, layering, and supported workflows. Use the API reference when you need exact class and member names.

## Start here

### For application developers

- [Public API guide](public-api/index.md)
- [C++ API reference](api/index.md)
- [Material 3 upstream references](material3/references.md)
- [Migration from Qt Material Widgets](migration/from-qt-material-widgets.md)

### For contributors

- [Architecture overview](architecture/01-overview.md)
- [Development rules](development/coding-rules.md)
- [Accessibility and keyboard rules](development/accessibility-keyboard-rules.md)
- [Release process](development/release-process.md)

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
    QtMaterial3::Theme
    QtMaterial3::Widgets
)
```

## Documentation structure

```{toctree}
:maxdepth: 2
:caption: User guide

public-api/index
api/index
material3/references
migration/from-qt-material-widgets
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
development/build-reconciliation-checklist
development/compile-fix-reconciliation-pass
development/release-process
```

## Material 3 note

This project follows Material 3 guidance, but the upstream Material 3 site remains the source for design-system semantics and visual rationale. The pages in this repository describe how those ideas are mapped into the Qt Widgets implementation.
