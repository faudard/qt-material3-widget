include_guard(GLOBAL)

function(qtmaterial3_add_architecture_contracts)
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(NOT Python3_Interpreter_FOUND)
        message(FATAL_ERROR
            "Python3 is required for QtMaterial3 architecture contracts."
        )
    endif()

    set(
        _qtm3_arch_zero_debt
        "${PROJECT_SOURCE_DIR}/tools/check_architecture_zero_debt.py"
    )

    if(NOT EXISTS "${_qtm3_arch_zero_debt}")
        message(FATAL_ERROR
            "Architecture zero-debt checker not found: ${_qtm3_arch_zero_debt}"
        )
    endif()

    add_custom_target(
        qtmaterial3_architecture_contracts
        COMMAND
            "${Python3_EXECUTABLE}"
            "${_qtm3_arch_zero_debt}"
            --root "${PROJECT_SOURCE_DIR}"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        COMMENT "Checking QtMaterial3 zero-debt architecture contract"
        VERBATIM
    )

    # Backward-compatible target name. It runs the same zero-debt authority.
    add_custom_target(
        qtmaterial3_architecture_contracts_strict
        COMMAND
            "${Python3_EXECUTABLE}"
            "${_qtm3_arch_zero_debt}"
            --root "${PROJECT_SOURCE_DIR}"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        COMMENT "Checking QtMaterial3 zero-debt architecture contract"
        VERBATIM
    )

    if(BUILD_TESTING AND QTMATERIAL3_BUILD_TESTS)
        add_test(
            NAME architecture_contracts
            COMMAND
                "${Python3_EXECUTABLE}"
                "${_qtm3_arch_zero_debt}"
                --root "${PROJECT_SOURCE_DIR}"
        )
        set_tests_properties(
            architecture_contracts
            PROPERTIES LABELS "architecture;static-analysis;zero-debt"
        )
    endif()
endfunction()
