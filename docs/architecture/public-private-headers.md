# Public / Private Header Contract

QtMaterial3's header surface is explicit and version-controlled.

## Rollout status

Phase 012.B seals the source and CMake package contracts:

- every header is classified by the manifest as public or private;
- public headers are checked for private and source-tree implementation includes;
- only public entries are installed, preserving their relative paths;
- one-header-per-translation-unit tests are generated from the public manifest;
- installed and invalid-component consumer scenarios compare the installed prefix exactly;
- the full source/install contract runs in repository health.

Real Qt5/Qt6 shared/static install and consumer executions remain required evidence. They
cannot be replaced by the static contract and are tracked separately in the phase tasks.

## Source-of-truth manifest

`cmake/QtMaterial3HeaderSurfaceManifest.cmake` partitions every header under
`include/qtmaterial/` into exactly one of:

```text
QTMATERIAL3_PUBLIC_HEADERS
QTMATERIAL3_PRIVATE_HEADERS
```

A new header is therefore not automatically public simply because it was added under
`include/`.

## Public header rules

A public header:
- is listed in `QTMATERIAL3_PUBLIC_HEADERS`;
- is installed by the package;
- must compile as the first/only project include in a translation unit;
- must not include a private `_p.h`/`private/` header;
- must not reach into `src/`;
- may forward-declare implementation classes;
- may expose only types whose intended API status is public.

## Private header rules

A private header:
- is explicitly classified private;
- is never installed;
- is never part of the public consumer contract;
- may change without source/ABI compatibility guarantees.

Private headers currently under `include/qtmaterial/**/private/` are tolerated as source-tree
implementation details during the 0.x cleanup, but package consumers never receive them.
Moving all private headers physically under `src/**/private/` remains the preferred final
layout when doing so does not create unnecessary churn.

## Install semantics

The old:

```cmake
install(DIRECTORY include/ ...)
```

is forbidden.

Only files in the public manifest are installed, one path at a time, preserving the
`qtmaterial/...` directory layout.

## Consumer verification

After every install scenario in the consumer matrix:

```text
installed headers == public manifest
```

exactly.

Missing public files, unexpected headers and any leaked private file fail the consumer job.

## API surface versus ABI

This specification defines *which headers are public*. It does not freeze all APIs or ABI.
PIMPL discipline, API manifests and ABI compatibility are strengthened in later dedicated
milestones.

The distinction matters:

```text
public header
    !=
stable forever

private header
    =
never supported downstream API
```
