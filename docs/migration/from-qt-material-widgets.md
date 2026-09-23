# Migrating from qt-material-widgets

`qt-material3-widget` is not a drop-in rename of the older Qt Material Widgets
APIs. The Material 3 library uses a layered architecture built around typed
theme data, resolved component specs, shared interaction infrastructure, and
public widgets.

## Recommended migration path

1. Add `QtMaterial3::ThemeModel`, `QtMaterial3::ThemeRuntime`, and `QtMaterial3::Widgets` to the application. Add `QtMaterial3::ThemeIO` only when serialization/import-export is needed.
2. Migrate one widget family at a time instead of replacing the whole UI at once.
3. Move ad-hoc colors, typography, shape, and motion values into the Material 3
   theme/token layer.
4. Prefer the Material 3 widget's public properties and resolved specs over
   subclass-specific painting overrides.
5. Validate keyboard behavior, accessibility names, RTL layout, and HiDPI
   rendering while migrating each screen.
6. Remove the legacy widget dependency only after all migrated screens build and
   pass the application's regression tests.

## API compatibility

Class names and properties may differ from the legacy library. Treat the
generated C++ API reference as the authority for the current public surface.

For new code, avoid compatibility shims that expose legacy theme lookups inside
Material 3 widgets. Widgets should continue to render from resolved specs.

## Mixed applications

A staged migration can keep legacy and Material 3 widgets in the same
application. Keep their theme plumbing separate and migrate shared application
styles toward Material 3 tokens incrementally.
