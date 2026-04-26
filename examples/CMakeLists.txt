option(QTMATERIAL3_BUILD_CONSUMER_EXAMPLES
    "Build examples that use find_package(QtMaterial3Widgets)"
    OFF
)

add_subdirectory(theme-playground)
add_subdirectory(core-playground)
add_subdirectory(gallery)
add_subdirectory(theme-studio)

if(QTMATERIAL3_BUILD_CONSUMER_EXAMPLES)
    add_subdirectory(minimal-consumer)
    add_subdirectory(install-consumer)
endif()


option(QTMATERIAL3_BUILD_THEMING_EXAMPLES "Build public theming workflow examples" ON)
if(QTMATERIAL3_BUILD_THEMING_EXAMPLES)
    add_subdirectory(theming-seed-workflow)
    add_subdirectory(theming-runtime-switch)
    add_subdirectory(theming-json-workflow)
    add_subdirectory(theming-component-overrides)
    add_subdirectory(theming-backend-report)
endif()
