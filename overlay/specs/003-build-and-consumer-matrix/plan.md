# Implementation Plan: Build and Consumer Matrix

## Architecture

```text
consumer-matrix.json
       │
       ├ check_build_consumer_matrix.py
       │     └ structure / pairwise / exhaustive policy
       │
       └ run-consumer-matrix.py
             ├ source
             ├ add_subdirectory
             ├ FetchContent (local SOURCE_DIR)
             ├ install + find_package
             └ invalid package component
```

## Design decisions

1. A consumer includes `qtmaterialthemebuilder.h` and `qtmaterialfilledbutton.h`.
2. It links only `QtMaterial3::Widgets`, exercising transitive public dependencies.
3. FetchContent is validated without network access.
4. Installed consumers request package components explicitly.
5. Unknown required component is a negative contract.
6. PR CI is bounded; `full` exists for release/pre-merge exhaustive checks.
7. Qt-version certification stays in Spec 004.
