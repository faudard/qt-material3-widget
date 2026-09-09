# Public header hygiene

The public-header contract covers every header classified in
`QTMATERIAL3_PUBLIC_HEADERS`, not only widget headers.

## Rules

Each public header:

- is explicitly listed in `cmake/QtMaterial3HeaderSurfaceManifest.cmake`;
- can be included as the only QtMaterial3 header in a translation unit;
- does not include a `_p.h` or a header below a `private/` directory;
- does not reach into the source implementation tree;
- exposes only types intended for downstream consumers.

The manifest is reviewed API metadata. Adding a header below `include/qtmaterial/`
does not publish it automatically, and private-looking paths cannot be promoted to
the public list.

## Local commands

Validate the complete source/install contract:

```bash
python3 tools/check_public_private_headers.py --root .
```

Validate that the checked-in manifest exactly matches the source tree:

```bash
python3 tools/update_header_surface_manifest.py --root .
```

Regenerating the manifest is a deliberate API review action. Use `--write` only when
the resulting public/private diff has been reviewed.

## CTest integration

The CMake integration adds:

1. `public_private_header_contract` — full source/install contract validation.
2. `public_header_self_contained/<header>` — one generated translation unit for
   each of the 166 declared public headers.

Run them with:

```bash
ctest --test-dir build -R "public_private_header_contract|public_header_self_contained" --output-on-failure
```

There is no exception or suppression mechanism. A violation must be resolved by
correcting the include boundary, the implementation ownership, or the reviewed
classification.
