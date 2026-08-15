# Theme Target Graph Contract
```text
qtmaterial3_foundation
        |
        v
qtmaterial3_theme_model
   |                |
   v                v
qtmaterial3_theme_io qtmaterial3_theme_runtime

qtmaterial3_theme_model -> qtmaterial3_specs
```
Forbidden: ThemeModel -> IO/Runtime/Core/Specs/Widgets/QtWidgets; Specs -> ThemeIO/Runtime/Core/Widgets/QtWidgets.
