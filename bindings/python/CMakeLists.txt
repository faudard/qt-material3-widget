if(NOT QT_VERSION_MAJOR EQUAL 6)
    message(FATAL_ERROR "QtMaterial3 Python bindings require Qt 6")
endif()

if(CMAKE_VERSION VERSION_LESS 3.22)
    message(FATAL_ERROR "QtMaterial3 Python bindings require CMake 3.22 or newer")
endif()

find_package(Python 3.10 REQUIRED COMPONENTS Interpreter Development)

set(_qtm3_pyside_config "${CMAKE_CURRENT_SOURCE_DIR}/pyside_config.py")

function(_qtm3_pyside_query option output_var)
    execute_process(
        COMMAND "${Python_EXECUTABLE}" "${_qtm3_pyside_config}" "${option}"
        RESULT_VARIABLE _qtm3_query_result
        OUTPUT_VARIABLE _qtm3_query_output
        ERROR_VARIABLE _qtm3_query_error
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(NOT _qtm3_query_result EQUAL 0 OR "${_qtm3_query_output}" STREQUAL "")
        message(FATAL_ERROR
            "Qt for Python discovery failed for ${option}: ${_qtm3_query_error}")
    endif()
    set(${output_var} "${_qtm3_query_output}" PARENT_SCOPE)
endfunction()

_qtm3_pyside_query("--shiboken-module-path" _qtm3_shiboken_module_path)
_qtm3_pyside_query("--shiboken-generator-path" _qtm3_shiboken_generator_path)
_qtm3_pyside_query("--pyside-path" _qtm3_pyside_path)
_qtm3_pyside_query("--python-include-path" _qtm3_python_include_dir)
_qtm3_pyside_query("--shiboken-generator-include-path" _qtm3_shiboken_include_dir)
_qtm3_pyside_query("--pyside-include-path" _qtm3_pyside_include_dir)
_qtm3_pyside_query("--shiboken-module-shared-libraries-cmake" _qtm3_shiboken_libraries)
_qtm3_pyside_query("--pyside-shared-libraries-cmake" _qtm3_pyside_libraries)
_qtm3_pyside_query("--python-extension-suffix" _qtm3_python_extension_suffix)

set(_qtm3_shiboken_executable
    "${_qtm3_shiboken_generator_path}/shiboken6${CMAKE_EXECUTABLE_SUFFIX}")
if(NOT EXISTS "${_qtm3_shiboken_executable}")
    message(FATAL_ERROR
        "Shiboken generator executable not found: ${_qtm3_shiboken_executable}")
endif()

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
    QtMaterialWidget
    QtMaterialControl
    QtMaterialSurface
    QtMaterialOverlaySurface
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
    QtMaterialRadioButton
    QtMaterialSwitch
    QtMaterialChip
    QtMaterialTooltip
    QtMaterialOutlinedTextField
    QtMaterialFilledTextField
    QtMaterialCard
    QtMaterialDialog
    QtMaterialSnackbar
    QtMaterialBadge
    QtMaterialPagination
    QtMaterialDivider
    QtMaterialNavigationBar
    QtMaterialNavigationRail
    QtMaterialRoute
    QtMaterialNavigationItem
    QtMaterialNavigationModel
    QtMaterialTabs
    QtMaterialLinearProgressIndicator
    QtMaterialCircularProgressIndicator
    QtMaterialLoadingIndicator
)

set(_qtm3_python_generated_sources
    "${_qtm3_python_generated_dir}/_qtmaterial3_module_wrapper.cpp"
    "${_qtm3_python_generated_dir}/qtmaterial_wrapper.cpp"
    "${_qtm3_python_generated_dir}/qtmaterialmenu_wrapper.cpp"
)
foreach(_qtm3_python_type IN LISTS _qtm3_python_types)
    string(TOLOWER "${_qtm3_python_type}" _qtm3_python_type_lower)
    list(APPEND _qtm3_python_generated_sources
        "${_qtm3_python_generated_dir}/qtmaterial_${_qtm3_python_type_lower}_wrapper.cpp"
    )
endforeach()

set(_qtm3_shiboken_include_options)
foreach(_qtm3_qt_target IN ITEMS Qt6::Core Qt6::Gui Qt6::Widgets)
    get_target_property(_qtm3_qt_includes
        ${_qtm3_qt_target} INTERFACE_INCLUDE_DIRECTORIES)
    foreach(_qtm3_include IN LISTS _qtm3_qt_includes)
        if(_qtm3_include AND NOT _qtm3_include MATCHES "^\\$<")
            list(APPEND _qtm3_shiboken_include_options "-I${_qtm3_include}")
        endif()
    endforeach()
endforeach()
list(REMOVE_DUPLICATES _qtm3_shiboken_include_options)

set(_qtm3_shiboken_options
    --generator-set=shiboken
    --enable-parent-ctor-heuristic
    --enable-pyside-extensions
    --enable-return-value-heuristic
    --use-isnull-as-nb_nonzero
    --avoid-protected-hack
    ${_qtm3_shiboken_include_options}
    "-I${PROJECT_SOURCE_DIR}/include"
    "-T${CMAKE_CURRENT_SOURCE_DIR}"
    "-T${_qtm3_pyside_path}/typesystems"
    "--output-directory=${CMAKE_CURRENT_BINARY_DIR}"
)

set(_qtm3_binding_header "${CMAKE_CURRENT_SOURCE_DIR}/qtmaterial3_bindings.h")
set(_qtm3_typesystem "${CMAKE_CURRENT_SOURCE_DIR}/typesystem_qtmaterial3.xml")

add_custom_command(
    OUTPUT ${_qtm3_python_generated_sources}
    COMMAND "${_qtm3_shiboken_executable}"
            ${_qtm3_shiboken_options}
            "${_qtm3_binding_header}"
            "${_qtm3_typesystem}"
    DEPENDS
        "${_qtm3_binding_header}"
        "${_qtm3_typesystem}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Generating QtMaterial3 Python bindings with Shiboken6"
    VERBATIM
)

add_library(${_qtm3_python_extension} MODULE ${_qtm3_python_generated_sources})

set(_qtm3_pyside_additional_includes)
foreach(_qtm3_pyside_include IN LISTS _qtm3_pyside_include_dir)
    list(APPEND _qtm3_pyside_additional_includes
        "${_qtm3_pyside_include}/QtCore"
        "${_qtm3_pyside_include}/QtGui"
        "${_qtm3_pyside_include}/QtWidgets"
    )
endforeach()

target_include_directories(${_qtm3_python_extension}
    PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        "${_qtm3_python_include_dir}"
        "${_qtm3_shiboken_include_dir}"
        "${_qtm3_pyside_include_dir}"
        ${_qtm3_pyside_additional_includes}
)

target_link_libraries(${_qtm3_python_extension}
    PRIVATE
        qtmaterial3_widgets
        qtmaterial3_theme_runtime
        qtmaterial3_theme_model
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        ${_qtm3_pyside_libraries}
        ${_qtm3_shiboken_libraries}
)

set(_qtm3_python_package_dir "${CMAKE_CURRENT_BINARY_DIR}/QtMaterial3")
file(MAKE_DIRECTORY "${_qtm3_python_package_dir}")

set_target_properties(${_qtm3_python_extension} PROPERTIES
    PREFIX ""
    SUFFIX "${_qtm3_python_extension_suffix}"
    LIBRARY_OUTPUT_DIRECTORY "${_qtm3_python_package_dir}"
    RUNTIME_OUTPUT_DIRECTORY "${_qtm3_python_package_dir}"
)

if(UNIX AND NOT APPLE)
    set_target_properties(${_qtm3_python_extension} PROPERTIES
        BUILD_RPATH "${_qtm3_shiboken_module_path};${_qtm3_pyside_path}"
        INSTALL_RPATH "$ORIGIN/../shiboken6;$ORIGIN/../PySide6"
    )
elseif(APPLE)
    set_target_properties(${_qtm3_python_extension} PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
        INSTALL_RPATH "@loader_path/../shiboken6;@loader_path/../PySide6"
    )
endif()

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

    add_test(
        NAME python_bindings_catalogue_2
        COMMAND "${Python_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_catalogue_2.py"
    )
    set_tests_properties(python_bindings_catalogue_2 PROPERTIES
        ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR};QT_QPA_PLATFORM=offscreen"
    )
endif()
