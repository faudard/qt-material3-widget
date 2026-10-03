find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS UiPlugin)

add_library(qtmaterial3_designer_plugin MODULE
    qtmaterial3designercollection.cpp
    qtmaterial3designercollection.h
)

target_compile_features(qtmaterial3_designer_plugin PRIVATE cxx_std_17)

target_link_libraries(qtmaterial3_designer_plugin
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::UiPlugin
        qtmaterial3_widgets
)

target_include_directories(qtmaterial3_designer_plugin
    PRIVATE "${PROJECT_SOURCE_DIR}/include"
)

set_target_properties(qtmaterial3_designer_plugin PROPERTIES
    AUTOMOC ON
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
)

if(QTMATERIAL3_INSTALL)
    install(
        TARGETS qtmaterial3_designer_plugin
        RUNTIME DESTINATION "${QTMATERIAL3_DESIGNER_PLUGIN_INSTALL_DIR}"
        LIBRARY DESTINATION "${QTMATERIAL3_DESIGNER_PLUGIN_INSTALL_DIR}"
    )
endif()
