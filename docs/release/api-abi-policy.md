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

Appending enumerators is additive when all baseline enumerators and initializers remain
an unchanged prefix. The checker still rejects insertion, reordering, renumbering and
changes to an enum's scope, declaration or originating public header.

## Historical pre-1.0 policy

Before 1.0, public API and ABI could break between minor versions and obsolete
unpublished APIs were removed directly rather than retained as compatibility shims.
Those pre-1.0 cleanup rules no longer apply to the stable 1.x public surface.

## 2.0 compatibility proposal (pending review)

This is a proposal, not an approved breaking-change list.

| Surface | Current classification | 2.0 proposal |
| --- | --- | --- |
| Installed C++ public headers | Stable 1.x source API per canonical header manifest and signature baseline | Keep existing source API unless a reviewed migration entry identifies a specific incompatibility. |
| Exported binary symbols | Best-effort 1.x ABI | Inventory per Qt major, compiler, configuration and architecture before promising a binary ABI guarantee. |
| Private headers and implementation/PIMPL state | Internal | No downstream compatibility promise; never include or link against internal implementation details. |
| Theme JSON formatVersion 1 | Versioned public format | Keep reads and round-trips compatible; document and test explicit migration before any format change. |
| Studio schemas and Designer properties | Optional, scope-specific | Stabilize only separately qualified formats and authoring workflows; preserve unsupported-field diagnostics. |
| Python/PySide6 | Optional Qt6-only binding surface | Version and test the exported names, QObject ownership rules, wheel ABI and supported interpreter/Qt combinations. |
| Screenshot reconstruction | Experimental | Exclude from mandatory 2.0 runtime compatibility. |

Prefer Qt 6 for new 2.0 applications. The exact minimum supported Qt 6 version,
toolchains and C++ standard are still subject to matrix evaluation and review.
Keep Qt 5.14.2 and C++17 compatibility for the existing 1.x branch. Do not
remove Qt 5 from 2.0 until a maintenance policy and a tested consumer migration
are approved. No new minimum version or source/binary break is authorized by
this proposal.

The migration sequence is to pin the old dependency, compile the unchanged
application on the supported Qt 6 toolchain, migrate native controls through
the opt-in adapter (retaining QObject ownership, signals, models and validators),
then test installation, theme persistence and rollback. The installed sample
exercises a small reversible native-button migration, not a complete production
application trial. A real application migration, symbol inventory and reviewed
breaking-change ledger remain mandatory before 2.0 sign-off.
