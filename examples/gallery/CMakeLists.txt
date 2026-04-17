find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

function(qtmaterial3_add_example_executable target)
    if(COMMAND qt_add_executable)
        qt_add_executable(${target} ${ARGN})
    else()
        add_executable(${target} ${ARGN})
    endif()
endfunction()

qtmaterial3_add_example_executable(qtmaterial3_gallery
    main.cpp
    gallerywindow.cpp
    gallerywindow.h
    pages/buttonspage.cpp
    pages/buttonspage.h
    pages/datapage.cpp
    pages/datapage.h
    pages/inputspage.cpp
    pages/inputspage.h
    pages/navigationpage.cpp
    pages/navigationpage.h
    pages/selectionpage.cpp
    pages/selectionpage.h
    pages/surfacespage.cpp
    pages/surfacespage.h
    themecontrols/themetoolbar.cpp
    themecontrols/themetoolbar.h
)

target_link_libraries(qtmaterial3_gallery
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        qtmaterial3_theme
        qtmaterial3_core
        qtmaterial3_specs
        qtmaterial3_effects
        qtmaterial3_widgets
)
