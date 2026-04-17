find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Test)

qt_add_executable(benchmark_theme_switch
    theme/benchmark_theme_switch.cpp
)
target_link_libraries(benchmark_theme_switch
    PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Test qtmaterial3_theme
)

qt_add_executable(benchmark_ripple
    effects/benchmark_ripple.cpp
)
target_link_libraries(benchmark_ripple
    PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Test qtmaterial3_theme qtmaterial3_core qtmaterial3_effects
)

qt_add_executable(benchmark_widget_creation
    widgets/benchmark_widget_creation.cpp
)
target_link_libraries(benchmark_widget_creation
    PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Test qtmaterial3_theme qtmaterial3_core qtmaterial3_specs qtmaterial3_effects qtmaterial3_widgets
)

# Phase 6 placeholders for advanced widget tests/examples/benchmarks.
