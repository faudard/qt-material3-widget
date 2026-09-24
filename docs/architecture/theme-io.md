# ThemeIO ownership

ThemeIO owns conversion between typed theme semantics and the persisted Theme
JSON contract.

```text
ThemeModel typed values
        ⇅
ThemeSerializer + private textual codecs
        ⇅
canonical Theme JSON
```

## Component override identifiers

ThemeModel and Specs use typed `ComponentId` values. ThemeIO owns conversion to
canonical first-party JSON identifiers and preserves unknown third-party
component names as opaque extension data.

Applications should use `ThemeSerializer` rather than depending on the
serializer's internal text-key mapping helpers.

## Runtime

ThemeRuntime does not own persistence. Applications explicitly compose IO and
runtime operations:

```cpp
const QByteArray json = QtMaterial::ThemeSerializer::toJson(manager.theme());

bool ok = false;
QString error;
const QtMaterial::Theme parsed = QtMaterial::ThemeSerializer::fromJson(
    json, QtMaterial::ThemeReadMode::Strict, &ok, &error);

if (ok) {
    manager.setTheme(parsed, QtMaterial::ThemeChangeReason::External);
}
```

This keeps serialization isolated from runtime theme propagation.
