cmake_minimum_required(VERSION 3.21)
project(qtmaterial3_install_consumer LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)
find_package(QtMaterial3Widgets REQUIRED CONFIG)

if(COMMAND qt_standard_project_setup)
    qt_standard_project_setup()
else()
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
endif()

if(COMMAND qt_add_executable)
    qt_add_executable(install-consumer
        main.cpp
    )
else()
    add_executable(install-consumer
        main.cpp
    )
endif()

target_link_libraries(install-consumer
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        QtMaterial3::Widgets
)
