# Foundation contains dependency-light value types shared by Theme, Specs,
# Core, Effects, and Widgets. It deliberately has no Qt dependency.
add_library(qtmaterial3_foundation INTERFACE)
add_library(QtMaterial3::Foundation ALIAS qtmaterial3_foundation)

target_compile_features(qtmaterial3_foundation INTERFACE cxx_std_17)

target_include_directories(
    qtmaterial3_foundation
    INTERFACE
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

set_target_properties(
    qtmaterial3_foundation
    PROPERTIES
        EXPORT_NAME Foundation
)
