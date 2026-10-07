if(NOT QT_VERSION_MAJOR EQUAL 6)
    message(FATAL_ERROR "QtMaterial3 Python bindings require Qt 6")
endif()

if(CMAKE_VERSION VERSION_LESS 3.22)
    message(FATAL_ERROR "QtMaterial3 Python bindings require CMake 3.22 or newer")
endif()

find_package(Python 3.10 REQUIRED COMPONENTS Interpreter Development)

execute_process(
    COMMAND "${Python_EXECUTABLE}" -c
        "from pathlib import Path; import PySide6, shiboken6_generator; print(Path(PySide6.__file__).resolve().parent / 'Qt'); print(Path(shiboken6_generator.__file__).resolve().parent / 'lib' / 'cmake')"
    RESULT_VARIABLE _qtm3_python_prefix_result
    OUTPUT_VARIABLE _qtm3_python_prefixes
    ERROR_VARIABLE _qtm3_python_prefix_error
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT _qtm3_python_prefix_result EQUAL 0)
    message(FATAL_ERROR
        "PySide6 and shiboken6-generator are required to build bindings: "
        "${_qtm3_python_prefix_error}"
    )
endif()

string(REPLACE "\r\n" "\n" _qtm3_python_prefixes "${_qtm3_python_prefixes}")
string(REPLACE "\n" ";" _qtm3_python_prefix_list "${_qtm3_python_prefixes}")
list(PREPEND CMAKE_PREFIX_PATH ${_qtm3_python_prefix_list})

find_package(Shiboken6Tools CONFIG REQUIRED)

set(_qtm3_python_extension "_QtMaterial3")
set(_qtm3_python_generated_dir
    "${CMAKE_CURRENT_BINARY_DIR}/${_qtm3_python_extension}"
)

set(_qtm3_python_types
    WindowSizeClass
    ThemeOptions
    Theme
    ThemeBuilder
    ThemeContext
    ThemeContextHost
    QtMaterialWidget
    QtMaterialControl
    QtMaterialSurface
    QtMaterialAbstractButton
    QtMaterialSelectionControl
    QtMaterialFormFieldControl
    QtMaterialInputControl
    QtMaterialTextButton
    QtMaterialFilledButton
    QtMaterialOutlinedButton
    QtMaterialFilledTonalButton
    QtMaterialElevatedButton
    QtMaterialCheckbox
    QtMaterialSwitch
    QtMaterialOutlinedTextField
    QtMaterialFilledTextField
    QtMaterialCard
    QtMaterialNavigationBar
    QtMaterialNavigationRail
    QtMaterialLinearProgressIndicator
    QtMaterialCircularProgressIndicator
    QtMaterialLoadingIndicator
)

set(_qtm3_python_generated_sources
    "${_qtm3_python_generated_dir}/_qtmaterial3_module_wrapper.cpp"
    "${_qtm3_python_generated_dir}/qtmaterial_wrapper.cpp"
)
foreach(_qtm3_python_type IN LISTS _qtm3_python_types)
    string(TOLOWER "${_qtm3_python_type}" _qtm3_python_type_lower)
    list(APPEND _qtm3_python_generated_sources
        "${_qtm3_python_generated_dir}/qtmaterial_${_qtm3_python_type_lower}_wrapper.cpp"
    )
endforeach()

shiboken_generator_create_binding(
    EXTENSION_TARGET ${_qtm3_python_extension}
    GENERATED_SOURCES ${_qtm3_python_generated_sources}
    HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/qtmaterial3_bindings.h"
    TYPESYSTEM_FILE "${CMAKE_CURRENT_SOURCE_DIR}/typesystem_qtmaterial3.xml"
    LIBRARY_TARGET qtmaterial3_widgets
    QT_MODULES Core Gui Widgets
    FORCE_LIMITED_API
)

target_include_directories(${_qtm3_python_extension}
    PRIVATE "${PROJECT_SOURCE_DIR}/include"
)
target_link_libraries(${_qtm3_python_extension}
    PRIVATE
        qtmaterial3_widgets
        qtmaterial3_theme_runtime
        qtmaterial3_theme_model
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)

set(_qtm3_python_package_dir "${CMAKE_CURRENT_BINARY_DIR}/QtMaterial3")
file(MAKE_DIRECTORY "${_qtm3_python_package_dir}")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/QtMaterial3/__init__.py"
    "${_qtm3_python_package_dir}/__init__.py"
    COPYONLY
)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/QtMaterial3/Widgets.py"
    "${_qtm3_python_package_dir}/Widgets.py"
    COPYONLY
)

set_target_properties(${_qtm3_python_extension} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${_qtm3_python_package_dir}"
    RUNTIME_OUTPUT_DIRECTORY "${_qtm3_python_package_dir}"
)

install(
    TARGETS ${_qtm3_python_extension}
    LIBRARY DESTINATION QtMaterial3
    RUNTIME DESTINATION QtMaterial3
)
install(
    FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/QtMaterial3/__init__.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/QtMaterial3/Widgets.py"
    DESTINATION QtMaterial3
)

if(QTMATERIAL3_BUILD_TESTS)
    add_test(
        NAME python_bindings_contract
        COMMAND "${Python_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_bindings.py"
    )
    set_tests_properties(python_bindings_contract PROPERTIES
        ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR};QT_QPA_PLATFORM=offscreen"
    )
endif()
