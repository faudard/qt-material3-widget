# Qt Designer plugin

Qt Material 3 exposes a production-oriented custom-widget collection for Qt Designer and Qt Creator's Form Editor. The plugin is tooling only: generated applications link the normal `QtMaterial3::Widgets` runtime target and do not depend on the Designer module.

## Build

The plugin is optional because Qt's UiPlugin development component is not required by normal library consumers. The richer Designer 2.0 authoring extensions use Qt's `Designer` development module when it is available; builds that only provide `UiPlugin` keep the complete palette and .ui serialization support and simply omit the in-process task-menu/container helpers.

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

- **Qt Material 3 - Buttons**: Filled, Filled Tonal, Outlined, Elevated, Text, Split Button and Button Group
- **Qt Material 3 - Inputs**: Combo Box, Slider, Range Slider, Outlined Text Field, Filled Text Field, Date Field and Search Bar
- **Qt Material 3 - Selection**: Checkbox, Radio Button, Switch and Chip
- **Qt Material 3 - Navigation**: Tabs, Breadcrumb and Navigation Bar
- **Qt Material 3 - Surfaces**: Card, Top App Bar, Bottom App Bar and Divider
- **Qt Material 3 - Progress**: Linear, Circular and Expressive Loading Indicator
- **Qt Material 3 - Data**: Pagination, Table, Tree View and Segmented List

That is **32 advertised controls** for Designer 2.0.

Designer 3.0 promotes three additional persistence-safe controls for a total of **35**:

- **Qt Material 3 - Layouts**: Adaptive Shell
- **Qt Material 3 - Surfaces**: Tooltip
- **Qt Material 3 - Data**: Badge

Adaptive Shell exposes `automaticDensity` and `supportingPaneWidth` as authored Qt properties while retaining its two-slot container extension. Tooltip and Badge are ordinary property-driven controls, so all three survive Designer -> `.ui` -> UIC without application-side construction code.

The Designer 2.0 additions remain non-placeholder controls. Split Button and Loading Indicator expose authored state through ordinary Qt properties. Button Group, Navigation Bar and Segmented List expose persistence-safe `QStringList` properties for their authored labels.

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

The plugin's DOM metadata supplies useful initial values for newly dropped controls. For example, Range Slider starts with a visible 25–75 range, Date Field has an editable date format, buttons have visible labels, App Bars have an editable title, Button Group starts with Day/Week/Month, Navigation Bar with Home/Search/Profile, and Segmented List with Personal/Work/Archive. These are only authoring defaults; the saved `.ui` owns the resulting values.

### Designer 3.0 task menu and property editor

When Qt's `Designer` development API is present, every Qt Material 3 widget gets a task-menu entry named **Edit Material 3 properties...**. Designer 3.0 discovers writable/designable/stored properties declared by Material classes instead of relying on a fixed hand-maintained property-name allowlist.

The editor provides specialized controls for:

- enums;
- booleans, integers and floating-point values;
- strings, dates, times and date-times;
- colors;
- real collection editing for `QStringList` properties, including add/remove/reorder;
- resettable overrides, with **Token/default** delegating to Designer's property-reset path so theme-resolved color properties can return to their Material token.

Button Group, Navigation Bar and Segmented List therefore have visual collection editors rather than multiline text editing. In a form window, changes and resets are applied through Designer's form cursor so they participate in the normal undo/serialization workflow.

The same task menu retains the non-persistent Light/Dark/Expressive actions and adds **Designer 3.0 preview...**. The preview dialog renders the selected widget without serializing preview state and can combine:

- current, Compact (480), Medium (720) or Expanded (1024) logical width;
- DPR 1x or 2x;
- LTR or RTL;
- current theme or six built-in theme presets: Material Default Light/Dark, Blue Light, Green Light, Amber Dark and Rose Expressive.

For Adaptive Shell this makes breakpoint, automatic density, navigation mode and supporting-pane behavior inspectable from Designer without resizing the authored form. The target widget's size/direction and the process theme are restored after each capture.

### Persistence-safe collection properties

Three collection-style widgets now have explicit authored-list properties:

- `QtMaterialButtonGroup::buttonLabels`
- `QtMaterialNavigationBar::destinationLabels`
- `QtMaterialSegmentedList::itemLabels`

Designer serializes these as `<stringlist>` values. The runtime setters rebuild the corresponding item collections, so UIC-generated forms recreate the authored controls without custom post-load code.

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
- expected palette groups and the 35-widget inventory are checked;
- Material-declared persistence-safe properties, including color/token overrides and Adaptive Shell authoring properties, remain visible through `QMetaObject`;
- key Designer defaults are checked in the generated DOM;
- a real `.ui` fixture is processed by AUTOUIC, compiled and instantiated against the runtime Widgets library, including Adaptive Shell, Tooltip and Badge;
- CI builds the plugin on Linux/Qt 6 and Windows Qt 5.14.2/MSVC.

### Container semantics

Only widgets with genuine container semantics are treated as containers. `QtMaterialTabs` has an explicit `QDesignerContainerExtension` so add/insert/remove/current-page operations remain deterministic even though it subclasses `QTabWidget`. `QtMaterialAdaptiveShell` also has a two-slot container extension mapping Designer children to the shell's content and supporting panes; this extension is available to promoted/custom forms even though Adaptive Shell is not yet part of the default 32-control palette.

`QtMaterialCard` remains a regular widget because its runtime API does not define child-container ownership semantics.

The advanced task-menu and container extensions are registered once per Designer form-editor extension manager. If the Qt SDK does not ship the `Designer` development module, the plugin reports a palette-only fallback at configure time instead of making Qt 5.14.2/packaging builds fail.

## Conan and vcpkg

This release does not add an in-repository Conan recipe or vcpkg port. A recipe that silently hard-pins Qt 6 would misrepresent the library's certified Qt 5.14.2 + Qt 6 support, while official vcpkg/Conan registry submissions have their own review and versioning lifecycle.

The CMake install/export contract is the package-manager boundary for 1.6. A future package-manager PR should validate at least:

- shared and static builds;
- the supported Qt-major selection;
- installed `find_package(QtMaterial3Widgets)` consumption;
- optional Designer/UiPlugin dependencies;
- package-manager cache/relocation behavior.

This keeps package-manager support testable instead of shipping an unverified manifest.
