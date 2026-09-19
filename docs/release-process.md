# Release process

## 0.5.x release candidate

A release candidate is cut only when all required CI workflows are green on the release branch.

Required checks:

1. Run the normal CI matrix.
2. Run repository-health, documentation and component-maturity gates.
3. Run `python tools/check_release_contract.py --root . --expected-version 0.5.0`.
4. Build and install the project with `QTMATERIAL3_INSTALL=ON`.
5. Run CPack for ZIP and TGZ binary archives.
6. Run CPack source packaging for ZIP and TGZ.
7. Build an external installed-package consumer.
8. Verify the release-scoped component registry contains no `partial`, `skeleton`, or `planned`
   component.
9. Review `CHANGELOG.md` and remove the `Unreleased` marker when publishing.

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

The release tag format is `vMAJOR.MINOR.PATCH`.
