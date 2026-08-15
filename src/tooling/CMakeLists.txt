set(QTMATERIAL3_TOOLING_SOURCES
    qtmaterialcomponentregistry.cpp
)

set(QTMATERIAL3_TOOLING_HEADERS
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/tooling/qtmaterialcomponentregistry.h
)

qtm3_add_module(
    qtmaterial3_tooling
    Tooling
    QtMaterial3Tooling
    QTMATERIAL3_TOOLING_LIBRARY
    ${QTMATERIAL3_TOOLING_SOURCES}
    ${QTMATERIAL3_TOOLING_HEADERS}
)

target_link_libraries(qtmaterial3_tooling
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
)
