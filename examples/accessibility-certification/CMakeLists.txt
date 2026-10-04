find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

if(COMMAND qt_add_executable)
    qt_add_executable(qtmaterial3_accessibility_certification
        main.cpp
    )
else()
    add_executable(qtmaterial3_accessibility_certification
        main.cpp
    )
endif()

target_link_libraries(qtmaterial3_accessibility_certification
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        qtmaterial3_theme_model
        qtmaterial3_theme_runtime
        qtmaterial3_core
        qtmaterial3_specs
        qtmaterial3_effects
        qtmaterial3_widgets
)
