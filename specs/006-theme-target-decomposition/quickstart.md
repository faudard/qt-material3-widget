# Quickstart / Validation

```text
cmake -S . -B build -G Ninja -DQTMATERIAL3_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
python tools/check_architecture_contracts.py
```
