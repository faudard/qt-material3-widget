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

## Qt Designer tooling

The optional custom-widget plugin can be built and installed independently from normal application consumption:

```bash
cmake --preset designer-dev
cmake --build --preset designer-dev
cmake --install build/designer-dev --component Designer
```

The plugin is ABI-coupled to the Qt Designer/Qt Creator instance that loads it. See the [Designer plugin guide](designer-plugin.md).

## Conan 2

The repository ships a Conan 2 recipe that reuses the authoritative installed CMake package instead of inventing a parallel target graph. Qt remains an external ABI dependency so the recipe can preserve both the certified Qt 5.14.2 path and supported Qt 6 installations.

A supported Qt must already be discoverable by CMake in the build environment. Create a Qt 6 shared package with:

```bash
conan profile detect --force
conan create . --build=missing \
  -o "qt-material3-widget/*:qt_major=6" \
  -o "qt-material3-widget/*:shared=True"
```

For the certified Qt 5 family, expose the intended Qt 5 prefix to CMake and select `qt_major=5`. The recipe passes `QTMATERIAL3_EXPECT_QT_MAJOR` so accidentally resolving the wrong major fails closed.

The optional Designer plugin is enabled with:

```bash
conan create . --build=missing \
  -o "qt-material3-widget/*:qt_major=6" \
  -o "qt-material3-widget/*:with_designer=True"
```

The package exports the existing `QtMaterial3WidgetsConfig.cmake`; consumers continue to use `find_package(QtMaterial3Widgets CONFIG REQUIRED)` and `QtMaterial3::*` targets.

## vcpkg overlay port

A repository-local overlay port lives at `packaging/vcpkg/ports/qt-material3-widget`. Qt 6 is the default feature. The port also exposes `qt5`, `designer-qt6` and `designer-qt5` features and rejects mixed Qt-major graphs.

For Qt 6:

```bash
vcpkg install qt-material3-widget \
  --overlay-ports=packaging/vcpkg/ports
```

For Qt 5, disable the default Qt 6 feature in the consuming manifest and request the Qt 5 feature explicitly:

```json
{
  "dependencies": [
    {
      "name": "qt-material3-widget",
      "default-features": false,
      "features": ["qt5"]
    }
  ]
}
```

Add `designer-qt5` or `designer-qt6` only when the matching Qt-major feature is selected. The port supports both static and dynamic vcpkg triplets and fixes up the installed `QtMaterial3Widgets` CMake config for relocation.

Package-manager metadata is checked on every relevant pull request. Conan Qt 6 shared/static consumption is built in CI; the heavier full vcpkg build is available as an explicit package-manager workflow dispatch.

## Build options

Useful top-level options include `QTMATERIAL3_BUILD_TESTS`, `QTMATERIAL3_BUILD_EXAMPLES`, `QTMATERIAL3_BUILD_BENCHMARKS`, `QTMATERIAL3_BUILD_DESIGNER_PLUGIN`, `QTMATERIAL3_INSTALL`, and `QTMATERIAL3_USE_MCU`.

See the [Qt compatibility policy](compatibility/qt5-qt6-policy.md) before changing the supported Qt range.
