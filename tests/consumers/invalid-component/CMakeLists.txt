cmake_minimum_required(VERSION 3.21)
project(qtm3_invalid_component_consumer LANGUAGES CXX)

find_package(QtMaterial3Widgets REQUIRED CONFIG COMPONENTS DefinitelyMissing)

message(FATAL_ERROR "find_package unexpectedly accepted DefinitelyMissing")
