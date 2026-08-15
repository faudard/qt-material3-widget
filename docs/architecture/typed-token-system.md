# Typed Token Identity

QtMaterial3 has two levels of typed identity.

## Domain roles

Rendering/spec code continues to use the strongest domain type:

```cpp
ColorRole::Primary
TypeRole::BodyMedium
ShapeRole::Large
MotionToken::Short2
```

These APIs remain preferable to a universal lookup.

## Universal `TokenId`

`TokenId` is a 32-bit stable cross-category identity for tooling, provenance,
canonical interchange, cache keys and future compiler/IR work.

Encoding v1:

```text
31             24 23                              0
+----------------+--------------------------------+
| TokenCategory  |  category-local stable id      |
+----------------+--------------------------------+
```

Examples:

```text
Color/Primary          0x01000001
Typography/BodyMedium  0x0200000B
Shape/Full             0x03000007
Motion/Long4           0x0500000C
Density/Default        0x07000002
IconSize/Medium        0x08000003
```

The numeric identity is independent of the declaration order of the C++ enum.

## Why not replace domain enums with TokenId?

A universal API such as:

```cpp
QVariant tokenValue(TokenId);
```

would erase useful compile-time information and encourage dynamic lookup in paint/spec hot
paths. It is explicitly not the runtime design.

Use:

```cpp
theme.colorScheme().color(ColorRole::Primary);
```

not:

```cpp
theme.tokenValue(TokenId(...)).value<QColor>();
```

## Text ownership

ThemeModel contains numeric mappings only. It contains no persistent token names.

ThemeIO owns:

```cpp
ThemeTextCodec::tokenIdToString()
ThemeTextCodec::tokenIdFromString()
```

The serializer therefore no longer has separate `kColorRoles`, `kTypeRoles`, etc. name
tables. This gives one textual mapping boundary.

## Component identity

`ComponentId` moves to Foundation because it is semantic identity shared by layers, not
a ThemeIO concern. It remains separate from TokenId:

```text
ComponentId::ButtonFilled
        says "which component/variant"

TokenId(ColorRole::Primary)
        says "which semantic design value"
```

## Stability rules

1. Existing raw IDs are never renumbered.
2. Removed IDs become reserved; they are not reused for a different semantic token.
3. New categories receive a new explicit category byte.
4. Token textual names may gain read aliases in ThemeIO, but canonical names are versioned.
5. `TokenId::EncodingVersion` changes only for a deliberate encoding break.
6. Enum ordinal values are never used as persistence identities.
