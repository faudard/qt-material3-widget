# SPDX-License-Identifier: MIT

# Public widget PIMPL contract audit.
#
# This audit is optional because older checkouts and downstream source packages
# may not carry the Python helper. Do not register a CTest test that points to a
# missing file: it fails at runtime before the audit can run.

find_package(Python3 QUIET COMPONENTS Interpreter)

set(_qtm3_pimpl_contract_script
    "${CMAKE_CURRENT_SOURCE_DIR}/tools/check_pimpl_contract.py"
)

if(Python3_Interpreter_FOUND AND EXISTS "${_qtm3_pimpl_contract_script}")
    add_test(
        NAME public_widget_pimpl_contract
        COMMAND
            "${Python3_EXECUTABLE}"
            "${_qtm3_pimpl_contract_script}"
            --root "${CMAKE_CURRENT_SOURCE_DIR}"
    )
elseif(NOT Python3_Interpreter_FOUND)
    message(WARNING
        "Python3 interpreter not found; "
        "public_widget_pimpl_contract test is disabled"
    )
else()
    message(WARNING
        "PIMPL contract script not found at ${_qtm3_pimpl_contract_script}; "
        "public_widget_pimpl_contract test is disabled"
    )
endif()
