# Theming stabilization checklist

Use this checklist before marking the theming API stable.

## Token model

- [ ] `ColorScheme` roles are complete and frozen.
- [ ] Typography, shape, elevation, motion, state layer, density, icon-size, and component override tokens round-trip through JSON.
- [ ] Adding a new required token implies a schema-version decision.

## Backend behavior

- [ ] `QTMATERIAL3_USE_MCU=OFF` builds and tests fallback-only behavior.
- [ ] `QTMATERIAL3_USE_MCU=ON` builds when MCU is vendored.
- [ ] `QTMATERIAL3_USE_MCU=ON` without vendored MCU reports fallback behavior cleanly.
- [ ] Seed-generation goldens exist for fallback and MCU where applicable.
- [ ] Light/dark parity tests cover the same resolved color role set.
- [ ] Contrast-mode tests cover standard, medium, and high contrast.

## JSON schema

- [ ] `theme-v1.schema.json` remains readable.
- [ ] `theme-v2.schema.json` is the current write format.
- [ ] `Strict` mode rejects unknown root keys and incomplete resolved tokens.
- [ ] `UpgradeIfPossible` migrates v1 inputs without lossy token behavior.
- [ ] Timestamps stay optional so golden snapshots remain deterministic.

## Runtime behavior

- [ ] Applying identical options does not emit duplicate theme notifications.
- [ ] Import failure does not mutate the active theme.
- [ ] Theme revision changes once per real apply cycle.
- [ ] Observer enable/disable and explicit sync are covered by tests.
- [ ] Widgets repolish from manager notifications instead of rebuilding their own themes.

## Examples and docs

- [ ] Seed workflow example builds.
- [ ] Runtime switch example builds.
- [ ] JSON workflow example builds and strict-reads its own output.
- [ ] Component override example builds.
- [ ] Backend report example builds in fallback-only and MCU-requested configurations.
- [ ] Public docs link to schema, backend, runtime, and override guides.
