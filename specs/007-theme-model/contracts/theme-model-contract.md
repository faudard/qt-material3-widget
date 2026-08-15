# ThemeModel Contract v1

`Theme` is a regular C++17 value model.

Structural identity:

```text
Theme == Theme
```

must cover every semantically exposed Theme value and must not depend on:
- serializer output;
- JSON/XML;
- QHash iteration order;
- QObject identity;
- ThemeManager/runtime state.

ThemeModel dependencies stop at QtCore/QtGui value types and Foundation.

ThemeContext uses Theme equality, not an IO fingerprint.
