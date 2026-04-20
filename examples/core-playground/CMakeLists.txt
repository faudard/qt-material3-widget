find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

function(qtmaterial3_add_example_executable target)
    if(COMMAND qt_add_executable)
        qt_add_executable(${target} ${ARGN})
    else()
        add_executable(${target} ${ARGN})
    endif()
endfunction()

qtmaterial3_add_example_executable(qtmaterial3_core_playground
    main.cpp
    testcontrolwidget.cpp
    testcontrolwidget.h
)

target_link_libraries(qtmaterial3_core_playground
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        qtmaterial3_theme
        qtmaterial3_core
        qtmaterial3_specs
)