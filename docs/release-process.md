# Release process

## Release candidate

A release candidate is cut only when all required CI workflows are green on the release branch.

Required checks:

1. Run the normal CI matrix.
2. Run repository-health and documentation gates.
3. Run `python tools/check_release.py --root . --scope all`; `all` includes the public-surface/API-freeze contract.
4. Verify the release-scoped component registry contains no `partial`, `skeleton`, or `planned` component.
5. Verify Theme JSON fixtures, token matrix, SystemTheme, Theme Studio, and runtime propagation tests are green.
6. Verify interaction-state, ripple, focus, motion, shadow/DPR and transient-focus tests are green.
7. Build and install the project with `QTMATERIAL3_INSTALL=ON`.
8. Run CPack for ZIP and TGZ binary archives.
9. Run CPack source packaging for ZIP and TGZ.
10. Build an external installed-package consumer.
11. For the first stable release, generate and review the Doxygen-derived public
    API signature baseline with `tools/api_signature_baseline.py`.
12. Commit and review the required visual PNG goldens, then run
    `tst_theme_visual_regression` with `QTMATERIAL3_VISUAL_STRICT=1` in the
    pinned release environment.
13. Review `CHANGELOG.md` and remove the `Unreleased` marker when publishing.

Example:

```bash
cmake -S . -B build-release -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DQTMATERIAL3_BUILD_TESTS=OFF \
  -DQTMATERIAL3_BUILD_EXAMPLES=OFF \
  -DQTMATERIAL3_BUILD_BENCHMARKS=OFF \
  -DQTMATERIAL3_USE_MCU=OFF \
  -DQTMATERIAL3_INSTALL=ON

cmake --build build-release --parallel
cmake --install build-release --prefix build-release/stage

cpack --config build-release/CPackConfig.cmake -G TGZ
cpack --config build-release/CPackConfig.cmake -G ZIP
cpack --config build-release/CPackSourceConfig.cmake -G TGZ
cpack --config build-release/CPackSourceConfig.cmake -G ZIP
```

For the first stable release, generate the stable API baseline after Doxygen XML is available:

```bash
python tools/api_signature_baseline.py \
  --xml-dir build/doxygen/xml \
  --baseline tools/api-baseline-1.0.json \
  --baseline-major 1 \
  --write
```

The 1.0 release checker intentionally fails if the baseline or reviewed visual
goldens are missing. Before creating the tag, run `release-readiness` manually with
`workflow_dispatch` on the exact release commit so the pinned Qt 6.4.0 strict visual
gate executes before publication; the same gate runs again for `v1.*` tags. The
release tag format is `vMAJOR.MINOR.PATCH`.

## Production SDK 1.30 qualification

The following checks extend the normal release contract rather than replacing it.
A green infrastructure PR does not mean a release candidate has been signed off.

| Gate | Executable evidence | Release decision |
| --- | --- | --- |
| A — distribution | `consumer-contract`, cross-platform installed consumers and `release-package` | Require shared/static source and downstream contracts, moved install prefixes, ZIP/TGZ checksums, archive extraction and application execution. |
| B — performance | `performance-scale` scheduled/manual/tag runs | Require CPU/RSS/p95/p99 budgets and nonempty benchmark evidence. PR budget exceedances remain report-only. |
| C — compatibility | Public header manifest, Doxygen signature baseline, native migration smoke | Review the API/ABI proposal and approve any breaking changes separately. |
| D — release candidate | Normal CI, release-readiness, strict visual goldens, native AT certification and optional SDK workflows | Require actual successful runs on the release commit and a human sign-off. |

Run installed consumers (shared and static) after removing the original installation
location:

```bash
python scripts/ci/run-consumer-matrix.py --profile full
```

Validate a trusted binary archive with its CPack-generated SHA256 sidecar in the
same directory (replace the archive filename with the actual artifact):

```bash
python scripts/ci/run-consumer-matrix.py --archive path/to/binary-sdk.zip
```

The downstream application is copied out of its original source layout. It uses
installed headers and targets only, creates a Material widget, adapts an existing
native button, checks signal delivery and restores the original style. The runner
moves the prefix into a path containing spaces and executes the program offscreen.
Qt remains an external runtime dependency; this is not a self-contained Qt deployment.

Conan and vcpkg are not qualified by these checks. Designer, Studio and Python
remain opt-in, with their own host/runtime ABI and packaging validation. The core
SDK consumer does not require these optional dependencies.

For a release candidate, use the existing Desktop Scale 2.0 budgets without
changing the thresholds simply to obtain a successful run. A controlled runner
must be calibrated and its OS, Qt, compiler and build configuration recorded;
hosted runner success alone is not proof of a general performance SLA.

### 2.0 readiness record

Record links to the exact commit's CI runs, binary archives and checksum results,
performance JSON/history and profiler output, reviewed API and schema inventory,
migration trial outcomes and native AT/visual certification. Record scope,
toolchain versions, failures, owners and any explicitly excluded experimental
features. Do not mark the RC ready if one of the mandatory 1.22–1.24 prerequisites
or any advertised stable optional feature is unqualified. See
[API and ABI policy](release/api-abi-policy.md) for the compatibility proposal.

No new tag or version bump is implied by the qualification infrastructure.
