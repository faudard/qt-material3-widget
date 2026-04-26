# Release checklist

Use this checklist before publishing a release or tagging a release candidate.

## Build matrix

- [ ] Linux fallback build passes.
- [ ] Linux MCU-requested build passes.
- [ ] Windows fallback build passes.
- [ ] macOS fallback build passes.
- [ ] Examples build.
- [ ] Install/export consumer build passes.

## Theming contracts

- [ ] Token model tests pass.
- [ ] JSON serializer tests pass.
- [ ] Strict read-mode tests pass.
- [ ] Seed golden tests pass.
- [ ] Backend diagnostics tests pass.
- [ ] System theme tests pass.
- [ ] Accessibility tests pass.
- [ ] Runtime theme stability tests pass.

## Documentation

- [ ] Theme JSON schema docs match the serializer.
- [ ] Backend docs match CMake behavior.
- [ ] Theme Studio docs match the example UI.
- [ ] Accessibility docs match token defaults.
- [ ] Visual regression docs match test behavior.

## Golden files

- [ ] Seed color goldens were not changed unintentionally.
- [ ] MCU goldens were updated only when MCU algorithm/version changed.
- [ ] Visual goldens were updated only through the documented update path.

## Package export

- [ ] `cmake --install` exports all public headers.
- [ ] `QtMaterial3WidgetsConfig.cmake` loads in a clean consumer build.
- [ ] Exported targets link without private source-tree include paths.
- [ ] Public headers compile independently.
