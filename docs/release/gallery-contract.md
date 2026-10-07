# Gallery 2.0 contract

The Gallery is both the public component explorer and the visual QA workbench for the library.

## Navigation and discovery

Gallery 2.0 must expose the canonical component registry directly:

- global search across component name, family, widget type and Gallery route;
- family/component navigation for every public registry entry;
- stable deep links such as `/buttons/filled`, accepted from the route field and the `--route` CLI option;
- automatic navigation to the page that actually contains the selected widget when possible.

The registry and Gallery catalog are checked together by the documentation contract so a public component cannot silently disappear from the explorer.

## Runtime preview controls

The top-level toolbar is the shared preview surface for:

- Light / Dark;
- Standard / Medium / High contrast;
- Standard / Expressive theme and motion scheme;
- LTR / RTL;
- Default / Compact / Comfortable density where the component exposes a writable density enum;
- seed color.

These controls are intentionally runtime-only and do not add new public widget API.

## State workbench

The inspector exposes a common state selector:

- Default
- Hover
- Focus
- Pressed
- Selected
- Disabled
- Error

The preview uses native widget state where it is meaningful. Components that do not expose a selected/error contract simply ignore the unsupported state; their documentation must describe the actual state set.

## Live properties and snippets

For the selected component the inspector provides:

- readable Qt properties;
- in-place editing for writable scalar/enum properties;
- copyable C++ construction snippets;
- copyable Qt Designer `.ui` custom-widget snippets;
- the canonical Gallery route.

The inspector is a consumer aid, not a replacement for Qt Designer or the generated C++ API reference.

## Documentation integration

`tools/generate_component_docs.py` generates one homogeneous page for every public registry component with these sections:

1. Screenshot / visual reference
2. When to use
3. API
4. States
5. Keyboard
6. Accessibility
7. RTL
8. Example

The documentation pipeline first prepares reviewed visual-regression goldens and then automatically attaches the best matching reviewed visual to generated component pages. If no matching reviewed golden exists, the Gallery deep link remains the maintained visual source.

## Showcase applications

The Gallery is the component-level workbench. The companion executables are application-level showcases:

- `qtmaterial3_dashboard_demo` demonstrates responsive composition, real page hierarchy, application states and production-style data surfaces;
- `qtmaterial3_theme_studio` demonstrates theme authoring, contrast/variant inspection and live theme propagation.

The examples guide should present all three as first-class entry points rather than treating the Dashboard and Theme Studio as secondary test fixtures.
