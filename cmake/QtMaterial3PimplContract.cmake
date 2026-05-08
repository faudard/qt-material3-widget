# SPDX-License-Identifier: MIT
# Public widget PIMPL contract audit.

find_package(Python3 QUIET COMPONENTS Interpreter)

if(Python3_Interpreter_FOUND)
    add_test(
        NAME public_widget_pimpl_contract
        COMMAND
            ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/check_pimpl_contract.py
            --root ${CMAKE_CURRENT_SOURCE_DIR}
    )
else()
    message(WARNING "Python3 interpreter not found; public_widget_pimpl_contract test is disabled")
endif()
