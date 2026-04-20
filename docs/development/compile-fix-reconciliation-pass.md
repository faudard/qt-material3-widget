# Compile-Fix Reconciliation Pass

This pass focuses on reducing obvious compile and integration friction across the accumulated refactors.

## Goals

- keep headers declaration-oriented
- avoid new public/private leakage
- ensure new helper sources are added to the correct CMake targets
- reduce keyboard and accessibility regressions while the widget families are being hardened

## Changes in this pass

- added `src/core/private/qtmaterialaccessibilityhelper_p.h/.cpp`
- added the helper source to `qtmaterial3_core`
- tightened `QtMaterialAbstractButton` keyboard behavior and focus policy
- added accessibility synchronization hooks to button and input-control bases
- connected outlined/filled text fields to the accessibility helper
- added Escape handling and scrim-progress syncing hooks for `QtMaterialDialog`
- strengthened tests for keyboard activation, keyboard toggle, dialog Escape close, and text-field accessibility state

## Remaining likely friction points

This is still a best-effort pass, not a verified clean build. The next local build should check:

- Qt moc/autogen output for all changed QObject-derived classes
- hidden overload warnings for `setText()` and `setIcon()` on the abstract button family
- package/export correctness after the new core private helper source addition
- dialog open/close behavior around transition timing
- any include-path issues for relative private-helper includes in widgets

## Recommended next local sequence

1. configure with tests on
2. build libraries only
3. build tests
4. run button, selection, input, and dialog tests first
5. fix install/export only after the core libraries and tests are clean
