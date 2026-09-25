# ThemeRuntime

Widget resolution order is:

`explicit ThemeContext -> nearest ancestor ThemeContextHost -> private default context provider`.

Core uses a private default provider and does not depend on ThemeManager directly.

ThemeContext is the sole revision authority: an equal Theme is a no-op; a successful
different Theme increments revision exactly once and emits one change sequence.

ThemeManager delegates revision to its default ThemeContext and owns no serializer,
JSON API, fingerprint, or second revision counter.

ThemeObserver observes ThemeContext. Null explicit context means the default context.

ThemeContext and ThemeManager mutations are owner-thread-only.

SystemTheme is optional. It may apply platform appearance to ThemeManager only when
auto-apply is explicitly enabled. Text conversion belongs to ThemeIO.

ThemeRuntime links ThemeModel and never ThemeIO.
