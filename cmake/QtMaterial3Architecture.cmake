include_guard(GLOBAL)

function(qtmaterial3_add_architecture_check)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)

    set(
        _qtm3_architecture_checker
        "${PROJECT_SOURCE_DIR}/scripts/architecture/check_architecture.py"
    )
    set(
        _qtm3_architecture_rules
        "${PROJECT_SOURCE_DIR}/scripts/architecture/architecture_rules.json"
    )

    if(NOT EXISTS "${_qtm3_architecture_checker}")
        message(FATAL_ERROR
            "QtMaterial3 architecture checker not found: "
            "${_qtm3_architecture_checker}"
        )
    endif()

    if(NOT EXISTS "${_qtm3_architecture_rules}")
        message(FATAL_ERROR
            "QtMaterial3 architecture rules not found: "
            "${_qtm3_architecture_rules}"
        )
    endif()

    add_custom_target(
        qtmaterial3_architecture_check
        COMMAND
            "${Python3_EXECUTABLE}"
            "${_qtm3_architecture_checker}"
            --root "${PROJECT_SOURCE_DIR}"
            --rules "${_qtm3_architecture_rules}"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        COMMENT "Checking QtMaterial3 architecture"
        VERBATIM
    )

    if(BUILD_TESTING AND QTMATERIAL3_BUILD_TESTS)
        add_test(
            NAME architecture
            COMMAND
                "${Python3_EXECUTABLE}"
                "${_qtm3_architecture_checker}"
                --root "${PROJECT_SOURCE_DIR}"
                --rules "${_qtm3_architecture_rules}"
        )
        set_tests_properties(
            architecture
            PROPERTIES LABELS "architecture;static-analysis;zero-debt"
        )
    endif()
endfunction()
