include_guard(GLOBAL)

option(
    QTMATERIAL3_ENABLE_API_CHECKS
    "Enable public API/header checks under CTest"
    ON
)
option(
    QTMATERIAL3_ENABLE_PUBLIC_HEADER_SELF_CONTAINED_TESTS
    "Compile one translation unit per declared public header"
    ON
)

function(qtmaterial3_add_api_checks)
    if(NOT BUILD_TESTING OR NOT QTMATERIAL3_ENABLE_API_CHECKS)
        return()
    endif()

    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    include(
        "${PROJECT_SOURCE_DIR}/cmake/QtMaterial3HeaderSurfaceManifest.cmake"
    )

    set(
        _qtm3_api_checker
        "${PROJECT_SOURCE_DIR}/tools/check_api_surface.py"
    )
    if(NOT EXISTS "${_qtm3_api_checker}")
        message(FATAL_ERROR
            "QtMaterial3 API checker not found: ${_qtm3_api_checker}"
        )
    endif()

    add_test(
        NAME api_surface
        COMMAND
            "${Python3_EXECUTABLE}"
            "${_qtm3_api_checker}"
            --root "${PROJECT_SOURCE_DIR}"
            --scope source
    )
    set_tests_properties(
        api_surface
        PROPERTIES LABELS "api;headers;install;hygiene"
    )

    if(NOT QTMATERIAL3_ENABLE_PUBLIC_HEADER_SELF_CONTAINED_TESTS)
        return()
    endif()

    set(
        _qtm3_header_gen_dir
        "${CMAKE_BINARY_DIR}/generated/public_header_self_contained"
    )
    file(MAKE_DIRECTORY "${_qtm3_header_gen_dir}")

    foreach(_qtm3_rel_header IN LISTS QTMATERIAL3_PUBLIC_HEADERS)
        string(MAKE_C_IDENTIFIER "${_qtm3_rel_header}" _qtm3_ident)
        set(
            _qtm3_src
            "${_qtm3_header_gen_dir}/${_qtm3_ident}.cpp"
        )
        file(
            WRITE "${_qtm3_src}"
            "#include <${_qtm3_rel_header}>\n"
            "int main() { return 0; }\n"
        )

        set(_qtm3_target "public_header_${_qtm3_ident}")
        add_library(
            "${_qtm3_target}"
            OBJECT EXCLUDE_FROM_ALL "${_qtm3_src}"
        )
        target_compile_features(
            "${_qtm3_target}" PRIVATE cxx_std_17
        )
        target_include_directories(
            "${_qtm3_target}"
            PRIVATE "${PROJECT_SOURCE_DIR}/include"
        )
        target_link_libraries(
            "${_qtm3_target}"
            PRIVATE
                Qt${QT_VERSION_MAJOR}::Core
                Qt${QT_VERSION_MAJOR}::Gui
                Qt${QT_VERSION_MAJOR}::Widgets
        )
        if(NOT BUILD_SHARED_LIBS)
            target_compile_definitions(
                "${_qtm3_target}" PRIVATE QTMATERIAL3_STATIC
            )
        endif()

        add_test(
            NAME "public_header_self_contained/${_qtm3_rel_header}"
            COMMAND
                "${CMAKE_COMMAND}"
                --build "${CMAKE_BINARY_DIR}"
                --target "${_qtm3_target}"
                --config "$<CONFIG>"
        )
        set_tests_properties(
            "public_header_self_contained/${_qtm3_rel_header}"
            PROPERTIES LABELS "api;headers;self-contained"
        )
    endforeach()
endfunction()
