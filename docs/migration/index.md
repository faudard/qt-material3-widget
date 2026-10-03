# Migration

Qt Material 3 Widgets 1.x treats documented installed public headers as the source-compatibility surface. Check the changelog and this section before updating a pinned dependency.

## Upgrade checklist

1. Read `CHANGELOG.md` for the target release.
2. Review the migration page for your version jump.
3. Build against every Qt major/minor line you support.
4. Run keyboard/accessibility-sensitive UI tests.
5. Recheck custom component overrides and serialized theme files.
6. If you consume an installed package, validate `find_package(QtMaterial3Widgets REQUIRED)` in a clean prefix.

## Guides

- [1.0 → 1.1](1.0-to-1.1.md)

For the compatibility boundary itself, see [API stability](../public-api/api-stability.md).
