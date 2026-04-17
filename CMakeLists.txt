cmake_minimum_required(VERSION 3.21)

project(qt-material3-widgets
    VERSION 0.1.0
    DESCRIPTION "Material 3 widgets for Qt Widgets"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CTest)

include(cmake/QtMaterial3Options.cmake)
include(cmake/QtMaterial3Warnings.cmake)
include(cmake/QtMaterial3Targets.cmake)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(QTMATERIAL3_THEME_SOURCES
    src/theme/qtmaterialcolorscheme.cpp
    src/theme/qtmaterialtypographyscale.cpp
    src/theme/qtmaterialshapescale.cpp
    src/theme/qtmaterialelevationscale.cpp
    src/theme/qtmaterialmotiontokens.cpp
    src/theme/qtmaterialstatelayer.cpp
    src/theme/qtmaterialthemeoptions.cpp
    src/theme/qtmaterialtheme.cpp
    src/theme/qtmaterialthemebuilder.cpp
    src/theme/qtmaterialthememanager.cpp
    src/theme/qtmaterialthemeobserver.cpp
)

set(QTMATERIAL3_PUBLIC_HEADERS
    include/qtmaterial/qtmaterialglobal.h
    include/qtmaterial/theme/qtmaterialcolortoken.h
    include/qtmaterial/theme/qtmaterialcolorscheme.h
    include/qtmaterial/theme/qtmaterialtypographyscale.h
    include/qtmaterial/theme/qtmaterialshapescale.h
    include/qtmaterial/theme/qtmaterialelevationscale.h
    include/qtmaterial/theme/qtmaterialmotiontokens.h
    include/qtmaterial/theme/qtmaterialstatelayer.h
    include/qtmaterial/theme/qtmaterialthemeoptions.h
    include/qtmaterial/theme/qtmaterialtheme.h
    include/qtmaterial/theme/qtmaterialthemebuilder.h
    include/qtmaterial/theme/qtmaterialthememanager.h
    include/qtmaterial/theme/qtmaterialthemeobserver.h
    include/qtmaterial/theme/qtmateriallegacyaliasmap.h
)

if(COMMAND qt_add_library)
    qt_add_library(qtmaterial3_theme SHARED
        ${QTMATERIAL3_THEME_SOURCES}
        ${QTMATERIAL3_PUBLIC_HEADERS}
    )
else()
    add_library(qtmaterial3_theme SHARED
        ${QTMATERIAL3_THEME_SOURCES}
        ${QTMATERIAL3_PUBLIC_HEADERS}
    )
endif()

add_library(QtMaterial3::Theme ALIAS qtmaterial3_theme)

target_compile_definitions(qtmaterial3_theme PRIVATE QTMATERIAL3_THEME_LIBRARY)

target_link_libraries(qtmaterial3_theme
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
)

target_include_directories(qtmaterial3_theme
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

qtmaterial3_set_common_target_options(qtmaterial3_theme)
qtmaterial3_apply_warnings(qtmaterial3_theme)

set_target_properties(qtmaterial3_theme PROPERTIES
    OUTPUT_NAME QtMaterial3Theme
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

set(QTMATERIAL3_LEGACY_SOURCES
    src/legacy/qtmateriallegacyaliasmap.cpp
    src/legacy/qtmateriallegacythemebridge.cpp
)

if(COMMAND qt_add_library)
    qt_add_library(qtmaterial3_legacy SHARED
        ${QTMATERIAL3_LEGACY_SOURCES}
    )
else()
    add_library(qtmaterial3_legacy SHARED
        ${QTMATERIAL3_LEGACY_SOURCES}
    )
endif()

add_library(QtMaterial3::Legacy ALIAS qtmaterial3_legacy)

target_compile_definitions(qtmaterial3_legacy PRIVATE QTMATERIAL3_LEGACY_LIBRARY)

target_link_libraries(qtmaterial3_legacy
    PUBLIC
        qtmaterial3_theme
)

target_include_directories(qtmaterial3_legacy
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

qtmaterial3_set_common_target_options(qtmaterial3_legacy)
qtmaterial3_apply_warnings(qtmaterial3_legacy)

set_target_properties(qtmaterial3_legacy PROPERTIES
    OUTPUT_NAME QtMaterial3Legacy
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

install(
    TARGETS qtmaterial3_theme qtmaterial3_legacy
    EXPORT QtMaterial3WidgetsTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT QtMaterial3WidgetsTargets
    FILE QtMaterial3WidgetsTargets.cmake
    NAMESPACE QtMaterial3::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtMaterial3Widgets
)

if(QTMATERIAL3_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(QTMATERIAL3_BUILD_EXAMPLES)
    add_subdirectory(examples/theme-playground)
endif()