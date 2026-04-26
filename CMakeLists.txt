cmake_minimum_required(VERSION 3.21)

option(BUILD_SHARED_LIBS "Build QtMaterial3 libraries as shared libraries" ON)

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

function(qtm3_add_library target)
    if(BUILD_SHARED_LIBS)
        set(_qtm3_library_type SHARED)
    else()
        set(_qtm3_library_type STATIC)
    endif()

    if(COMMAND qt_add_library)
        qt_add_library(${target} ${_qtm3_library_type} ${ARGN})
    else()
        add_library(${target} ${_qtm3_library_type} ${ARGN})
    endif()

    target_compile_features(${target} PUBLIC cxx_std_17)

    if(NOT BUILD_SHARED_LIBS)
        target_compile_definitions(${target} PUBLIC QTMATERIAL3_STATIC)
    endif()

    set_target_properties(${target} PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
endfunction()

set(QTMATERIAL3_THEME_SOURCES
    src/theme/qtmaterialcolorscheme.cpp
    src/theme/qtmaterialthemeoptions.cpp
    src/theme/qtmaterialtheme.cpp
    src/theme/qtmaterialthemebuilder.cpp
    src/theme/qtmaterialcolorbackend.cpp
    src/theme/qtmaterialmcuadapter.cpp
    src/theme/qtmaterialthememanager.cpp
    src/theme/qtmaterialthemeserializer.cpp
    src/theme/qtmaterialthemeobserver.cpp
    src/theme/qtmaterialtypographyscale.cpp
    src/theme/qtmaterialshapescale.cpp
    src/theme/qtmaterialelevationscale.cpp
    src/theme/qtmaterialmotiontokens.cpp
    src/theme/qtmaterialstatelayer.cpp
    src/theme/qtmaterialdensitytokens.cpp
    src/theme/qtmaterialiconsizetokens.cpp
    src/theme/qtmaterialcomponenttokens.cpp
    src/theme/qtmaterialxmlthemeadapter.cpp
)

set(QTMATERIAL3_THEME_HEADERS
    include/qtmaterial/qtmaterialglobal.h
    include/qtmaterial/theme/qtmaterialcolortoken.h
    include/qtmaterial/theme/qtmaterialcolorbackend.h
    include/qtmaterial/theme/qtmaterialcolorscheme.h
    include/qtmaterial/theme/qtmaterialthemeoptions.h
    include/qtmaterial/theme/qtmaterialtheme.h
    include/qtmaterial/theme/qtmaterialthemebuilder.h
    include/qtmaterial/theme/qtmaterialthememanager.h
    include/qtmaterial/theme/qtmaterialthemeserializer.h
    include/qtmaterial/theme/qtmaterialthemeobserver.h
    include/qtmaterial/theme/qtmaterialtypographyscale.h
    include/qtmaterial/theme/qtmaterialshapescale.h
    include/qtmaterial/theme/qtmaterialelevationscale.h
    include/qtmaterial/theme/qtmaterialmotiontokens.h
    include/qtmaterial/theme/qtmaterialstatelayer.h
    include/qtmaterial/theme/qtmaterialdensitytokens.h
    include/qtmaterial/theme/qtmaterialiconsizetokens.h
    include/qtmaterial/theme/qtmaterialcomponenttokens.h
    include/qtmaterial/theme/qtmaterialxmlthemeadapter.h
)

qtm3_add_library(qtmaterial3_theme
    ${QTMATERIAL3_THEME_SOURCES}
    ${QTMATERIAL3_THEME_HEADERS}
)
add_library(QtMaterial3::Theme ALIAS qtmaterial3_theme)

target_compile_features(qtmaterial3_theme PUBLIC cxx_std_17)
target_compile_definitions(qtmaterial3_theme PRIVATE QTMATERIAL3_THEME_LIBRARY)
# QTMATERIAL3_USE_MCU expresses the user request. QTMATERIAL3_HAS_MCU expresses
# whether an actual MCU adapter is compiled into this target.
set(QTMATERIAL3_MCU_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/material-color-utilities"
    CACHE PATH
    "Path to the Material Color Utilities checkout containing the cpp/ directory"
)
set(QTMATERIAL3_HAS_MCU OFF)

if(QTMATERIAL3_USE_MCU)
    set(QTMATERIAL3_MCU_REQUIRED_HEADERS
        "${QTMATERIAL3_MCU_ROOT}/cpp/cam/hct.h"
        "${QTMATERIAL3_MCU_ROOT}/cpp/dynamiccolor/material_dynamic_colors.h"
        "${QTMATERIAL3_MCU_ROOT}/cpp/scheme/scheme_tonal_spot.h"
        "${QTMATERIAL3_MCU_ROOT}/cpp/scheme/scheme_expressive.h"
        "${QTMATERIAL3_MCU_ROOT}/cpp/utils/utils.h"
    )
    set(QTMATERIAL3_MCU_HEADERS_PRESENT ON)
    foreach(_qtm3_mcu_header IN LISTS QTMATERIAL3_MCU_REQUIRED_HEADERS)
        if(NOT EXISTS "${_qtm3_mcu_header}")
            set(QTMATERIAL3_MCU_HEADERS_PRESENT OFF)
        endif()
    endforeach()

    if(QTMATERIAL3_MCU_HEADERS_PRESENT)
        file(GLOB_RECURSE QTMATERIAL3_MCU_SOURCES CONFIGURE_DEPENDS
            "${QTMATERIAL3_MCU_ROOT}/cpp/*.cc"
            "${QTMATERIAL3_MCU_ROOT}/cpp/*.cpp"
        )
        list(FILTER QTMATERIAL3_MCU_SOURCES EXCLUDE REGEX ".*(_test|test)\.(cc|cpp)$")

        if(QTMATERIAL3_MCU_SOURCES)
            add_library(qtm3_material_color_utilities STATIC ${QTMATERIAL3_MCU_SOURCES})
            target_compile_features(qtm3_material_color_utilities PUBLIC cxx_std_17)
            target_include_directories(qtm3_material_color_utilities PUBLIC "${QTMATERIAL3_MCU_ROOT}")
            set_target_properties(qtm3_material_color_utilities PROPERTIES POSITION_INDEPENDENT_CODE ON)
            set(QTMATERIAL3_HAS_MCU ON)
        else()
            message(WARNING "QTMATERIAL3_USE_MCU=ON but no MCU C++ sources were found under ${QTMATERIAL3_MCU_ROOT}/cpp; using fallback backend.")
        endif()
    else()
        message(WARNING "QTMATERIAL3_USE_MCU=ON but Material Color Utilities headers were not found at ${QTMATERIAL3_MCU_ROOT}; using fallback backend.")
    endif()
endif()

target_compile_definitions(qtmaterial3_theme
    PUBLIC
        QTMATERIAL3_USE_MCU=$<BOOL:${QTMATERIAL3_USE_MCU}>
        QTMATERIAL3_HAS_MCU=$<BOOL:${QTMATERIAL3_HAS_MCU}>
)

if(TARGET qtm3_material_color_utilities)
    target_link_libraries(qtmaterial3_theme PRIVATE qtm3_material_color_utilities)
    target_include_directories(qtmaterial3_theme PRIVATE "${QTMATERIAL3_MCU_ROOT}")
endif()
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
    EXPORT_NAME Theme
    OUTPUT_NAME QtMaterial3Theme
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
    src/core/private/qtmaterialaccessibilityhelper.cpp
)

set(QTMATERIAL3_CORE_HEADERS
    include/qtmaterial/core/qtmaterialdensity.h
    include/qtmaterial/core/qtmaterialwidget.h
    include/qtmaterial/core/qtmaterialinteractionstate.h
    include/qtmaterial/core/qtmaterialeventcompat.h
    include/qtmaterial/core/qtmaterialcontrol.h
    include/qtmaterial/core/qtmaterialsurface.h
    include/qtmaterial/core/qtmaterialoverlaysurface.h
    include/qtmaterial/core/qtmaterialabstractbutton.h
    include/qtmaterial/core/qtmaterialselectioncontrol.h
    include/qtmaterial/core/qtmaterialinputcontrol.h
)

qtm3_add_library(qtmaterial3_core
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
    EXPORT_NAME Core
    OUTPUT_NAME QtMaterial3Core
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

set(QTMATERIAL3_SPECS_SOURCES
    src/specs/qtmaterialspecfactory.cpp
    src/specs/qtmaterialcomponenttokenapplier.cpp
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
    src/specs/qtmaterialtabsspec.cpp
    src/specs/qtmaterialsnackbarspec.cpp
    src/specs/qtmaterialprogressindicatorspec.cpp
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
    include/qtmaterial/specs/qtmaterialtabsspec.h
    include/qtmaterial/specs/qtmaterialspecfactory.h
    include/qtmaterial/specs/qtmaterialsnackbarspec.h
    include/qtmaterial/specs/qtmaterialprogressindicatorspec.h
)

qtm3_add_library(qtmaterial3_specs
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
    EXPORT_NAME Specs
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

qtm3_add_library(qtmaterial3_effects
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
    EXPORT_NAME Effects
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
    src/widgets/selection/private/qtmaterialselectionrenderhelper.cpp
    src/widgets/surfaces/private/qtmaterialsurfacerenderhelper.cpp
    src/widgets/inputs/private/qtmaterialtextfieldshellhelper_p.cpp
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
    src/widgets/navigation/qtmaterialtabs.cpp
    src/widgets/surfaces/qtmaterialsnackbar.cpp
    src/widgets/surfaces/qtmaterialsnackbarhost.cpp
    src/widgets/progress/qtmateriallinearprogressindicator.cpp
    src/widgets/progress/qtmaterialcircularprogressindicator.cpp
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
    include/qtmaterial/widgets/navigation/qtmaterialnavigationcontroller.h
    include/qtmaterial/widgets/navigation/qtmaterialtabs.h
    include/qtmaterial/widgets/data/qtmaterialdivider.h
    include/qtmaterial/widgets/surfaces/qtmaterialsnackbar.h
    include/qtmaterial/widgets/surfaces/qtmaterialsnackbarhost.h
    include/qtmaterial/widgets/progress/qtmateriallinearprogressindicator.h
    include/qtmaterial/widgets/progress/qtmaterialcircularprogressindicator.h
)

qtm3_add_library(qtmaterial3_widgets
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
    EXPORT_NAME Widgets
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