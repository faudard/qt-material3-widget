# Examples

The examples are executable consumer documentation.

## Component Gallery

`qtmaterial3_gallery` is the primary widget catalog. Use it to inspect component states, keyboard behavior, light/dark themes, contrast and LTR/RTL behavior.

## Dashboard demo

`qtmaterial3_dashboard_demo` demonstrates application composition: responsive navigation, tables, profile/pricing flows, loading/empty/error/offline states, dialogs, banners, progress indicators and runtime theme controls.

It also supports deterministic screenshot capture. See the [Dashboard showcase guide](dashboard-showcase.md).

## Theme Studio

`qtmaterial3_theme_studio` is the interactive theme-authoring and inspection tool.

## Theming workflows

`qtmaterial3_theming_workflows` demonstrates supported public theming API workflows without relying on private implementation details.

## Build examples

Examples are enabled by default when the repository is the top-level CMake project:

```bash
cmake -S . -B build -DQTMATERIAL3_BUILD_EXAMPLES=ON
cmake --build build
```

When consumed as a subproject, examples stay off unless explicitly enabled.
