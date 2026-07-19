cmake_minimum_required(VERSION 3.21)

project(qtm3_consumer LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QtMaterial3Widgets CONFIG REQUIRED COMPONENTS
    Foundation Theme Core Specs Effects Widgets
    Integration Tooling Testing
)

add_executable(qtm3_consumer
    main.cpp
)

target_link_libraries(qtm3_consumer PRIVATE
    QtMaterial3::Widgets
    QtMaterial3::Integration
    QtMaterial3::Tooling
    QtMaterial3::Testing
)

target_compile_features(qtm3_consumer PRIVATE cxx_std_17)