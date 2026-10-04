# Qt Designer plugin

Qt Material 3 exposes a production-oriented custom-widget collection for Qt Designer and Qt Creator's Form Editor. The plugin is tooling only: generated applications link the normal `QtMaterial3::Widgets` runtime target and do not depend on the Designer module.

## Build

The plugin is optional because Qt's UiPlugin development component is not required by normal library consumers.

```bash
cmake -S . -B build -DQTMATERIAL3_BUILD_DESIGNER_PLUGIN=ON
cmake --build build --target qtmaterial3_designer_plugin
```

For repository development, use the dedicated preset:

```bash
cmake --preset designer-dev
cmake --build --preset designer-dev
ctest --preset designer-dev
```

The plugin must be built with a Qt/toolchain ABI compatible with the Qt Designer or Qt Creator instance that loads it. This matters especially on Windows and for side-by-side Qt installations.

## Install and package

A normal install places the plugin below `${CMAKE_INSTALL_LIBDIR}/qt<major>/plugins/designer` unless `QTMATERIAL3_DESIGNER_PLUGIN_INSTALL_DIR` is overridden.

```bash
cmake --install build
```

The Designer payload is also an explicit CMake install component:

```bash
cmake --install build --component Designer
```

The component contains the plugin plus this integration guide. It assumes the matching Qt Material 3 runtime libraries have already been installed into the same prefix.

For a Qt SDK installation, the effective destination is typically the `designer` directory below Qt's plugin directory. Qt Designer lists loaded custom widgets in **Help > About Plugins**; Qt Creator exposes equivalent Form Editor plugin diagnostics.

## Designer 2.0 palette

The palette intentionally contains only widgets whose useful authored state can be represented safely by Qt properties or native Designer container semantics.

- **Qt Material 3 - Buttons**: Filled, Filled Tonal, Outlined, Elevated and Text buttons
- **Qt Material 3 - Inputs**: Combo Box, Slider, Range Slider, Outlined Text Field, Filled Text Field, Date Field and Search Bar
- **Qt Material 3 - Selection**: Checkbox, Radio Button, Switch and Chip
- **Qt Material 3 - Navigation**: Tabs and Breadcrumb
- **Qt Material 3 - Surfaces**: Card, Top App Bar, Bottom App Bar and Divider
- **Qt Material 3 - Progress**: Linear and Circular progress indicators
- **Qt Material 3 - Data**: Pagination, Table and Tree View

That is 27 advertised controls.

Controls whose meaningful content requires a list editor, model editor, task menu or custom container extension are not advertised merely to increase the palette count. They should be added together with a persistence-safe Designer extension.

## Property authoring

Designer uses Qt's meta-object system, so public `Q_PROPERTY` declarations are the source of truth.

Outlined and Filled Text Fields expose the common authoring surface directly in the Property Editor:

- `text`
- `placeholderText`
- `prefixText` / `suffixText`
- `clearButtonEnabled`
- `echoMode`
- `readOnly`
- `maxLength`
- `characterCounterEnabled`

They also inherit the form-field properties such as `label`, `helperText`, `errorText` and `required`.

The plugin's DOM metadata supplies useful initial values for newly dropped controls. For example, Range Slider starts with a visible 25–75 range, Date Field has an editable date format, buttons have visible labels, and App Bars have an editable title. These are only authoring defaults; the saved `.ui` owns the resulting values.

## Real .ui workflow

`examples/designer-form/designerform.ui` is a consumer-style form authored as XML and compiled by AUTOUIC. It exercises fields, selection controls, surfaces, Model/View data, progress and actions.

Build it with normal examples:

```bash
cmake -S . -B build -DQTMATERIAL3_BUILD_EXAMPLES=ON
cmake --build build --target qtmaterial3_designer_form_example
```

The separate `designer/designer_smoke.ui` fixture is smaller and exists specifically as a CI contract. Its executable validates that authored properties survive the UIC-generated code path.

## Production validation

Designer integration is release-gated by complementary contracts:

- collection metadata is unique and every advertised widget can be instantiated;
- expected palette groups and the 27-widget inventory are checked;
- Text Field authoring properties must remain visible through `QMetaObject`;
- key Designer defaults are checked in the generated DOM;
- a real `.ui` fixture is processed by AUTOUIC, compiled and instantiated against the runtime Widgets library;
- CI builds the plugin on Linux/Qt 6 and Windows Qt 5.14.2/MSVC.

### Container semantics

Only widgets with genuine container semantics are advertised as Designer containers. `QtMaterialTabs` derives from `QTabWidget` and uses Designer's native tab-container editing behavior. `QtMaterialCard` remains a regular widget because its runtime API does not define child-container ownership semantics.

Components such as segmented controls, navigation rails and data collections have application-owned item APIs. They are deliberately excluded until a task-menu/model extension can serialize their authored items correctly.

## Conan and vcpkg

This release does not add an in-repository Conan recipe or vcpkg port. A recipe that silently hard-pins Qt 6 would misrepresent the library's certified Qt 5.14.2 + Qt 6 support, while official vcpkg/Conan registry submissions have their own review and versioning lifecycle.

The CMake install/export contract is the package-manager boundary for 1.6. A future package-manager PR should validate at least:

- shared and static builds;
- the supported Qt-major selection;
- installed `find_package(QtMaterial3Widgets)` consumption;
- optional Designer/UiPlugin dependencies;
- package-manager cache/relocation behavior.

This keeps package-manager support testable instead of shipping an unverified manifest.
