set(QTMATERIAL3_EFFECTS_SOURCES
    qtmaterialripplecontroller.cpp
    qtmaterialstatelayerpainter.cpp
    qtmaterialfocusindicator.cpp
    qtmaterialshadowrenderer.cpp
    qtmaterialshadowcache.cpp
    qtmaterialtransitioncontroller.cpp
    qtmaterialscrimwidget.cpp
)

set(QTMATERIAL3_EFFECTS_HEADERS
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/effects/qtmaterialripplecontroller.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/effects/qtmaterialstatelayerpainter.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/effects/qtmaterialfocusindicator.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/effects/qtmaterialshadowrenderer.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/effects/qtmaterialshadowcache.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/effects/qtmaterialtransitioncontroller.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/effects/qtmaterialscrimwidget.h
)

qtm3_add_module(
    qtmaterial3_effects
    Effects
    QtMaterial3Effects
    QTMATERIAL3_EFFECTS_LIBRARY
    ${QTMATERIAL3_EFFECTS_SOURCES}
    ${QTMATERIAL3_EFFECTS_HEADERS}
)

target_link_libraries(qtmaterial3_effects
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        qtmaterial3_theme
        qtmaterial3_core
)
