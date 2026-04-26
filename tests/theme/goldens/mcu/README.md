# MCU seed goldens

These snapshots are generated only when the Material Color Utilities backend is actually compiled in.

From a repository checkout with MCU vendored under `third_party/material-color-utilities` or passed through `-DQTMATERIAL3_MCU_ROOT=/path/to/material-color-utilities`, run:

```bash
cmake -S . -B build -DQTMATERIAL3_BUILD_TESTS=ON -DQTMATERIAL3_USE_MCU=ON
cmake --build build
QTMATERIAL3_UPDATE_GOLDENS=1 ctest --test-dir build --output-on-failure -R tst_theme_mcu_goldens
ctest --test-dir build --output-on-failure -R 'mcu|backend|seed'
```

Commit the generated `seed_*.json` files with the MCU integration change. Golden files intentionally omit timestamps.
