# Examples

The examples are executable consumer documentation. Gallery, Dashboard and Theme Studio are the three primary showcase entry points.

## Component Gallery 2.0

`qtmaterial3_gallery` is the complete component workbench for the current 60-component public registry.

Use it for:

- global component/type/route search;
- family navigation and deep links;
- Light, Dark, Medium/High Contrast and Expressive previews;
- LTR/RTL and density switching;
- Default/Hover/Focus/Pressed/Selected/Disabled/Error previews where supported;
- live writable Qt properties;
- copyable C++ and Qt Designer `.ui` snippets.

Open a component directly:

```bash
qtmaterial3_gallery --route /buttons/filled
qtmaterial3_gallery /data/tree-view
```

## Dashboard showcase

`qtmaterial3_dashboard_demo` is the application-level showcase. It demonstrates responsive navigation, analytics, tables, profile/pricing flows, loading/empty/error/offline states, dialogs, banners, progress indicators and runtime theme controls in a production-style shell.

It also supports deterministic screenshot capture. See the [Dashboard showcase guide](dashboard-showcase.md).

Use the Dashboard when evaluating how the component library behaves as a composed desktop application rather than in isolation.

## Theme Studio

`qtmaterial3_theme_studio` is the theme-authoring showcase. Use it to inspect seed-driven palettes, light/dark themes, contrast, Expressive variants and live theme propagation while authoring a theme.

Together, Theme Studio + Gallery 2.0 provide the quickest workflow for authoring a theme and immediately validating it against the full component catalog.

## Theming workflows

`qtmaterial3_theming_workflows` demonstrates supported public theming API workflows without relying on private implementation details.

## Designer form

`qtmaterial3_designer_form_example` is a real AUTOUIC form built from `examples/designer-form/designerform.ui`. It demonstrates the same XML that Qt Designer writes: component properties are persisted in the form and runtime-only data such as table models is attached in C++.

See the [Qt Designer plugin guide](../designer-plugin.md) for plugin installation and the supported palette.

## Build examples

Examples are enabled by default when the repository is the top-level CMake project:

```bash
cmake -S . -B build -DQTMATERIAL3_BUILD_EXAMPLES=ON
cmake --build build
```

When consumed as a subproject, examples stay off unless explicitly enabled.
