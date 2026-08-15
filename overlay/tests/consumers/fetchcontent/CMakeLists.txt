cmake_minimum_required(VERSION 3.21)
project(qtm3_fetchcontent_consumer LANGUAGES CXX)

include(FetchContent)

if(NOT DEFINED QTMATERIAL3_SOURCE_DIR)
    message(FATAL_ERROR "QTMATERIAL3_SOURCE_DIR is required")
endif()

set(QTMATERIAL3_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(QTMATERIAL3_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(QTMATERIAL3_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(QTMATERIAL3_INSTALL OFF CACHE BOOL "" FORCE)
set(QTMATERIAL3_USE_MCU OFF CACHE BOOL "" FORCE)

FetchContent_Declare(QtMaterial3 SOURCE_DIR "${QTMATERIAL3_SOURCE_DIR}")
FetchContent_MakeAvailable(QtMaterial3)

add_executable(qtm3-fetchcontent-consumer ../common/main.cpp)
target_compile_features(qtm3-fetchcontent-consumer PRIVATE cxx_std_17)
target_link_libraries(qtm3-fetchcontent-consumer PRIVATE QtMaterial3::Widgets)
