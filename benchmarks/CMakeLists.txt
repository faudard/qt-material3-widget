find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Test)

function(qtmaterial3_add_benchmark_executable target)
    if(COMMAND qt_add_executable)
        qt_add_executable(${target} ${ARGN})
    else()
        add_executable(${target} ${ARGN})
    endif()
endfunction()

qtmaterial3_add_benchmark_executable(benchmark_theme_switch
    theme/benchmark_theme_switch.cpp
)
target_link_libraries(benchmark_theme_switch
    PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Test qtmaterial3_theme
)

qtmaterial3_add_benchmark_executable(benchmark_ripple
    effects/benchmark_ripple.cpp
)
target_link_libraries(benchmark_ripple
    PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Test qtmaterial3_theme qtmaterial3_core qtmaterial3_effects
)

qtmaterial3_add_benchmark_executable(benchmark_widget_creation
    widgets/benchmark_widget_creation.cpp
)
target_link_libraries(benchmark_widget_creation
    PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Test qtmaterial3_theme qtmaterial3_core qtmaterial3_specs qtmaterial3_effects qtmaterial3_widgets
)

# Slice 11: expanded performance and cache benchmarks.
if(NOT TARGET benchmark_theme_pipeline)
  qtmaterial3_add_benchmark_executable(benchmark_theme_pipeline
    theme/benchmark_theme_pipeline.cpp
  )
  target_link_libraries(benchmark_theme_pipeline PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Test
    qtmaterial3_theme
  )
endif()

if(NOT TARGET benchmark_theme_manager_fanout)
  qtmaterial3_add_benchmark_executable(benchmark_theme_manager_fanout
    theme/benchmark_theme_manager_fanout.cpp
  )
  target_link_libraries(benchmark_theme_manager_fanout PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Test
    qtmaterial3_theme
  )
endif()

if(NOT TARGET benchmark_spec_factory_pipeline)
  qtmaterial3_add_benchmark_executable(benchmark_spec_factory_pipeline
    specs/benchmark_spec_factory_pipeline.cpp
  )
  target_link_libraries(benchmark_spec_factory_pipeline PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Test
    qtmaterial3_theme
    qtmaterial3_core
    qtmaterial3_specs
  )
endif()

if(NOT TARGET benchmark_shadow_cache_pipeline)
  qtmaterial3_add_benchmark_executable(benchmark_shadow_cache_pipeline
    effects/benchmark_shadow_cache_pipeline.cpp
  )
  target_link_libraries(benchmark_shadow_cache_pipeline PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Test
    qtmaterial3_theme
    qtmaterial3_core
    qtmaterial3_effects
  )
endif()

