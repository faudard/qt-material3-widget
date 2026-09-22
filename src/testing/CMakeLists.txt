set(QTMATERIAL3_TESTING_SOURCES
    qtmaterialautomation.cpp
)

set(QTMATERIAL3_TESTING_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/qtmaterialautomation.h
)

qtm3_add_module(
    qtmaterial3_testing
    Testing
    QtMaterial3Testing
    QTMATERIAL3_TESTING_LIBRARY
    ${QTMATERIAL3_TESTING_SOURCES}
    ${QTMATERIAL3_TESTING_HEADERS}
)

target_link_libraries(
    qtmaterial3_testing
    PUBLIC
        qtmaterial3_foundation
        Qt${QT_VERSION_MAJOR}::Core
)

target_include_directories(qtmaterial3_testing
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)
