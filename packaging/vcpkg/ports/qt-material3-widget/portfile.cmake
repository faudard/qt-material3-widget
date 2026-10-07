vcpkg_from_git(
    OUT_SOURCE_PATH SOURCE_PATH
    URL "https://github.com/faudard/qt-material3-widget.git"
    REF 6d45e7e87b7bcf46647d73b40008aed1689c7667
    HEAD_REF main
)

if("qt5" IN_LIST FEATURES AND "qt6" IN_LIST FEATURES)
    message(FATAL_ERROR "qt-material3-widget: select exactly one of qt5 or qt6")
endif()
if(NOT "qt5" IN_LIST FEATURES AND NOT "qt6" IN_LIST FEATURES)
    message(FATAL_ERROR "qt-material3-widget: either qt5 or qt6 must be enabled")
endif()

set(_qtm3_designer OFF)
if("designer-qt5" IN_LIST FEATURES)
    if(NOT "qt5" IN_LIST FEATURES)
        message(FATAL_ERROR "designer-qt5 requires the qt5 feature")
    endif()
    set(_qtm3_designer ON)
endif()
if("designer-qt6" IN_LIST FEATURES)
    if(NOT "qt6" IN_LIST FEATURES)
        message(FATAL_ERROR "designer-qt6 requires the qt6 feature")
    endif()
    set(_qtm3_designer ON)
endif()

if(VCPKG_LIBRARY_LINKAGE STREQUAL "dynamic")
    set(_qtm3_shared ON)
else()
    set(_qtm3_shared OFF)
endif()

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DBUILD_SHARED_LIBS=${_qtm3_shared}
        -DQTMATERIAL3_BUILD_TESTS=OFF
        -DQTMATERIAL3_BUILD_EXAMPLES=OFF
        -DQTMATERIAL3_BUILD_BENCHMARKS=OFF
        -DQTMATERIAL3_BUILD_DESIGNER_PLUGIN=${_qtm3_designer}
        -DQTMATERIAL3_INSTALL=ON
        -DQTMATERIAL3_USE_MCU=OFF
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(
    PACKAGE_NAME QtMaterial3Widgets
    CONFIG_PATH lib/cmake/QtMaterial3Widgets
)
vcpkg_copy_pdbs()

file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/debug/include"
    "${CURRENT_PACKAGES_DIR}/debug/share"
)

file(
    INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage"
    DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
)
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
