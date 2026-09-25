set(QTMATERIAL3_INTEGRATION_SOURCES
    qtmaterialpaletteadapter.cpp
    qtmaterialproxystyle.cpp
)

set(QTMATERIAL3_INTEGRATION_HEADERS
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/integration/qtmaterialpaletteadapter.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/integration/qtmaterialproxystyle.h
)

qtm3_add_module(
    qtmaterial3_integration
    Integration
    QtMaterial3Integration
    QTMATERIAL3_INTEGRATION_LIBRARY
    ${QTMATERIAL3_INTEGRATION_SOURCES}
    ${QTMATERIAL3_INTEGRATION_HEADERS}
)

target_link_libraries(qtmaterial3_integration
    PUBLIC
        #Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        qtmaterial3_theme_model
)
