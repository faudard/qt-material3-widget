# Plan

1. Add ThemeContextDefaults.
2. Remove ThemeManager from Core binding.
3. Make ThemeContext thread-affine and revision-authoritative.
4. Rebuild ThemeManager without IO/fingerprint/revision duplication.
5. Make ThemeObserver context-based.
6. Remove SystemTheme textual conversion functions.
7. Remove ThemeRuntime -> ThemeIO CMake edge.
8. Add C++ and architecture tests.
9. Converge reparent/context/system-theme tests.
