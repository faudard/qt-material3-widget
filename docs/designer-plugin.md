# Qt Designer plugin

Qt Material 3 can expose its most common widgets directly in Qt Designer and Qt Creator's Form Editor.

## Build

The Designer plugin is optional because Qt's UiPlugin development component is not required by normal library consumers.

```bash
cmake -S . -B build -DQTMATERIAL3_BUILD_DESIGNER_PLUGIN=ON
cmake --build build --target qtmaterial3_designer_plugin
```

The plugin must be built with a Qt/toolchain ABI compatible with the Qt Designer or Qt Creator instance that loads it. This matters especially on Windows.

## Install

```bash
cmake --install build
```

By default the plugin is installed below
`${CMAKE_INSTALL_LIBDIR}/qt<major>/plugins/designer`.
Override `QTMATERIAL3_DESIGNER_PLUGIN_INSTALL_DIR` when the Designer installation uses another plugin directory.

For a Qt SDK installation, the effective destination is typically the `designer` directory below Qt's plugin directory. Qt Designer lists loaded custom widgets in **Help > About Plugins**; Qt Creator exposes equivalent Form Editor plugin diagnostics.

## Palette

The first supported palette is intentionally restricted to widgets that can be safely instantiated and edited without a custom task-menu or container extension:

- **Qt Material 3 - Buttons**: Filled, Filled Tonal, Outlined, Elevated and Text buttons
- **Qt Material 3 - Inputs**: Combo Box, Slider, Outlined and Filled text fields
- **Qt Material 3 - Selection**: Checkbox, Radio Button, Switch and Chip
- **Qt Material 3 - Navigation**: Tabs and Breadcrumb
- **Qt Material 3 - Surfaces**: Card
- **Qt Material 3 - Progress**: Linear and Circular progress indicators
- **Qt Material 3 - Data**: Pagination, Table and Tree View

The generated `.ui` stores the real Qt Material 3 class and public include path, so applications keep using the normal `QtMaterial3::Widgets` runtime target. The Designer plugin is tooling only and is never a runtime dependency of consumer applications.

More widgets can be added once their Designer-specific editing semantics are defined. Complex containers should use proper Designer extensions rather than pretending to be ordinary widgets.


## Production validation

The Designer integration is release-gated by two complementary tests:

- a collection contract that instantiates every advertised widget and validates its Designer metadata;
- a real `.ui` fixture processed by AUTOUIC, compiled and instantiated against the runtime Widgets library.

CI builds the plugin on Linux/Qt 6 and on Windows with the project's primary Qt 5.14.2/MSVC compatibility profile. This catches both metadata/runtime regressions and the ABI/toolchain combination used by the supported Qt 5 workflow.

### Container semantics

Only widgets with genuine container semantics are advertised as Designer containers. `QtMaterialTabs` derives from `QTabWidget` and therefore uses Designer's native tab-container editing behavior. `QtMaterialCard` is intentionally exposed as a regular widget: its runtime API does not define child-container ownership semantics, so advertising a synthetic container extension would make generated forms misleading.

Widgets that need task-menu or custom container behavior should be added only together with the corresponding Designer extension and automated contract coverage.
