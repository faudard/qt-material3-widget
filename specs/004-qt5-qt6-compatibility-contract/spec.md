# Feature Specification: Qt 5 / Qt 6 Compatibility Contract

**Status**: Ready for application  
**Milestone**: 0.5  
**Created**: 2026-08-15

## Requirements

- **QTC-FR-001** Qt5 floor is exactly 5.14.2.
- **QTC-FR-002** Qt6 floor is explicitly 6.4.0.
- **QTC-FR-003** CMake rejects versions below the selected floor.
- **QTC-FR-004** Hosted CI has an exact Qt 5.14.2 Windows x64 gate.
- **QTC-FR-005** The Qt5 archive is `win64_msvc2017_64`.
- **QTC-FR-006** Hosted Qt5 uses v142/14.29 and probes the compiler.
- **QTC-FR-007** MSVC 14.28/cl 19.28 remains a separate reference gate.
- **QTC-FR-008** The actual Qt version is probed before build and asserted by CMake.
- **QTC-FR-009** Qt5 builds library, tests and examples.
- **QTC-FR-010** Qt5 validates an installed downstream consumer.
- **QTC-FR-011** Shared Qt-major event differences use a central compatibility helper.
- **QTC-FR-012** `DevicePixelRatioChange` is guarded before Qt 6.6.
- **QTC-FR-013** The compatibility helper has a cross-major compile test.
- **QTC-FR-014** Repository health validates the compatibility contract.
- **QTC-FR-015** Qt5 compatibility is a C++ contract; Python remains Qt6/PySide6.
- **QTC-FR-016** Qt5 support is not considered release-certified until the exact CI gate is green.

## Non-goals

No PySide2/Shiboken2, no Qt7 work, no claim that hosted 14.29 is exact 14.28.

## Acceptance

`windows-qt5.14.2-v142`, `windows-qt6.7.3-v142`, `ubuntu-qt6`,
and `macos-qt6.8.4` are green, plus local/reference MSVC 14.28 evidence.
