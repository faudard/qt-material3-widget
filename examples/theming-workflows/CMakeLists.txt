find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui)

if(COMMAND qt_add_executable)
    qt_add_executable(qtmaterial3_theming_workflows main.cpp)
else()
    add_executable(qtmaterial3_theming_workflows main.cpp)
endif()

target_compile_features(qtmaterial3_theming_workflows PRIVATE cxx_std_17)

target_link_libraries(qtmaterial3_theming_workflows
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        qtmaterial3_theme_model
        qtmaterial3_theme_io
        qtmaterial3_theme_runtime
)
