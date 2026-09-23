include_guard(GLOBAL)

include(
    "${PROJECT_SOURCE_DIR}/cmake/QtMaterial3HeaderSurfaceManifest.cmake"
)

function(qtmaterial3_validate_header_surface_manifest)
    foreach(_qtm3_header IN LISTS QTMATERIAL3_PUBLIC_HEADERS)
        if(NOT EXISTS "${PROJECT_SOURCE_DIR}/include/${_qtm3_header}")
            message(FATAL_ERROR
                "QtMaterial3 public header missing from source tree: "
                "${_qtm3_header}"
            )
        endif()
        if(_qtm3_header MATCHES "(^|/)private/" OR
           _qtm3_header MATCHES "_p\\.(h|hh|hpp|hxx)$")
            message(FATAL_ERROR
                "Private-looking header classified public: "
                "${_qtm3_header}"
            )
        endif()
    endforeach()

    foreach(_qtm3_header IN LISTS QTMATERIAL3_PRIVATE_HEADERS)
        if(NOT EXISTS "${PROJECT_SOURCE_DIR}/include/${_qtm3_header}")
            message(FATAL_ERROR
                "QtMaterial3 private header missing from source tree: "
                "${_qtm3_header}"
            )
        endif()
    endforeach()
endfunction()

function(qtmaterial3_install_public_headers)
    qtmaterial3_validate_header_surface_manifest()

    foreach(_qtm3_header IN LISTS QTMATERIAL3_PUBLIC_HEADERS)
        get_filename_component(
            _qtm3_header_dir "${_qtm3_header}" DIRECTORY
        )
        install(
            FILES "${PROJECT_SOURCE_DIR}/include/${_qtm3_header}"
            DESTINATION
                "${CMAKE_INSTALL_INCLUDEDIR}/${_qtm3_header_dir}"
        )
    endforeach()
endfunction()

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

    set(_qtm3_header_sources)
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
        list(APPEND _qtm3_header_sources "${_qtm3_src}")
    endforeach()

    set(_qtm3_target qtmaterial3_public_headers_self_contained)
    add_library(
        "${_qtm3_target}"
        OBJECT EXCLUDE_FROM_ALL
        ${_qtm3_header_sources}
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
        NAME public_headers_self_contained
        COMMAND
            "${CMAKE_COMMAND}"
            --build "${CMAKE_BINARY_DIR}"
            --target "${_qtm3_target}"
            --config "$<CONFIG>"
            --parallel
    )
    set_tests_properties(
        public_headers_self_contained
        PROPERTIES LABELS "api;headers;self-contained"
    )
endfunction()
