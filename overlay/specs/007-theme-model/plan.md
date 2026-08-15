# Implementation Plan: ThemeModel

## Core change

Replace:

```text
Theme -> ThemeSerializer -> compact JSON -> equality
```

with:

```text
Theme -> structural value equality
```

## Identity implementation

The comparator enumerates the known typed Material roles in a fixed semantic set and
compares component override maps by key/value rather than iteration order.

Compatibility typography aliases participate because they are still public mutable state.

## Runtime effect

ThemeContext becomes:

```cpp
if (theme == m_theme)
    return false;
```

and therefore no longer needs ThemeSerializer/QJsonDocument/fingerprint bytes.

ThemeManager may still depend on ThemeIO for import/export until Spec 009.

## Build convergence

1. Apply after Spec 006.
2. Run Python architecture checker/tests.
3. Compile ThemeModel.
4. Run new `tst_themeidentity`.
5. Run existing ThemeContext tests.
6. Shared/static.
7. Qt6 matrix.
8. Exact Qt5.14.2 gate.
