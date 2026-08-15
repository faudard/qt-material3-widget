cmake_minimum_required(VERSION 3.21)
project(qtm3_add_subdirectory_consumer LANGUAGES CXX)

if(NOT DEFINED QTMATERIAL3_SOURCE_DIR)
    message(FATAL_ERROR "QTMATERIAL3_SOURCE_DIR is required")
endif()

set(QTMATERIAL3_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(QTMATERIAL3_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(QTMATERIAL3_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(QTMATERIAL3_INSTALL OFF CACHE BOOL "" FORCE)
set(QTMATERIAL3_USE_MCU OFF CACHE BOOL "" FORCE)

add_subdirectory("${QTMATERIAL3_SOURCE_DIR}" qtm3)

add_executable(qtm3-add-subdirectory-consumer ../common/main.cpp)
target_compile_features(qtm3-add-subdirectory-consumer PRIVATE cxx_std_17)
target_link_libraries(qtm3-add-subdirectory-consumer PRIVATE QtMaterial3::Widgets)
