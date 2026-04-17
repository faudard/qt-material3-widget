find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Widgets)

function(qtm3_add_test target source)
    if(COMMAND qt_add_executable)
        qt_add_executable(${target} ${source})
    else()
        add_executable(${target} ${source})
    endif()

    target_link_libraries(${target}
        PRIVATE
            Qt${QT_VERSION_MAJOR}::Test
            ${ARGN}
    )

    add_test(NAME ${target} COMMAND ${target})
endfunction()

qtm3_add_test(tst_themebuilder theme/tst_themebuilder.cpp qtmaterial3_theme)
qtm3_add_test(tst_thememanager theme/tst_thememanager.cpp qtmaterial3_theme)
qtm3_add_test(tst_legacyaliasmap legacy/tst_legacyaliasmap.cpp qtmaterial3_theme qtmaterial3_legacy)
qtm3_add_test(tst_interactionstate core/tst_interactionstate.cpp qtmaterial3_core)
qtm3_add_test(tst_specfactory specs/tst_specfactory.cpp qtmaterial3_theme qtmaterial3_core qtmaterial3_specs)

qtm3_add_test(tst_statelayerpainter effects/tst_statelayerpainter.cpp qtmaterial3_effects)
qtm3_add_test(tst_ripplecontroller effects/tst_ripplecontroller.cpp qtmaterial3_effects)
qtm3_add_test(tst_focusindicator effects/tst_focusindicator.cpp qtmaterial3_effects)
qtm3_add_test(tst_shadowrenderer effects/tst_shadowrenderer.cpp qtmaterial3_effects)
qtm3_add_test(tst_transitioncontroller effects/tst_transitioncontroller.cpp qtmaterial3_effects)

qtm3_add_test(tst_textbutton widgets/buttons/tst_textbutton.cpp qtmaterial3_widgets)
qtm3_add_test(tst_filledbutton widgets/buttons/tst_filledbutton.cpp qtmaterial3_widgets)
qtm3_add_test(tst_iconbutton widgets/buttons/tst_iconbutton.cpp qtmaterial3_widgets)
qtm3_add_test(tst_checkbox widgets/selection/tst_checkbox.cpp qtmaterial3_widgets)
qtm3_add_test(tst_switch widgets/selection/tst_switch.cpp qtmaterial3_widgets)
qtm3_add_test(tst_dialog widgets/surfaces/tst_dialog.cpp qtmaterial3_widgets)
qtm3_add_test(tst_outlinedtextfield widgets/inputs/tst_outlinedtextfield.cpp qtmaterial3_widgets)