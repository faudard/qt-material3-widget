# API and ABI policy

## Before 1.0

- public API may break between minor versions;
- ABI may break between minor versions;
- breaking changes are documented in `CHANGELOG.md`;
- retired pre-1.0 APIs are removed directly rather than kept as deprecated shims.

## From 1.0

- source compatibility is expected within the 1.x line for documented public headers;
- binary compatibility is best-effort unless a stricter ABI policy is published;
- once a stable API needs replacement, a deprecation mechanism may be introduced with an explicit removal window.

The 1.0 public-header manifest is the source of truth for the supported C++ surface.
