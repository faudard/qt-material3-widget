# Material Visual UI Studio — 1.26 foundation

**Optional developer tool, not a released SDK feature.** Configure with QTMATERIAL3_BUILD_VISUAL_STUDIO=ON and build target qtmaterial3_visual_studio. Normal library consumers have no Studio dependency. With QTMATERIAL3_BUILD_TESTS=ON, run CTest test studio_document_contract.

The palette is computed from the intersection of the actual embedded docs/components/component-registry.json entries (matching id and widgetType) and the explicit exportable component whitelist. Supported: Filled, Outlined, Filled Tonal Button, Checkbox, Radio, Outlined Text Field, Card. The other registry components are deliberately not advertised as exportable.

## Editor

- Drag or double-click a palette entry to add it. Select on canvas or in widget tree.
- Edit label/text and bounds in the inspector. Drag to move; resize with the blue lower-right handle. Geometry snaps to 8px. Ctrl+wheel zooms. Undo/Redo uses document snapshots.
- Theme/density/direction choices persist; Compact/Medium/Expanded viewport preview **clips** content and does not perform responsive reflow. 1x/2x renders are simulated using actual offscreen QtMaterial widgets. Pixel equality and exact theme rendering are not claimed.
- Use Templates menu for dashboard, settings or form. The templates exercise the same validation pipeline.

## Schema and exports

The current strict UTF-8 JSON schemaVersion=1 contains name, canvas {width,height}, appearance {theme,density,direction}, and elements. Each element has a stable unique id, whitelisted component, bounded rect and properties {text}. Unknown fields, invalid types, off-canvas bounds, missing required fields, duplicate ids, and unsupported versions fail closed. Documents are saved atomically; no business logic, actions, models or bindings are owned or serialized by Studio.

File menu exports a .ui file for the approved flat widget subset, or a standalone main.cpp + CMakeLists.txt using the installed QtMaterial3Widgets package. Both validate the document before writing. .ui export depends on the corresponding Designer plugin/headers for Qt Designer. C++ export uses the native QtMaterial widgets. Neither exporter promises pixel parity or runtime signal wiring.

CLI commands for automation:

    qtmaterial3_visual_studio --check studio/fixtures/form.json
    qtmaterial3_visual_studio --export-ui studio/fixtures/form.json output.ui
    qtmaterial3_visual_studio --export-cpp studio/fixtures/form.json output-directory

## Explicitly outstanding 1.26 gates

This is a functional *vertical slice*, not full 1.26 certification. Still required: layout hierarchy and layout inspector, real adaptive reflow and guides, pan UX, structured token references and named styles, reusable compositions, safe migration tooling for schema v2+, authenticated Designer host save/reopen, .ui re-import, generated-output compile/run matrix (Qt 5/6, Windows/macOS/Linux), deterministic visual parity reports, and completed audit evidence. Existing schemas do not silently accept planned future fields. Keep the 1.26 milestone open until these gates are independently verified.
