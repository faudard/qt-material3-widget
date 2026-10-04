option(QTMATERIAL3_BUILD_CONSUMER_EXAMPLES
    "Build examples that use find_package(QtMaterial3Widgets)"
    OFF
)

add_subdirectory(gallery)
add_subdirectory(dashboard-demo)
add_subdirectory(theme-studio)
add_subdirectory(designer-form)
add_subdirectory(accessibility-certification)
add_subdirectory(material3-catalogue-certification)
add_subdirectory(adaptive-desktop-certification)

if(QTMATERIAL3_BUILD_CONSUMER_EXAMPLES)
    add_subdirectory(minimal-consumer)
endif()


option(QTMATERIAL3_BUILD_THEMING_EXAMPLES "Build the public theming workflow example" ON)
if(QTMATERIAL3_BUILD_THEMING_EXAMPLES)
    add_subdirectory(theming-workflows)
endif()
