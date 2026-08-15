# Build / Consumer Contract v1

Supported producer/consumer evidence:

- source shared/static;
- `add_subdirectory`;
- local `FetchContent`;
- installed `find_package(QtMaterial3Widgets CONFIG COMPONENTS Theme Widgets)`;
- negative unknown REQUIRED component.

The canonical downstream contract compiles public Theme and Widgets headers while linking only
`QtMaterial3::Widgets`. Therefore all required Qt/QtMaterial dependencies and static-build
definitions must propagate transitively through exported CMake targets.

`ci` provides bounded pairwise coverage.  
`full` provides all consumer mechanism × linkage combinations.
