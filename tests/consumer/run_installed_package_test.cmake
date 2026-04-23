if(NOT DEFINED REPO_BINARY_DIR OR REPO_BINARY_DIR STREQUAL "")
    message(FATAL_ERROR "REPO_BINARY_DIR is required")
endif()

if(NOT DEFINED TEST_SOURCE_DIR OR TEST_SOURCE_DIR STREQUAL "")
    message(FATAL_ERROR "TEST_SOURCE_DIR is required")
endif()

if(NOT DEFINED TEST_BINARY_DIR OR TEST_BINARY_DIR STREQUAL "")
    message(FATAL_ERROR "TEST_BINARY_DIR is required")
endif()

if(NOT DEFINED INSTALL_PREFIX OR INSTALL_PREFIX STREQUAL "")
    message(FATAL_ERROR "INSTALL_PREFIX is required")
endif()

set(_config "${BUILD_CONFIG}")
if(_config STREQUAL "")
    set(_config "${DEFAULT_BUILD_CONFIG}")
endif()
if(_config STREQUAL "")
    set(_config "Release")
endif()

file(REMOVE_RECURSE "${INSTALL_PREFIX}")
file(REMOVE_RECURSE "${TEST_BINARY_DIR}")

set(_install_cmd
    "${CMAKE_COMMAND}" --install "${REPO_BINARY_DIR}" --prefix "${INSTALL_PREFIX}"
)
if(NOT _config STREQUAL "")
    list(APPEND _install_cmd --config "${_config}")
endif()

execute_process(
    COMMAND ${_install_cmd}
    RESULT_VARIABLE _install_result
)
if(NOT _install_result EQUAL 0)
    message(FATAL_ERROR "Install step failed with code ${_install_result}")
endif()

set(_configure_cmd
    "${CMAKE_COMMAND}"
    -S "${TEST_SOURCE_DIR}"
    -B "${TEST_BINARY_DIR}"
    -DCMAKE_PREFIX_PATH=${INSTALL_PREFIX}
)

if(DEFINED GENERATOR_NAME AND NOT GENERATOR_NAME STREQUAL "")
    list(APPEND _configure_cmd -G "${GENERATOR_NAME}")
endif()

if(DEFINED GENERATOR_PLATFORM AND NOT GENERATOR_PLATFORM STREQUAL "")
    list(APPEND _configure_cmd -A "${GENERATOR_PLATFORM}")
endif()

if(DEFINED GENERATOR_TOOLSET AND NOT GENERATOR_TOOLSET STREQUAL "")
    list(APPEND _configure_cmd -T "${GENERATOR_TOOLSET}")
endif()

if(NOT _config STREQUAL "")
    list(APPEND _configure_cmd -DCMAKE_BUILD_TYPE=${_config})
endif()

execute_process(
    COMMAND ${_configure_cmd}
    RESULT_VARIABLE _configure_result
)
if(NOT _configure_result EQUAL 0)
    message(FATAL_ERROR "Consumer configure step failed with code ${_configure_result}")
endif()

set(_build_cmd
    "${CMAKE_COMMAND}" --build "${TEST_BINARY_DIR}"
)
if(NOT _config STREQUAL "")
    list(APPEND _build_cmd --config "${_config}")
endif()

execute_process(
    COMMAND ${_build_cmd}
    RESULT_VARIABLE _build_result
)
if(NOT _build_result EQUAL 0)
    message(FATAL_ERROR "Consumer build step failed with code ${_build_result}")
endif()