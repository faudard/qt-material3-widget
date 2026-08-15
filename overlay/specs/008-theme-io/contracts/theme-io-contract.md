# ThemeIO Contract v1

Typed model:
`ComponentId`, token/value enums and Theme values.

Text boundary:
`ThemeTextCodec`, `ThemeSerializer`, `XmlThemeAdapter`.

Forbidden outside ThemeIO:
- componentIdToString/componentIdFromString;
- QString-key ComponentTokenOverrides methods;
- resolver component alias QStringLists;
- ThemeManager JSON/file persistence API.

Unknown third-party serialized component names may be stored opaquely but only ThemeIO may
interpret or enumerate their textual identifiers.
