find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

function(qtmaterial3_add_example_executable target)
    if(COMMAND qt_add_executable)
        qt_add_executable(${target} ${ARGN})
    else()
        add_executable(${target} ${ARGN})
    endif()
endfunction()

qtmaterial3_add_example_executable(qtmaterial3_theme_playground
    main.cpp
    themepreviewwindow.cpp
    themepreviewwindow.h
)

target_link_libraries(qtmaterial3_theme_playground
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        qtmaterial3_theme
)