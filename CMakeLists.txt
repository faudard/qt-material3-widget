cmake_minimum_required(VERSION 3.21)

project(qt-material3-widgets
    VERSION 0.4.0
    DESCRIPTION "Material 3 widgets for Qt Widgets"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CTest)
include(CMakePackageConfigHelpers)

option(QTMATERIAL3_BUILD_TESTS "Build tests" ${BUILD_TESTING})
option(QTMATERIAL3_BUILD_EXAMPLES "Build examples" ON)
option(QTMATERIAL3_BUILD_BENCHMARKS "Build benchmark executables" OFF)
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

macro(qtm3_add_library target)
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

qtm3_add_library(qtmaterial3_theme SHARED
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

qtm3_add_library(qtmaterial3_legacy SHARED
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

qtm3_add_library(qtmaterial3_core SHARED
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
    src/specs/qtmaterialfabspec.cpp
    src/specs/qtmaterialcardspec.cpp
    src/specs/qtmaterialnavigationdrawerspec.cpp
    src/specs/qtmaterialbottomsheetspec.cpp
    src/specs/qtmaterialbannerspec.cpp
    src/specs/qtmaterialappbarspec.cpp
    src/specs/qtmateriallistitemspec.cpp
    src/specs/qtmaterialdividerspec.cpp
    src/specs/qtmaterialautocompletepopupspec.cpp
    src/specs/qtmaterialdatefieldspec.cpp
)

set(QTMATERIAL3_SPECS_HEADERS
    include/qtmaterial/specs/qtmaterialbuttonspec.h
    include/qtmaterial/specs/qtmaterialiconbuttonspec.h
    include/qtmaterial/specs/qtmaterialcheckboxspec.h
    include/qtmaterial/specs/qtmaterialradiobuttonspec.h
    include/qtmaterial/specs/qtmaterialswitchspec.h
    include/qtmaterial/specs/qtmaterialdialogspec.h
    include/qtmaterial/specs/qtmaterialtextfieldspec.h
    include/qtmaterial/specs/qtmaterialfabspec.h
    include/qtmaterial/specs/qtmaterialcardspec.h
    include/qtmaterial/specs/qtmaterialnavigationdrawerspec.h
    include/qtmaterial/specs/qtmaterialbottomsheetspec.h
    include/qtmaterial/specs/qtmaterialbannerspec.h
    include/qtmaterial/specs/qtmaterialappbarspec.h
    include/qtmaterial/specs/qtmateriallistitemspec.h
    include/qtmaterial/specs/qtmaterialdividerspec.h
    include/qtmaterial/specs/qtmaterialautocompletepopupspec.h
    include/qtmaterial/specs/qtmaterialdatefieldspec.h
    include/qtmaterial/specs/qtmaterialspecfactory.h
)

qtm3_add_library(qtmaterial3_specs SHARED
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

set(QTMATERIAL3_EFFECTS_SOURCES
    src/effects/qtmaterialripplecontroller.cpp
    src/effects/qtmaterialstatelayerpainter.cpp
    src/effects/qtmaterialfocusindicator.cpp
    src/effects/qtmaterialshadowrenderer.cpp
    src/effects/qtmaterialshadowcache.cpp
    src/effects/qtmaterialtransitioncontroller.cpp
    src/effects/qtmaterialscrimwidget.cpp
)

set(QTMATERIAL3_EFFECTS_HEADERS
    include/qtmaterial/effects/qtmaterialripplecontroller.h
    include/qtmaterial/effects/qtmaterialstatelayerpainter.h
    include/qtmaterial/effects/qtmaterialfocusindicator.h
    include/qtmaterial/effects/qtmaterialshadowrenderer.h
    include/qtmaterial/effects/qtmaterialshadowcache.h
    include/qtmaterial/effects/qtmaterialtransitioncontroller.h
    include/qtmaterial/effects/qtmaterialscrimwidget.h
)

qtm3_add_library(qtmaterial3_effects SHARED
    ${QTMATERIAL3_EFFECTS_SOURCES}
    ${QTMATERIAL3_EFFECTS_HEADERS}
)
add_library(QtMaterial3::Effects ALIAS qtmaterial3_effects)

target_compile_features(qtmaterial3_effects PUBLIC cxx_std_17)
target_compile_definitions(qtmaterial3_effects PRIVATE QTMATERIAL3_EFFECTS_LIBRARY)
target_link_libraries(qtmaterial3_effects
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        qtmaterial3_theme
        qtmaterial3_core
)
target_include_directories(qtmaterial3_effects
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
set_target_properties(qtmaterial3_effects PROPERTIES
    OUTPUT_NAME QtMaterial3Effects
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

set(QTMATERIAL3_WIDGETS_SOURCES
    src/widgets/buttons/private/qtmaterialbuttonrenderhelper.cpp
    src/widgets/buttons/qtmaterialtextbutton.cpp
    src/widgets/buttons/qtmaterialfilledbutton.cpp
    src/widgets/buttons/qtmaterialiconbutton.cpp
    src/widgets/buttons/qtmaterialfilledtonalbutton.cpp
    src/widgets/buttons/qtmaterialoutlinedbutton.cpp
    src/widgets/buttons/qtmaterialelevatedbutton.cpp
    src/widgets/buttons/qtmaterialfab.cpp
    src/widgets/buttons/qtmaterialextendedfab.cpp
    src/widgets/selection/qtmaterialcheckbox.cpp
    src/widgets/selection/qtmaterialswitch.cpp
    src/widgets/selection/qtmaterialradiobutton.cpp
    src/widgets/surfaces/qtmaterialdialog.cpp
    src/widgets/surfaces/qtmaterialnavigationdrawer.cpp
    src/widgets/surfaces/qtmaterialbottomsheet.cpp
    src/widgets/surfaces/qtmaterialbanner.cpp
    src/widgets/surfaces/qtmaterialcard.cpp
    src/widgets/surfaces/qtmaterialtopappbar.cpp
    src/widgets/surfaces/qtmaterialbottomappbar.cpp
    src/widgets/inputs/qtmaterialoutlinedtextfield.cpp
    src/widgets/inputs/qtmaterialfilledtextfield.cpp
    src/widgets/inputs/qtmaterialautocompletepopup.cpp
    src/widgets/inputs/qtmaterialdatefield.cpp
    src/widgets/data/qtmateriallistitem.cpp
    src/widgets/data/qtmateriallist.cpp
    src/widgets/data/qtmaterialdivider.cpp
)

set(QTMATERIAL3_WIDGETS_HEADERS
    include/qtmaterial/widgets/buttons/qtmaterialtextbutton.h
    include/qtmaterial/widgets/buttons/qtmaterialfilledbutton.h
    include/qtmaterial/widgets/buttons/qtmaterialiconbutton.h
    include/qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h
    include/qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h
    include/qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h
    include/qtmaterial/widgets/buttons/qtmaterialfab.h
    include/qtmaterial/widgets/buttons/qtmaterialextendedfab.h
    include/qtmaterial/widgets/selection/qtmaterialcheckbox.h
    include/qtmaterial/widgets/selection/qtmaterialswitch.h
    include/qtmaterial/widgets/selection/qtmaterialradiobutton.h
    include/qtmaterial/widgets/surfaces/qtmaterialdialog.h
    include/qtmaterial/widgets/surfaces/qtmaterialnavigationdrawer.h
    include/qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h
    include/qtmaterial/widgets/surfaces/qtmaterialbanner.h
    include/qtmaterial/widgets/surfaces/qtmaterialcard.h
    include/qtmaterial/widgets/surfaces/qtmaterialtopappbar.h
    include/qtmaterial/widgets/surfaces/qtmaterialbottomappbar.h
    include/qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h
    include/qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h
    include/qtmaterial/widgets/inputs/qtmaterialautocompletepopup.h
    include/qtmaterial/widgets/inputs/qtmaterialdatefield.h
    include/qtmaterial/widgets/data/qtmateriallistitem.h
    include/qtmaterial/widgets/data/qtmateriallist.h
    include/qtmaterial/widgets/data/qtmaterialdivider.h
)

qtm3_add_library(qtmaterial3_widgets SHARED
    ${QTMATERIAL3_WIDGETS_SOURCES}
    ${QTMATERIAL3_WIDGETS_HEADERS}
)
add_library(QtMaterial3::Widgets ALIAS qtmaterial3_widgets)

target_compile_features(qtmaterial3_widgets PUBLIC cxx_std_17)
target_compile_definitions(qtmaterial3_widgets PRIVATE QTMATERIAL3_WIDGETS_LIBRARY)
target_link_libraries(qtmaterial3_widgets
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        qtmaterial3_theme
        qtmaterial3_core
        qtmaterial3_specs
        qtmaterial3_effects
)
target_include_directories(qtmaterial3_widgets
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
set_target_properties(qtmaterial3_widgets PROPERTIES
    OUTPUT_NAME QtMaterial3Widgets
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

set(QTMATERIAL3_CMAKE_INSTALL_DIR
    ${CMAKE_INSTALL_LIBDIR}/cmake/QtMaterial3Widgets
)

install(
    TARGETS
        qtmaterial3_theme
        qtmaterial3_legacy
        qtmaterial3_core
        qtmaterial3_specs
        qtmaterial3_effects
        qtmaterial3_widgets
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
    DESTINATION ${QTMATERIAL3_CMAKE_INSTALL_DIR}
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/packaging/QtMaterial3WidgetsConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/QtMaterial3WidgetsConfig.cmake
    INSTALL_DESTINATION ${QTMATERIAL3_CMAKE_INSTALL_DIR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/QtMaterial3WidgetsConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/QtMaterial3WidgetsConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/QtMaterial3WidgetsConfigVersion.cmake
        ${CMAKE_CURRENT_SOURCE_DIR}/packaging/QtMaterial3WidgetsMacros.cmake
    DESTINATION ${QTMATERIAL3_CMAKE_INSTALL_DIR}
)

if(QTMATERIAL3_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(QTMATERIAL3_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(QTMATERIAL3_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()