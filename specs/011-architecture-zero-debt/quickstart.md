# Quickstart / Validation

```text
python tools/check_architecture_contracts.py
cmake --build build
ctest --test-dir build --output-on-failure
python tools/repo_health.py
```
