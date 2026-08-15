# Critical Build Matrix
Automated evidence must cover:
- linkage: shared + static;
- build type: Release, plus Debug on at least one primary platform;
- MCU: OFF + requested ON;
- consumers: installed `find_package`, `add_subdirectory`, `FetchContent`;
- library-only: tests/examples OFF;
- Qt versions: delegated to spec 004.
Use pairwise PR coverage and broader release-readiness coverage.
