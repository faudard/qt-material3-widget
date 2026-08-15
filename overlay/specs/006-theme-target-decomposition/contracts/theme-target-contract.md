# Theme Physical Layer Contract v1

```text
Foundation
   ↓
ThemeModel ← Specs
   ↑
ThemeIO
   ↑
ThemeRuntime   (temporary Runtime -> IO edge)
```

Public CMake targets:
- `QtMaterial3::ThemeModel`
- `QtMaterial3::ThemeIO`
- `QtMaterial3::ThemeRuntime`
- `QtMaterial3::Theme` (INTERFACE compatibility aggregate)

Specs may consume only ThemeModel.

Target-specific DLL export macros are mandatory.
