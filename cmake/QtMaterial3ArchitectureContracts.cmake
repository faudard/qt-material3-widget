include_guard(GLOBAL)

function(qtmaterial3_add_architecture_contracts)
    find_package(Python3 COMPONENTS Interpreter QUIET)

    if(NOT Python3_Interpreter_FOUND)
        message(WARNING
            "Python3 was not found; qt-material3 architecture contracts are disabled."
        )
        return()
    endif()

    set(_qtm3_arch_checker
        "${PROJECT_SOURCE_DIR}/tools/check_architecture_contracts.py"
    )
    set(_qtm3_arch_config
        "${PROJECT_SOURCE_DIR}/tools/architecture_contracts.json"
    )

    if(NOT EXISTS "${_qtm3_arch_checker}")
        message(WARNING
            "Architecture contract checker not found: ${_qtm3_arch_checker}"
        )
        return()
    endif()

    add_custom_target(qtmaterial3_architecture_contracts
        COMMAND "${Python3_EXECUTABLE}" "${_qtm3_arch_checker}"
                --root "${PROJECT_SOURCE_DIR}"
                --config "${_qtm3_arch_config}"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        COMMENT "Checking qt-material3 architecture contracts"
        VERBATIM
    )

    add_custom_target(qtmaterial3_architecture_contracts_strict
        COMMAND "${Python3_EXECUTABLE}" "${_qtm3_arch_checker}"
                --root "${PROJECT_SOURCE_DIR}"
                --config "${_qtm3_arch_config}"
                --strict
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        COMMENT "Checking qt-material3 architecture contracts in strict mode"
        VERBATIM
    )

    add_custom_target(qtmaterial3_architecture_contracts_update_baseline
        COMMAND "${Python3_EXECUTABLE}" "${_qtm3_arch_checker}"
                --root "${PROJECT_SOURCE_DIR}"
                --config "${_qtm3_arch_config}"
                --update-baseline
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        COMMENT "Updating qt-material3 architecture debt baseline"
        VERBATIM
    )

    if(BUILD_TESTING AND QTMATERIAL3_BUILD_TESTS)
        add_test(
            NAME architecture_contracts
            COMMAND "${Python3_EXECUTABLE}" "${_qtm3_arch_checker}"
                    --root "${PROJECT_SOURCE_DIR}"
                    --config "${_qtm3_arch_config}"
        )
        set_tests_properties(architecture_contracts PROPERTIES
            LABELS "architecture;static-analysis"
        )
    endif()
endfunction()
