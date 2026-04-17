find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

if(COMMAND qt_add_executable)
    qt_add_executable(qtmaterial3_theme_playground
        main.cpp
        themepreviewwindow.cpp
        themepreviewwindow.h
    )
else()
    add_executable(qtmaterial3_theme_playground
        main.cpp
        themepreviewwindow.cpp
        themepreviewwindow.h
    )
endif()

target_link_libraries(qtmaterial3_theme_playground
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        qtmaterial3_theme
)