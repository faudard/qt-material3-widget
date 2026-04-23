cmake_minimum_required(VERSION 3.21)

project(qtm3_consumer LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QtMaterial3Widgets CONFIG REQUIRED)

add_executable(qtm3_consumer
    main.cpp
)

target_link_libraries(qtm3_consumer PRIVATE QtMaterial3::Widgets)

target_compile_features(qtm3_consumer PRIVATE cxx_std_17)