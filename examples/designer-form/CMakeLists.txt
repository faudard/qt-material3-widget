if(COMMAND qt_add_executable)
    qt_add_executable(qtmaterial3_designer_form_example
        main.cpp
        designerform.ui
    )
else()
    add_executable(qtmaterial3_designer_form_example
        main.cpp
        designerform.ui
    )
endif()

target_compile_features(qtmaterial3_designer_form_example PRIVATE cxx_std_17)
target_link_libraries(qtmaterial3_designer_form_example
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        qtmaterial3_widgets
)
set_target_properties(qtmaterial3_designer_form_example PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
)
