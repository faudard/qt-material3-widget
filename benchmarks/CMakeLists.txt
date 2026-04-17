find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Test)

function(qtm3_add_benchmark target)
    if(COMMAND qt_add_executable)
        qt_add_executable(${target} ${ARGN})
    else()
        add_executable(${target} ${ARGN})
    endif()
endfunction()

qtm3_add_benchmark(benchmark_theme_switch
    theme/benchmark_theme_switch.cpp
)
target_link_libraries(benchmark_theme_switch
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Test
        qtmaterial3_theme
)

qtm3_add_benchmark(benchmark_ripple
    effects/benchmark_ripple.cpp
)
target_link_libraries(benchmark_ripple
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Test
        qtmaterial3_theme
        qtmaterial3_core
        qtmaterial3_effects
)

qtm3_add_benchmark(benchmark_widget_creation
    widgets/benchmark_widget_creation.cpp
)
target_link_libraries(benchmark_widget_creation
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Test
        qtmaterial3_theme
        qtmaterial3_core
        qtmaterial3_specs
        qtmaterial3_effects
        qtmaterial3_widgets
)