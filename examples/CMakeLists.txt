function(qtmaterial3_add_example_executable target)
    if(COMMAND qt_add_executable)
        qt_add_executable(${target} ${ARGN})
    else()
        add_executable(${target} ${ARGN})
    endif()
endfunction()

qtmaterial3_add_example_executable(qtmaterial3_theme_playground
    theme-playground/main.cpp
    theme-playground/themepreviewwindow.cpp
)
target_link_libraries(qtmaterial3_theme_playground
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        qtmaterial3_theme
        qtmaterial3_core
        qtmaterial3_specs
)

qtmaterial3_add_example_executable(qtmaterial3_core_playground
    core-playground/main.cpp
    core-playground/testcontrolwidget.cpp
)
target_link_libraries(qtmaterial3_core_playground PRIVATE Qt${QT_VERSION_MAJOR}::Widgets qtmaterial3_theme qtmaterial3_core qtmaterial3_specs)

qtmaterial3_add_example_executable(qtmaterial3_gallery
    gallery/main.cpp
    gallery/gallerywindow.cpp
    gallery/pages/buttonspage.cpp
    gallery/pages/selectionpage.cpp
    gallery/pages/surfacespage.cpp
    gallery/pages/inputspage.cpp
    gallery/themecontrols/themetoolbar.cpp
    gallery/pages/navigationpage.cpp
    gallery/pages/datapage.cpp
)
target_include_directories(qtmaterial3_gallery PRIVATE gallery)
target_link_libraries(qtmaterial3_gallery
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        qtmaterial3_theme
        qtmaterial3_core
        qtmaterial3_specs
        qtmaterial3_effects
        qtmaterial3_widgets
)
