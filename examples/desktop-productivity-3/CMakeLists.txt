find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

if(COMMAND qt_add_executable)
    qt_add_executable(qtmaterial3_desktop_productivity_3 main.cpp)
else()
    add_executable(qtmaterial3_desktop_productivity_3 main.cpp)
endif()

target_link_libraries(qtmaterial3_desktop_productivity_3
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets qtmaterial3_widgets)
