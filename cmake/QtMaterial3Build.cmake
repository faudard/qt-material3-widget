include_guard(GLOBAL)

function(qtm3_add_library target)
    if(BUILD_SHARED_LIBS)
        set(_qtm3_library_type SHARED)
    else()
        set(_qtm3_library_type STATIC)
    endif()

    if(COMMAND qt_add_library)
        qt_add_library(${target} ${_qtm3_library_type} ${ARGN})
    else()
        add_library(${target} ${_qtm3_library_type} ${ARGN})
    endif()

    target_compile_features(${target} PUBLIC cxx_std_17)

    if(NOT BUILD_SHARED_LIBS)
        target_compile_definitions(${target} PUBLIC QTMATERIAL3_STATIC)
    endif()

    set_target_properties(${target}
        PROPERTIES
            CXX_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN ON
    )
endfunction()

function(qtm3_setup_library target export_name output_name private_export_define)
    target_compile_features(${target} PUBLIC cxx_std_17)

    target_compile_definitions(${target}
        PRIVATE
            ${private_export_define}
    )

    target_include_directories(${target}
        PUBLIC
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )

    set_target_properties(${target}
        PROPERTIES
            EXPORT_NAME ${export_name}
            OUTPUT_NAME ${output_name}
            VERSION ${PROJECT_VERSION}
            SOVERSION ${PROJECT_VERSION_MAJOR}
    )
endfunction()

function(qtm3_add_module target export_name output_name private_export_define)
    qtm3_add_library(${target} ${ARGN})
    add_library(QtMaterial3::${export_name} ALIAS ${target})
    qtm3_setup_library(
        ${target}
        ${export_name}
        ${output_name}
        ${private_export_define}
    )
endfunction()
