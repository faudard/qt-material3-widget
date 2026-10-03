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

- **Qt Material 3 - Buttons**: Filled, Outlined, Elevated and Text buttons
- **Qt Material 3 - Inputs**: Outlined and Filled text fields
- **Qt Material 3 - Selection**: Checkbox, Radio Button and Switch
- **Qt Material 3 - Navigation**: Tabs
- **Qt Material 3 - Surfaces**: Card
- **Qt Material 3 - Data**: Table

The generated `.ui` stores the real Qt Material 3 class and public include path, so applications keep using the normal `QtMaterial3::Widgets` runtime target. The Designer plugin is tooling only and is never a runtime dependency of consumer applications.

More widgets can be added once their Designer-specific editing semantics are defined. Complex containers should use proper Designer extensions rather than pretending to be ordinary widgets.
