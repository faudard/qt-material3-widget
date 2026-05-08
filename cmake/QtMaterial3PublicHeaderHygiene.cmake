# Public widget header hygiene and self-contained compile checks.
# This module is intentionally optional and is meant for the source tree tests.

function(qtmaterial3_add_public_header_hygiene_tests)
    if(NOT BUILD_TESTING)
        return()
    endif()

    option(QTMATERIAL3_ENABLE_PUBLIC_HEADER_HYGIENE_AUDIT
        "Enable public widget header include hygiene audit" ON)
    option(QTMATERIAL3_ENABLE_PUBLIC_HEADER_SELF_CONTAINED_TESTS
        "Enable one-translation-unit-per-public-widget-header compile checks" ON)

    find_package(Python3 COMPONENTS Interpreter QUIET)

    if(QTMATERIAL3_ENABLE_PUBLIC_HEADER_HYGIENE_AUDIT)
        if(Python3_Interpreter_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/tools/check_public_header_hygiene.py")
            add_test(
                NAME public_widget_header_hygiene
                COMMAND "${Python3_EXECUTABLE}"
                        "${CMAKE_SOURCE_DIR}/tools/check_public_header_hygiene.py"
                        --root "${CMAKE_SOURCE_DIR}"
                        --strict
            )
            set_tests_properties(public_widget_header_hygiene PROPERTIES
                LABELS "abi;headers;pimpl;hygiene"
            )
        else()
            message(STATUS "QtMaterial3: public header hygiene audit skipped; Python3 or tool missing")
        endif()
    endif()

    if(NOT QTMATERIAL3_ENABLE_PUBLIC_HEADER_SELF_CONTAINED_TESTS)
        return()
    endif()

    if(NOT TARGET qtmaterial3_widgets)
        message(STATUS "QtMaterial3: public header self-contained tests skipped; qtmaterial3_widgets target not defined yet")
        return()
    endif()

    file(GLOB_RECURSE QTMATERIAL3_PUBLIC_WIDGET_HEADERS CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/include/qtmaterial/widgets/*.h"
    )

    if(NOT QTMATERIAL3_PUBLIC_WIDGET_HEADERS)
        message(STATUS "QtMaterial3: public header self-contained tests skipped; no public widget headers found")
        return()
    endif()

    set(_qtm3_header_gen_dir "${CMAKE_BINARY_DIR}/generated/public_widget_header_self_contained")
    file(MAKE_DIRECTORY "${_qtm3_header_gen_dir}")

    foreach(_qtm3_header IN LISTS QTMATERIAL3_PUBLIC_WIDGET_HEADERS)
        file(RELATIVE_PATH _qtm3_rel_header "${CMAKE_SOURCE_DIR}/include" "${_qtm3_header}")
        string(MAKE_C_IDENTIFIER "${_qtm3_rel_header}" _qtm3_ident)
        set(_qtm3_src "${_qtm3_header_gen_dir}/${_qtm3_ident}.cpp")

        file(WRITE "${_qtm3_src}"
            "#include <${_qtm3_rel_header}>\n"
            "int main() { return 0; }\n"
        )

        set(_qtm3_target "public_header_${_qtm3_ident}")
        add_executable("${_qtm3_target}" EXCLUDE_FROM_ALL "${_qtm3_src}")
        target_link_libraries("${_qtm3_target}" PRIVATE qtmaterial3_widgets)

        add_test(
            NAME "public_widget_header_self_contained/${_qtm3_rel_header}"
            COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target "${_qtm3_target}" --config "$<CONFIG>"
        )
        set_tests_properties("public_widget_header_self_contained/${_qtm3_rel_header}" PROPERTIES
            LABELS "abi;headers;pimpl;self-contained"
        )
    endforeach()
endfunction()
