option(QTMATERIAL3_BUILD_CONSUMER_EXAMPLES
    "Build examples that use find_package(QtMaterial3Widgets)"
    OFF
)

add_subdirectory(theme-playground)
add_subdirectory(core-playground)
add_subdirectory(gallery)

if(QTMATERIAL3_BUILD_CONSUMER_EXAMPLES)
    add_subdirectory(minimal-consumer)
    add_subdirectory(install-consumer)
endif()
