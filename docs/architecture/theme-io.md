# ThemeIO Ownership

ThemeIO owns every conversion between typed theme semantics and persisted text.

```text
ThemeModel typed values
        ⇅
ThemeTextCodec / ThemeSerializer / XML adapter
        ⇅
JSON / XML / textual aliases
```

## Component override identifiers

ThemeModel/Specs use `ComponentId`. First-party JSON identifiers are accepted only in canonical form by
`ThemeTextCodec`; serialization emits that same stable name per typed ID.

Unknown third-party component names are preserved as an opaque extension map inside
`ComponentTokenOverrides`; only ThemeIO may enumerate/read/write those textual names.

## Runtime

ThemeManager is no longer a persistence façade. Application/tooling code composes:

```cpp
const QByteArray json = ThemeSerializer::toJson(manager.theme());
Theme parsed = ThemeSerializer::fromJson(json, ...);
manager.setTheme(parsed, ThemeChangeReason::ImportJson);
```

This removes serialization responsibilities from ThemeRuntime.

## Migration rule

The architecture baseline for `ARCH-THEME-IO-OWNS-TEXT-KEYS` is removed by Spec 008.
Any remaining occurrence becomes a new CI error rather than accepted debt.
