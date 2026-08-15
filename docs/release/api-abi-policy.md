# API and ABI policy

Before 1.0:

- public API may break between minor versions;
- ABI may break between minor versions;
- all breaking changes must be documented in `CHANGELOG.md`.

At 1.0:

- source compatibility is expected within the 1.x line for public headers;
- binary compatibility is best-effort unless the project explicitly decides otherwise;
- deprecated API should remain until the next major release unless removal is documented before 1.0.

Deprecation macros:

```cpp
QTMATERIAL3_DEPRECATED
QTMATERIAL3_DEPRECATED_X("message")
QTMATERIAL3_DISABLE_DEPRECATED_BEFORE
```
