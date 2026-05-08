# Public header ABI audit for qt-material3-widget.
# Include this file from the top-level CMakeLists.txt, then call:
#   qtmaterial3_add_public_abi_audit()

option(QTMATERIAL3_ENABLE_PUBLIC_ABI_AUDIT "Enable public widget header ABI audit under CTest" ON)

function(qtmaterial3_add_public_abi_audit)
    if(NOT QTMATERIAL3_ENABLE_PUBLIC_ABI_AUDIT)
        return()
    endif()

    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(NOT Python3_Interpreter_FOUND)
        message(WARNING "Python3 not found; public ABI audit test disabled")
        return()
    endif()

    set(_qtmaterial3_abi_audit_script "${PROJECT_SOURCE_DIR}/tools/check_public_widget_abi.py")
    set(_qtmaterial3_abi_audit_headers "${PROJECT_SOURCE_DIR}/include/qtmaterial/widgets")

    if(NOT EXISTS "${_qtmaterial3_abi_audit_script}")
        message(WARNING "Public ABI audit script not found: ${_qtmaterial3_abi_audit_script}")
        return()
    endif()

    if(NOT EXISTS "${_qtmaterial3_abi_audit_headers}")
        message(WARNING "Public widget header directory not found: ${_qtmaterial3_abi_audit_headers}")
        return()
    endif()

    add_test(
        NAME qtmaterial3_public_widget_headers_abi_audit
        COMMAND "${Python3_EXECUTABLE}"
                "${_qtmaterial3_abi_audit_script}"
                --root "${PROJECT_SOURCE_DIR}"
                --headers-dir "${_qtmaterial3_abi_audit_headers}"
                --strict
    )

    set_tests_properties(qtmaterial3_public_widget_headers_abi_audit PROPERTIES
        LABELS "abi;headers;widgets"
    )
endfunction()
