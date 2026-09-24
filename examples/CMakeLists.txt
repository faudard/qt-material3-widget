option(QTMATERIAL3_BUILD_CONSUMER_EXAMPLES
    "Build examples that use find_package(QtMaterial3Widgets)"
    OFF
)

add_subdirectory(gallery)
add_subdirectory(theme-studio)

if(QTMATERIAL3_BUILD_CONSUMER_EXAMPLES)
    add_subdirectory(minimal-consumer)
endif()


option(QTMATERIAL3_BUILD_THEMING_EXAMPLES "Build the public theming workflow example" ON)
if(QTMATERIAL3_BUILD_THEMING_EXAMPLES)
    add_subdirectory(theming-workflows)
endif()
