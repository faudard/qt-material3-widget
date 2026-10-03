# Installation

Qt Material 3 Widgets supports four consumer paths. CI validates source-tree, `add_subdirectory`, FetchContent and installed-package consumption.

## Requirements

- Qt 5.14.2+ or Qt 6.4+
- CMake 3.21+
- C++17

## add_subdirectory

Use this when the library source is already part of your workspace.

```cmake
add_subdirectory(external/qt-material3-widget)

target_link_libraries(my_app PRIVATE
    QtMaterial3::ThemeModel
    QtMaterial3::ThemeRuntime
    QtMaterial3::Widgets
)
```

When embedded as a subproject, tests and examples are not enabled by default.

## FetchContent

Pin a tag or commit in production rather than tracking `main`.

```cmake
include(FetchContent)

FetchContent_Declare(
    qt_material3_widget
    GIT_REPOSITORY https://github.com/faudard/qt-material3-widget.git
    GIT_TAG v1.0.0
)

FetchContent_MakeAvailable(qt_material3_widget)

target_link_libraries(my_app PRIVATE
    QtMaterial3::ThemeModel
    QtMaterial3::ThemeRuntime
    QtMaterial3::Widgets
)
```

## Installed package

Build and install the library once:

```bash
cmake -S qt-material3-widget -B build \
  -DCMAKE_INSTALL_PREFIX=/opt/qt-material3-widget
cmake --build build
cmake --install build
```

Consume it from another project:

```cmake
find_package(QtMaterial3Widgets REQUIRED)

target_link_libraries(my_app PRIVATE
    QtMaterial3::ThemeModel
    QtMaterial3::ThemeRuntime
    QtMaterial3::Widgets
)
```

If the prefix is not on CMake's default search path, add it to `CMAKE_PREFIX_PATH`.

## Supported package components

Application-facing components are:

- `QtMaterial3::ThemeModel`
- `QtMaterial3::ThemeIO`
- `QtMaterial3::ThemeRuntime`
- `QtMaterial3::Integration`
- `QtMaterial3::Widgets`

Foundation, Core, Specs and Effects are exported dependencies of the target graph, not application-level `find_package(... COMPONENTS ...)` entry points.

## Build options

Useful top-level options include `QTMATERIAL3_BUILD_TESTS`, `QTMATERIAL3_BUILD_EXAMPLES`, `QTMATERIAL3_BUILD_BENCHMARKS`, `QTMATERIAL3_INSTALL`, and `QTMATERIAL3_USE_MCU`.

See the [Qt compatibility policy](compatibility/qt5-qt6-policy.md) before changing the supported Qt range.
