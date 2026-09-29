# API and ABI policy

## 1.x stable line

- documented installed public headers are expected to remain source-compatible within
  the 1.x line;
- the checked-in 1.0 Doxygen signature baseline enforces that stable source surface;
- additive declarations are allowed in 1.x, while removals or signature changes require
  a future major release unless an explicitly compatible migration is available;
- binary compatibility is best-effort unless a stricter ABI policy is published;
- when stable API needs replacement, deprecation must use an explicit removal window
  before a future major version.

The canonical public-header manifest defines which headers are supported. The baseline
records the public/protected declarations originating from exactly those installed
headers, including namespace-level declarations and enum initializers. Private headers,
internal resolver/spec implementation surfaces and PIMPL state are deliberately
excluded.

## Historical pre-1.0 policy

Before 1.0, public API and ABI could break between minor versions and obsolete
unpublished APIs were removed directly rather than retained as compatibility shims.
Those pre-1.0 cleanup rules no longer apply to the stable 1.x public surface.
