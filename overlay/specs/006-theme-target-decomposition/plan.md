# Implementation Plan: Theme Target Decomposition

## Source ownership

### ThemeModel
`colorscheme`, `themeoptions`, `theme`, `themebuilder`, `colorbackend`,
`mcuadapter`, `themevalidation`, typography/shape/elevation/motion/state/density/
icon/component/accessibility tokens.

### ThemeIO
`themeserializer`, `xmlthemeadapter`.

### ThemeRuntime
`themecontext`, `themecontexthost`, `thememanager`, `themeobserver`, `systemtheme`.

## Shared-library ABI mechanics

The old single `QTMATERIAL3_THEME_EXPORT` macro is insufficient once code lives in
three DLLs. Each physical library therefore gets:
- `QTMATERIAL3_THEME_MODEL_EXPORT`;
- `QTMATERIAL3_THEME_IO_EXPORT`;
- `QTMATERIAL3_THEME_RUNTIME_EXPORT`.

The old macro remains temporarily defined for source compatibility but is no longer
used by classified Theme headers.

## CMake compatibility

`QtMaterial3::Theme` becomes an INTERFACE aggregate. Existing consumers can continue
linking it while new internal boundaries use narrower targets.

## Transitional debt

Runtime still publicly depends on IO because public ThemeManager signatures mention
`ThemeReadMode`/JSON and its implementation serializes themes. This is recorded, not hidden.
Spec 009 must remove the edge.

## Validation order

1. Python decomposition checker.
2. Configure shared.
3. Build shared.
4. Tests.
5. Install + consumer.
6. Configure/build static.
7. Exact Qt5 gate.
8. Qt6 matrix.
