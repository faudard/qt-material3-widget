cmake_minimum_required(VERSION 3.21)

project(qt-material3-widgets
    VERSION 0.2.0
    DESCRIPTION "Material 3 widgets for Qt Widgets"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CTest)

option(QTMATERIAL3_BUILD_TESTS "Build tests" ${BUILD_TESTING})
option(QTMATERIAL3_BUILD_EXAMPLES "Build examples" ON)
option(QTMATERIAL3_USE_MCU "Use Material Color Utilities backend" ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

if(COMMAND qt_standard_project_setup)
    qt_standard_project_setup()
else()
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
endif()

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

macro(qtmaterial3_add_library target)
    if(COMMAND qt_add_library)
        qt_add_library(${target} ${ARGN})
    else()
        add_library(${target} ${ARGN})
    endif()
endmacro()

set(QTMATERIAL3_THEME_SOURCES
    src/theme/qtmaterialcolorscheme.cpp
    src/theme/qtmaterialthemeoptions.cpp
    src/theme/qtmaterialtheme.cpp
    src/theme/qtmaterialthemebuilder.cpp
    src/theme/qtmaterialthememanager.cpp
    src/theme/qtmaterialthemeobserver.cpp
    src/theme/qtmaterialtypographyscale.cpp
    src/theme/qtmaterialshapescale.cpp
    src/theme/qtmaterialelevationscale.cpp
    src/theme/qtmaterialmotiontokens.cpp
    src/theme/qtmaterialstatelayer.cpp
)

set(QTMATERIAL3_THEME_HEADERS
    include/qtmaterial/qtmaterialglobal.h
    include/qtmaterial/theme/qtmaterialcolortoken.h
    include/qtmaterial/theme/qtmaterialcolorscheme.h
    include/qtmaterial/theme/qtmaterialthemeoptions.h
    include/qtmaterial/theme/qtmaterialtheme.h
    include/qtmaterial/theme/qtmaterialthemebuilder.h
    include/qtmaterial/theme/qtmaterialthememanager.h
    include/qtmaterial/theme/qtmaterialthemeobserver.h
    include/qtmaterial/theme/qtmateriallegacyaliasmap.h
    include/qtmaterial/theme/qtmaterialtypographyscale.h
    include/qtmaterial/theme/qtmaterialshapescale.h
    include/qtmaterial/theme/qtmaterialelevationscale.h
    include/qtmaterial/theme/qtmaterialmotiontokens.h
    include/qtmaterial/theme/qtmaterialstatelayer.h
)

qtmaterial3_add_library(qtmaterial3_theme SHARED
    ${QTMATERIAL3_THEME_SOURCES}
    ${QTMATERIAL3_THEME_HEADERS}
)
add_library(QtMaterial3::Theme ALIAS qtmaterial3_theme)

target_compile_features(qtmaterial3_theme PUBLIC cxx_std_17)
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
set_target_properties(qtmaterial3_theme PROPERTIES
    OUTPUT_NAME QtMaterial3Theme
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

set(QTMATERIAL3_LEGACY_SOURCES
    src/legacy/qtmateriallegacyaliasmap.cpp
    src/legacy/qtmateriallegacythemebridge.cpp
)

set(QTMATERIAL3_LEGACY_HEADERS
    include/qtmaterial/theme/qtmateriallegacyaliasmap.h
)

qtmaterial3_add_library(qtmaterial3_legacy SHARED
    ${QTMATERIAL3_LEGACY_SOURCES}
    ${QTMATERIAL3_LEGACY_HEADERS}
)
add_library(QtMaterial3::Legacy ALIAS qtmaterial3_legacy)

target_compile_features(qtmaterial3_legacy PUBLIC cxx_std_17)
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
set_target_properties(qtmaterial3_legacy PROPERTIES
    OUTPUT_NAME QtMaterial3Legacy
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

set(QTMATERIAL3_CORE_SOURCES
    src/core/qtmaterialwidget.cpp
    src/core/qtmaterialinteractionstate.cpp
    src/core/qtmaterialcontrol.cpp
    src/core/qtmaterialsurface.cpp
    src/core/qtmaterialoverlaysurface.cpp
    src/core/qtmaterialabstractbutton.cpp
    src/core/qtmaterialselectioncontrol.cpp
    src/core/qtmaterialinputcontrol.cpp
)

set(QTMATERIAL3_CORE_HEADERS
    include/qtmaterial/core/qtmaterialdensity.h
    include/qtmaterial/core/qtmaterialwidget.h
    include/qtmaterial/core/qtmaterialinteractionstate.h
    include/qtmaterial/core/qtmaterialcontrol.h
    include/qtmaterial/core/qtmaterialsurface.h
    include/qtmaterial/core/qtmaterialoverlaysurface.h
    include/qtmaterial/core/qtmaterialabstractbutton.h
    include/qtmaterial/core/qtmaterialselectioncontrol.h
    include/qtmaterial/core/qtmaterialinputcontrol.h
)

qtmaterial3_add_library(qtmaterial3_core SHARED
    ${QTMATERIAL3_CORE_SOURCES}
    ${QTMATERIAL3_CORE_HEADERS}
)
add_library(QtMaterial3::Core ALIAS qtmaterial3_core)

target_compile_features(qtmaterial3_core PUBLIC cxx_std_17)
target_compile_definitions(qtmaterial3_core PRIVATE QTMATERIAL3_CORE_LIBRARY)
target_link_libraries(qtmaterial3_core
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        qtmaterial3_theme
)
target_include_directories(qtmaterial3_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
set_target_properties(qtmaterial3_core PROPERTIES
    OUTPUT_NAME QtMaterial3Core
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

set(QTMATERIAL3_SPECS_SOURCES
    src/specs/qtmaterialspecfactory.cpp
    src/specs/qtmaterialbuttonspec.cpp
    src/specs/qtmaterialiconbuttonspec.cpp
    src/specs/qtmaterialcheckboxspec.cpp
    src/specs/qtmaterialradiobuttonspec.cpp
    src/specs/qtmaterialswitchspec.cpp
    src/specs/qtmaterialdialogspec.cpp
    src/specs/qtmaterialtextfieldspec.cpp
)

set(QTMATERIAL3_SPECS_HEADERS
    include/qtmaterial/specs/qtmaterialbuttonspec.h
    include/qtmaterial/specs/qtmaterialiconbuttonspec.h
    include/qtmaterial/specs/qtmaterialcheckboxspec.h
    include/qtmaterial/specs/qtmaterialradiobuttonspec.h
    include/qtmaterial/specs/qtmaterialswitchspec.h
    include/qtmaterial/specs/qtmaterialdialogspec.h
    include/qtmaterial/specs/qtmaterialtextfieldspec.h
    include/qtmaterial/specs/qtmaterialspecfactory.h
)

qtmaterial3_add_library(qtmaterial3_specs SHARED
    ${QTMATERIAL3_SPECS_SOURCES}
    ${QTMATERIAL3_SPECS_HEADERS}
)
add_library(QtMaterial3::Specs ALIAS qtmaterial3_specs)

target_compile_features(qtmaterial3_specs PUBLIC cxx_std_17)
target_compile_definitions(qtmaterial3_specs PRIVATE QTMATERIAL3_SPECS_LIBRARY)
target_link_libraries(qtmaterial3_specs
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        qtmaterial3_theme
        qtmaterial3_core
)
target_include_directories(qtmaterial3_specs
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
set_target_properties(qtmaterial3_specs PROPERTIES
    OUTPUT_NAME QtMaterial3Specs
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

install(
    TARGETS
        qtmaterial3_theme
        qtmaterial3_legacy
        qtmaterial3_core
        qtmaterial3_specs
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
    add_subdirectory(examples)
endif()