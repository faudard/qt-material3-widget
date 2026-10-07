cmake_minimum_required(VERSION 3.21)
project(qtm3_conan_test LANGUAGES CXX)

find_package(QtMaterial3Widgets CONFIG REQUIRED COMPONENTS Widgets)

add_executable(qtm3-conan-test main.cpp)
target_compile_features(qtm3-conan-test PRIVATE cxx_std_17)
target_link_libraries(qtm3-conan-test PRIVATE QtMaterial3::Widgets)
