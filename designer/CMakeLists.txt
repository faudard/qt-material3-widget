find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS UiPlugin)
find_package(Qt${QT_VERSION_MAJOR} QUIET COMPONENTS Designer)

set(_qtm3_designer_plugin_sources
    qtmaterial3designercollection.cpp
    qtmaterial3designercollection.h
)

if(TARGET Qt${QT_VERSION_MAJOR}::Designer)
    list(APPEND _qtm3_designer_plugin_sources
        qtmaterial3designerextensions.cpp
        qtmaterial3designerextensions.h
    )
endif()

add_library(qtmaterial3_designer_plugin MODULE
    ${_qtm3_designer_plugin_sources}
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

if(TARGET Qt${QT_VERSION_MAJOR}::Designer)
    target_link_libraries(qtmaterial3_designer_plugin
        PRIVATE Qt${QT_VERSION_MAJOR}::Designer
    )
    target_compile_definitions(qtmaterial3_designer_plugin
        PRIVATE QTMATERIAL3_HAS_DESIGNER_EXTENSIONS=1
    )
else()
    message(STATUS
        "Qt Designer development API not found: building palette-only plugin; "
        "task menus, theme previews and custom container extensions are disabled"
    )
endif()

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
        COMPONENT Designer
    )
    install(
        FILES "${PROJECT_SOURCE_DIR}/docs/designer-plugin.md"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/qt-material3-widget/designer"
        COMPONENT Designer
    )
endif()

if(QTMATERIAL3_BUILD_TESTS)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)
    add_executable(tst_qtmaterial3_designer_collection
        tst_designercollection.cpp
        qtmaterial3designercollection.cpp
        qtmaterial3designercollection.h
    )
    target_link_libraries(tst_qtmaterial3_designer_collection
        PRIVATE
            Qt${QT_VERSION_MAJOR}::Test
            Qt${QT_VERSION_MAJOR}::UiPlugin
            qtmaterial3_widgets
    )
    target_include_directories(tst_qtmaterial3_designer_collection
        PRIVATE "${PROJECT_SOURCE_DIR}/include"
    )
    set_target_properties(tst_qtmaterial3_designer_collection PROPERTIES AUTOMOC ON)
    add_test(NAME designer_collection_contract COMMAND tst_qtmaterial3_designer_collection)

    add_executable(tst_qtmaterial3_designer_ui_smoke
        designer_smoke.cpp
        designer_smoke.ui
    )
    target_link_libraries(tst_qtmaterial3_designer_ui_smoke
        PRIVATE
            Qt${QT_VERSION_MAJOR}::Widgets
            qtmaterial3_widgets
    )
    target_include_directories(tst_qtmaterial3_designer_ui_smoke
        PRIVATE "${PROJECT_SOURCE_DIR}/include"
    )
    set_target_properties(tst_qtmaterial3_designer_ui_smoke PROPERTIES AUTOUIC ON)
    add_test(NAME designer_ui_smoke COMMAND tst_qtmaterial3_designer_ui_smoke)

    # 1.19.2: AUTOUIC must deserialize native Material declarations for
    # every supported Qt widget kind (including opt-out and no declaration).
    add_executable(tst_qtmaterial3_native_roundtrip
        tst_native_roundtrip.cpp
        native_roundtrip.ui
    )
    target_link_libraries(tst_qtmaterial3_native_roundtrip
        PRIVATE
            Qt${QT_VERSION_MAJOR}::Test
            Qt${QT_VERSION_MAJOR}::Widgets
            qtmaterial3_widgets
    )
    target_include_directories(tst_qtmaterial3_native_roundtrip
        PRIVATE "${PROJECT_SOURCE_DIR}/include"
    )
    set_target_properties(tst_qtmaterial3_native_roundtrip PROPERTIES
        AUTOMOC ON
        AUTOUIC ON
    )
    add_test(NAME designer_native_roundtrip COMMAND tst_qtmaterial3_native_roundtrip)

    if(TARGET Qt${QT_VERSION_MAJOR}::Designer)
        add_executable(tst_qtmaterial3_designer_extensions
            tst_designerextensions.cpp
            qtmaterial3designerextensions.cpp
            qtmaterial3designerextensions.h
        )
        target_link_libraries(tst_qtmaterial3_designer_extensions
            PRIVATE
                Qt${QT_VERSION_MAJOR}::Test
                Qt${QT_VERSION_MAJOR}::Designer
                qtmaterial3_widgets
        )
        target_include_directories(tst_qtmaterial3_designer_extensions
            PRIVATE "${PROJECT_SOURCE_DIR}/include"
        )
        set_target_properties(tst_qtmaterial3_designer_extensions PROPERTIES AUTOMOC ON)
        add_test(NAME designer_extensions_contract COMMAND tst_qtmaterial3_designer_extensions)
    endif()
endif()
