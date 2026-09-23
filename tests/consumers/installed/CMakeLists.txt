cmake_minimum_required(VERSION 3.21)
project(qtm3_installed_consumer LANGUAGES CXX)

find_package(QtMaterial3Widgets REQUIRED CONFIG COMPONENTS ThemeModel Widgets)

if(NOT TARGET QtMaterial3::ThemeModel)
    message(FATAL_ERROR "QtMaterial3::ThemeModel is missing from installed package")
endif()
if(NOT TARGET QtMaterial3::Widgets)
    message(FATAL_ERROR "QtMaterial3::Widgets is missing from installed package")
endif()

add_executable(qtm3-installed-consumer ../common/main.cpp)
target_compile_features(qtm3-installed-consumer PRIVATE cxx_std_17)
target_link_libraries(qtm3-installed-consumer PRIVATE QtMaterial3::Widgets)
