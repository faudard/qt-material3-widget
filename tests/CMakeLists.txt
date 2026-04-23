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
qtm3_add_test(tst_phase4specs specs/tst_phase4specs.cpp qtmaterial3_theme qtmaterial3_core qtmaterial3_specs)

qtm3_add_test(tst_statelayerpainter effects/tst_statelayerpainter.cpp qtmaterial3_effects)
qtm3_add_test(tst_ripplecontroller effects/tst_ripplecontroller.cpp qtmaterial3_effects)
qtm3_add_test(tst_focusindicator effects/tst_focusindicator.cpp qtmaterial3_effects)
qtm3_add_test(tst_shadowrenderer effects/tst_shadowrenderer.cpp qtmaterial3_effects)
qtm3_add_test(tst_transitioncontroller effects/tst_transitioncontroller.cpp qtmaterial3_effects)

qtm3_add_test(tst_textbutton widgets/buttons/tst_textbutton.cpp qtmaterial3_widgets)
qtm3_add_test(tst_filledbutton widgets/buttons/tst_filledbutton.cpp qtmaterial3_widgets)
qtm3_add_test(tst_iconbutton widgets/buttons/tst_iconbutton.cpp qtmaterial3_widgets)
qtm3_add_test(tst_filledtonalbutton widgets/buttons/tst_filledtonalbutton.cpp qtmaterial3_widgets)
qtm3_add_test(tst_outlinedbutton widgets/buttons/tst_outlinedbutton.cpp qtmaterial3_widgets)
qtm3_add_test(tst_elevatedbutton widgets/buttons/tst_elevatedbutton.cpp qtmaterial3_widgets)
qtm3_add_test(tst_fab widgets/buttons/tst_fab.cpp qtmaterial3_widgets)
qtm3_add_test(tst_extendedfab widgets/buttons/tst_extendedfab.cpp qtmaterial3_widgets)

qtm3_add_test(tst_checkbox widgets/selection/tst_checkbox.cpp qtmaterial3_widgets)
qtm3_add_test(tst_switch widgets/selection/tst_switch.cpp qtmaterial3_widgets)
qtm3_add_test(tst_radiobutton widgets/selection/tst_radiobutton.cpp qtmaterial3_widgets)

qtm3_add_test(tst_dialog widgets/surfaces/tst_dialog.cpp qtmaterial3_widgets)
qtm3_add_test(tst_navigationdrawer widgets/surfaces/tst_navigationdrawer.cpp qtmaterial3_widgets)
qtm3_add_test(tst_bottomsheet widgets/surfaces/tst_bottomsheet.cpp qtmaterial3_widgets)
qtm3_add_test(tst_banner widgets/surfaces/tst_banner.cpp qtmaterial3_widgets)
qtm3_add_test(tst_card widgets/surfaces/tst_card.cpp qtmaterial3_widgets)
qtm3_add_test(tst_topappbar widgets/surfaces/tst_topappbar.cpp qtmaterial3_widgets)
qtm3_add_test(tst_bottomappbar widgets/surfaces/tst_bottomappbar.cpp qtmaterial3_widgets)

qtm3_add_test(tst_outlinedtextfield widgets/inputs/tst_outlinedtextfield.cpp qtmaterial3_widgets)
qtm3_add_test(tst_filledtextfield widgets/inputs/tst_filledtextfield.cpp qtmaterial3_widgets)
qtm3_add_test(tst_autocompletepopup widgets/inputs/tst_autocompletepopup.cpp qtmaterial3_widgets)
qtm3_add_test(tst_datefield widgets/inputs/tst_datefield.cpp qtmaterial3_widgets)

qtm3_add_test(tst_listitem widgets/data/tst_listitem.cpp qtmaterial3_widgets)
qtm3_add_test(tst_list widgets/data/tst_list.cpp qtmaterial3_widgets)
qtm3_add_test(tst_divider widgets/data/tst_divider.cpp qtmaterial3_widgets)
qtm3_add_test(tst_filledtextfield_render render/inputs/tst_filledtextfield_render.cpp qtmaterial3_widgets)

# Phase 5 note: add accessibility, keyboard, and installed-package consumer checks here.

qtm3_add_test(tst_filledbutton_render render/buttons/tst_filledbutton_render.cpp qtmaterial3_widgets)
qtm3_add_test(tst_outlinedbutton_render render/buttons/tst_outlinedbutton_render.cpp qtmaterial3_widgets)

qtm3_add_test(tst_outlinedtextfield_render render/inputs/tst_outlinedtextfield_render.cpp qtmaterial3_widgets)
qtm3_add_test(tst_themeserializer theme/tst_themeserializer.cpp qtmaterial3_theme)

qtm3_add_test(tst_abstractbutton_state core/tst_abstractbutton_state.cpp qtmaterial3_core)
# qtm3_add_test(tst_overlaysurface core/tst_overlaysurface.cpp qtmaterial3_core)
#qtm3_add_test(tst_selectioncontrol_layout core/tst_selectioncontrol_layout.cpp qtmaterial3_core)
qtm3_add_test(tst_inputcontrol core/tst_inputcontrol.cpp qtmaterial3_core)
qtm3_add_test(tst_scrimwidget effects/tst_scrimwidget.cpp qtmaterial3_effects)
# qtm3_add_test(tst_filledtonalbutton_render tests/render/buttons/tst_filledtonalbutton_render.cpp qtmaterial3_widgets)


# comment don t compile TODO
# qtm3_add_test(tst_textfieldshellhelper widgets/inputs/tst_textfieldshellhelper.cpp qtmaterial3_widgets)
# target_include_directories(tst_textfieldshellhelper PRIVATE
#     ${CMAKE_SOURCE_DIR}/src/widgets/inputs/private
# )

qtm3_add_test(tst_snackbar widgets/surfaces/tst_snackbar.cpp qtmaterial3_widgets)
qtm3_add_test(tst_snackbarhost widgets/surfaces/tst_snackbarhost.cpp qtmaterial3_widgets)




set(QTM3_CONSUMER_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/_installed_package")
set(QTM3_CONSUMER_BINARY_DIR  "${CMAKE_CURRENT_BINARY_DIR}/_consumer_build")

add_test(
    NAME tst_installed_package_consumer
    COMMAND
        ${CMAKE_COMMAND}
        -DREPO_BINARY_DIR=${CMAKE_BINARY_DIR}
        -DTEST_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/consumer
        -DTEST_BINARY_DIR=${QTM3_CONSUMER_BINARY_DIR}
        -DINSTALL_PREFIX=${QTM3_CONSUMER_INSTALL_DIR}
        -DBUILD_CONFIG=$<CONFIG>
        -DDEFAULT_BUILD_CONFIG=${CMAKE_BUILD_TYPE}
        -DGENERATOR_NAME=${CMAKE_GENERATOR}
        -DGENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}
        -DGENERATOR_TOOLSET=${CMAKE_GENERATOR_TOOLSET}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/consumer/run_installed_package_test.cmake
)