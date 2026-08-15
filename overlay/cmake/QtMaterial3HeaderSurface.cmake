include_guard(GLOBAL)

include(
    "${PROJECT_SOURCE_DIR}/cmake/QtMaterial3HeaderSurfaceManifest.cmake"
)

function(qtmaterial3_validate_header_surface_manifest)
    foreach(_qtm3_header IN LISTS QTMATERIAL3_PUBLIC_HEADERS)
        if(NOT EXISTS "${PROJECT_SOURCE_DIR}/include/${_qtm3_header}")
            message(FATAL_ERROR
                "QtMaterial3 public header missing from source tree: ${_qtm3_header}"
            )
        endif()

        if(_qtm3_header MATCHES "(^|/)private/" OR
           _qtm3_header MATCHES "_p\\.(h|hh|hpp|hxx)$")
            message(FATAL_ERROR
                "Private-looking header classified public: ${_qtm3_header}"
            )
        endif()
    endforeach()

    foreach(_qtm3_header IN LISTS QTMATERIAL3_PRIVATE_HEADERS)
        if(NOT EXISTS "${PROJECT_SOURCE_DIR}/include/${_qtm3_header}")
            message(FATAL_ERROR
                "QtMaterial3 private header missing from source tree: ${_qtm3_header}"
            )
        endif()
    endforeach()
endfunction()

function(qtmaterial3_install_public_headers)
    qtmaterial3_validate_header_surface_manifest()

    foreach(_qtm3_header IN LISTS QTMATERIAL3_PUBLIC_HEADERS)
        get_filename_component(_qtm3_header_dir "${_qtm3_header}" DIRECTORY)
        install(
            FILES "${PROJECT_SOURCE_DIR}/include/${_qtm3_header}"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${_qtm3_header_dir}"
        )
    endforeach()
endfunction()
