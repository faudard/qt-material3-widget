# Qt 5 / Qt 6 Compatibility Policy

QtMaterial3's C++ contract is **Qt 5.14.2** as the exact Qt5 floor and **Qt 6.4+** as the Qt6 floor, with C++17.

The hosted Qt5 gate uses `windows-2022`, Qt **5.14.2** archive
`win64_msvc2017_64`, and MSVC **v142/14.29**. GitHub retired its
`windows-2019` image; the hosted job therefore uses a newer runner with the
ABI-compatible v142 compiler family.

The local/reference workstation contract additionally covers **MSVC 14.28 /
cl 19.28**. Hosted CI MUST NOT pretend to test that exact compiler minor.

Version-specific API differences belong in
`include/qtmaterial/foundation/qtmaterialqtcompat.h` or a narrowly scoped
private adapter. Examples include Qt5 `localPos()/screenPos()` versus Qt6
`position()/globalPosition()`, QWidget enter-event signatures, and
`QEvent::DevicePixelRatioChange` (Qt 6.6+ only).

Python bindings remain a separate Qt6/PySide6/Shiboken6 contract.
