# Plan

1. Machine-readable Qt support manifest.
2. CMake floor/exact-version gate.
3. Exact hosted Qt 5.14.2 build/test job.
4. Separate MSVC 14.28 reference gate.
5. Central `qtmaterialqtcompat.h`.
6. Compile regression test.
7. Installed consumer on Qt5.
8. Iterate on any real Qt5 compile failures until green.

Authority order: real compile > tests > installed consumer > CMake assertion >
toolchain probe > static governance.
