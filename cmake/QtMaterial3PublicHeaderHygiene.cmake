# Public header manifest validation and one-header-per-TU compile tests.

function(qtmaterial3_add_public_header_hygiene_tests)
    if(NOT BUILD_TESTING)
        return()
    endif()

    find_package(Python3 REQUIRED COMPONENTS Interpreter)

    include(
        "${PROJECT_SOURCE_DIR}/cmake/QtMaterial3HeaderSurfaceManifest.cmake"
    )

    add_test(
        NAME public_private_header_contract
        COMMAND
            "${Python3_EXECUTABLE}"
            "${PROJECT_SOURCE_DIR}/tools/check_public_private_headers.py"
            --root "${PROJECT_SOURCE_DIR}"
    )
    set_tests_properties(
        public_private_header_contract
        PROPERTIES LABELS "api;headers;install;hygiene"
    )

    option(
        QTMATERIAL3_ENABLE_PUBLIC_HEADER_SELF_CONTAINED_TESTS
        "Compile one translation unit per declared public header"
        ON
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
        set(_qtm3_src "${_qtm3_header_gen_dir}/${_qtm3_ident}.cpp")

        file(
            WRITE "${_qtm3_src}"
            "#include <${_qtm3_rel_header}>\n"
            "int main() { return 0; }\n"
        )

        set(_qtm3_target "public_header_${_qtm3_ident}")
        add_library("${_qtm3_target}" OBJECT EXCLUDE_FROM_ALL "${_qtm3_src}")

        target_compile_features("${_qtm3_target}" PRIVATE cxx_std_17)
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
            target_compile_definitions("${_qtm3_target}" PRIVATE QTMATERIAL3_STATIC)
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
