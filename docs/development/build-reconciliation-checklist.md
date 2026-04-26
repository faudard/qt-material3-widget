# Build Reconciliation Checklist

This checklist turns the scaffold into a real shared-library project.

## 1. Configure pass
- fresh build directory
- verify all targets are declared
- verify all source files referenced by CMake exist
- verify Qt modules are complete for each target
- verify install/export generation succeeds during configure

## 2. Library compile pass
Build libraries first with tests and examples off:
- `qtmaterial3_theme`
- `qtmaterial3_core`
- `qtmaterial3_specs`
- `qtmaterial3_effects`
- `qtmaterial3_widgets`

## 3. Public header audit
- no private headers leaked through `include/`
- no `src/.../private` includes from public headers
- export macros present on public classes
- heavy implementation kept out of public headers

## 4. Dependency audit
Expected direction:
- `theme` independent
- `legacy -> theme`
- `core -> theme`
- `specs -> theme, core`
- `effects -> theme, core`
- `widgets -> theme, core, specs, effects`

## 5. Examples pass
Build:
- gallery
- minimal consumer
- install consumer
- theme playground

## 6. Test pass
- build tests after libraries compile
- classify tests as passing, placeholder, or intentionally disabled
- do not leave ambiguous broken tests

## 7. Install/export pass
- install headers and shared libraries
- install exported targets
- install config package files
- validate external consumer with `find_package(QtMaterial3Widgets REQUIRED)`

## 8. Warnings pass
Turn warnings up and clean:
- missing `override`
- hidden virtuals
- sign/size issues in geometry math
- moc warnings
- unused parameters

## 9. First release-quality checkpoint
The scaffold is reconciled only when:
- configure passes
- libraries compile
- examples compile
- install/export works
- consumer example works
- warnings are acceptable
