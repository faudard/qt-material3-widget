# Vendoring Material Color Utilities

The project can consume the upstream Material Color Utilities C++ implementation as source.

Recommended layout:

```text
third_party/material-color-utilities/
  cpp/
    cam/
    dynamiccolor/
    palettes/
    scheme/
    utils/
```

Example setup:

```bash
git submodule add https://github.com/material-foundation/material-color-utilities third_party/material-color-utilities
cmake -S . -B build -DQTMATERIAL3_BUILD_TESTS=ON -DQTMATERIAL3_USE_MCU=ON
cmake --build build
ctest --test-dir build --output-on-failure -R 'mcu|backend|seed'
```

The CMake option `QTMATERIAL3_USE_MCU` is a request. The compile definition `QTMATERIAL3_HAS_MCU` is the resolved result and is set to `1` only when the expected MCU headers and sources are present.
