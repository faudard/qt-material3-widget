# Contributing

Contributions should preserve the project architecture:

1. widgets render from resolved specs, not ad hoc theme lookups;
2. public widgets expose automation metadata;
3. interaction state uses `QtMaterialInteractionState` vocabulary;
4. accessibility and keyboard behavior are part of the implementation, not polish;
5. new public API requires tests and documentation.

Before submitting a change:

```bash
cmake -S . -B build -DQTMATERIAL3_BUILD_TESTS=ON -DQTMATERIAL3_BUILD_EXAMPLES=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Architecture checks

The repository enforces staged module boundaries and migrated-family rules.

Run the checker directly:

```bash
python scripts/architecture/check_architecture.py --root .
```

Or through CMake:

```bash
cmake --build build --target qtmaterial3_architecture_check
ctest --test-dir build -L architecture --output-on-failure
```

When migrating another component family, extend
`scripts/architecture/architecture_rules.json` in the same change. Keep rules
strict for migrated code and avoid baselining new violations.

