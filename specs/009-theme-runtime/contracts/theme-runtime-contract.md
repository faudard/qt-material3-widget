# ThemeRuntime Contract v1

Resolution: `explicit -> inherited ThemeContextHost -> ThemeContextDefaults`.

ThemeContext owns revision.
ThemeManager owns the default context and ThemeBuilder, not persistence.
ThemeObserver observes ThemeContext.
SystemTheme is optional and auto-apply is opt-in.
ThemeRuntime links ThemeModel, never ThemeIO.
