cmake_minimum_required(VERSION 3.21)

set(PROJECT_LICENSE "LGPL-3.0-only")
option(BUILD_SHARED_LIBS "Build QtMaterial3 libraries as shared libraries" ON)

project(
    qt-material3-widgets
    VERSION 0.4.0
    DESCRIPTION "Material 3 widgets for Qt Widgets"
    LANGUAGES CXX
)

set(QTMATERIAL3_IS_TOP_LEVEL ${PROJECT_IS_TOP_LEVEL})

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(QTMATERIAL3_IS_TOP_LEVEL)
    include(CTest)
    set(_qtm3_default_build_tests ${BUILD_TESTING})
else()
    set(_qtm3_default_build_tests OFF)
endif()

option(QTMATERIAL3_BUILD_TESTS "Build tests" ${_qtm3_default_build_tests})
option(QTMATERIAL3_BUILD_EXAMPLES "Build examples" ${QTMATERIAL3_IS_TOP_LEVEL})
option(QTMATERIAL3_BUILD_BENCHMARKS "Build benchmark executables" OFF)
option(QTMATERIAL3_INSTALL "Install QtMaterial3 targets and package config" ${QTMATERIAL3_IS_TOP_LEVEL})
option(QTMATERIAL3_USE_MCU "Use Material Color Utilities backend" ON)

if(QTMATERIAL3_BUILD_TESTS AND NOT QTMATERIAL3_IS_TOP_LEVEL)
    enable_testing()
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

if(COMMAND qt_standard_project_setup)
    qt_standard_project_setup()
else()
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(QtMaterial3Build)

# BEGIN QTMATERIAL3 ARCHITECTURE CONTRACTS
option(
    QTMATERIAL3_ENABLE_ARCHITECTURE_CONTRACTS
    "Enable static architecture contract checks"
    ON
)
if(QTMATERIAL3_ENABLE_ARCHITECTURE_CONTRACTS)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/QtMaterial3ArchitectureContracts.cmake")
    qtmaterial3_add_architecture_contracts()
endif()
# END QTMATERIAL3 ARCHITECTURE CONTRACTS

if(QTMATERIAL3_BUILD_TESTS)
    # QtMaterial3 public widget PIMPL contract audit.
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/QtMaterial3PimplContract.cmake" OPTIONAL)
endif()

add_subdirectory(src)

set(QTMATERIAL3_INSTALL_TARGETS
    qtmaterial3_foundation
    qtmaterial3_tooling
    qtmaterial3_testing
    qtmaterial3_theme
    qtmaterial3_integration
    qtmaterial3_core
    qtmaterial3_specs
    qtmaterial3_effects
    qtmaterial3_widgets
)

set(QTMATERIAL3_CMAKE_INSTALL_DIR
    "${CMAKE_INSTALL_LIBDIR}/cmake/QtMaterial3Widgets"
)

if(QTMATERIAL3_INSTALL)
    install(
        TARGETS ${QTMATERIAL3_INSTALL_TARGETS}
        EXPORT QtMaterial3WidgetsTargets
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )

    install(
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )

    install(
        EXPORT QtMaterial3WidgetsTargets
        FILE QtMaterial3WidgetsTargets.cmake
        NAMESPACE QtMaterial3::
        DESTINATION "${QTMATERIAL3_CMAKE_INSTALL_DIR}"
    )

    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/packaging/QtMaterial3WidgetsConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/QtMaterial3WidgetsConfig.cmake"
        INSTALL_DESTINATION "${QTMATERIAL3_CMAKE_INSTALL_DIR}"
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/QtMaterial3WidgetsConfigVersion.cmake"
        VERSION "${PROJECT_VERSION}"
        COMPATIBILITY SameMajorVersion
    )

    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/QtMaterial3WidgetsConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/QtMaterial3WidgetsConfigVersion.cmake"
            "${CMAKE_CURRENT_SOURCE_DIR}/packaging/QtMaterial3WidgetsMacros.cmake"
        DESTINATION "${QTMATERIAL3_CMAKE_INSTALL_DIR}"
    )
endif()

if(QTMATERIAL3_BUILD_TESTS)
    add_subdirectory(tests)

    # qt-material3-widget public ABI audit.
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/QtMaterial3PublicAbiAudit.cmake" OPTIONAL)
    if(COMMAND qtmaterial3_add_public_abi_audit)
        qtmaterial3_add_public_abi_audit()
    endif()

    # qt-material3-widget public header hygiene.
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/QtMaterial3PublicHeaderHygiene.cmake" OPTIONAL)
    if(COMMAND qtmaterial3_add_public_header_hygiene_tests)
        qtmaterial3_add_public_header_hygiene_tests()
    endif()
endif()


if(QTMATERIAL3_BUILD_TESTS)
    include(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/QtMaterial3ArchitectureChecks.cmake"
    )
    qtmaterial3_add_architecture_checks()
endif()

if(QTMATERIAL3_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(QTMATERIAL3_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
