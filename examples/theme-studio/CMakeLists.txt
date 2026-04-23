find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

function(qtmaterial3_add_example_executable target)
    if(COMMAND qt_add_executable)
        qt_add_executable(${target} ${ARGN})
    else()
        add_executable(${target} ${ARGN})
    endif()
endfunction()

qtmaterial3_add_example_executable(qtmaterial3_theme_studio
    main.cpp
    themestudiowindow.cpp
    themestudiowindow.h
    themestudiocontroller.cpp
    themestudiocontroller.h
    themeeditorpanel.cpp
    themeeditorpanel.h
    themepreviewpane.cpp
    themepreviewpane.h
    themecolorrolesview.cpp
    themecolorrolesview.h
    themejsonview.cpp
    themejsonview.h
    themepresetcatalog.cpp
    themepresetcatalog.h
    themecompareview.cpp
    themecompareview.h
)

target_link_libraries(qtmaterial3_theme_studio PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    qtmaterial3_theme
    qtmaterial3_core
    qtmaterial3_specs
    qtmaterial3_effects
    qtmaterial3_widgets
)
