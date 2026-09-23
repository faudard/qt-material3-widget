find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

function(qtmaterial3_add_example_executable target)
    if(COMMAND qt_add_executable)
        qt_add_executable(${target} ${ARGN})
    else()
        add_executable(${target} ${ARGN})
    endif()
endfunction()

qtmaterial3_add_example_executable(
    qtmaterial3_core_playground
    main.cpp
    testcontrolwidget.cpp
    testcontrolwidget.h
)

target_link_libraries(qtmaterial3_core_playground PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    qtmaterial3_theme_model qtmaterial3_theme_io qtmaterial3_theme_runtime
    qtmaterial3_core
    qtmaterial3_specs
    qtmaterial3_widgets
)

if(WIN32 AND QTMATERIAL3_IS_TOP_LEVEL
        AND NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
endif()