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
qtm3_add_test(tst_themecontext theme/tst_themecontext.cpp qtmaterial3_theme)
qtm3_add_test(tst_themecontextbinding core/tst_themecontextbinding.cpp qtmaterial3_core)
qtm3_add_test(tst_thememanager_runtime theme/tst_thememanager_runtime.cpp qtmaterial3_theme)
qtm3_add_test(tst_interactionstate core/tst_interactionstate.cpp qtmaterial3_core)
qtm3_add_test(tst_widget_themecontext core/tst_widget_themecontext.cpp qtmaterial3_core)
qtm3_add_test(tst_dataspecresolver specs/tst_dataspecresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_surfacespecresolver specs/tst_surfacespecresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_surfaceresolvedspec specs/tst_surfaceresolvedspec.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_snackbarresolvedspec specs/tst_snackbarresolvedspec.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_progressspecresolver specs/tst_progressspecresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_tabsspecresolver specs/tst_tabsspecresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_navigationrailspecresolver specs/tst_navigationrailspecresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_navigationrailresolvedspec specs/tst_navigationrailresolvedspec.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_overlaysurfacespecresolver specs/tst_overlaysurfacespecresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_appbarspecresolver specs/tst_appbarspecresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_dialogspecresolver specs/tst_dialogspecresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_buttonspecresolver specs/tst_buttonspecresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_chipspecresolver specs/tst_chipspecresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_segmentedbuttonspecresolver specs/tst_segmentedbuttonspecresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_compactresolvedspec specs/tst_compactresolvedspec.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_menuspecresolver specs/tst_menuspecresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_menuresolvedspec specs/tst_menuresolvedspec.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_autocompletespecresolver specs/tst_autocompletespecresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_autocompleteresolvedspec specs/tst_autocompleteresolvedspec.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_datacomponentspecresolver specs/tst_datacomponentspecresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_datacomponentdefaults_linkage specs/tst_datacomponentdefaults_linkage.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_buttonresolvedspec specs/tst_buttonresolvedspec.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_actionbuttonspecresolver specs/tst_actionbuttonspecresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_textfieldspecresolver specs/tst_textfieldspecresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_textfield_validation_robustness widgets/inputs/tst_textfield_validation_robustness.cpp qtmaterial3_widgets)
qtm3_add_test(tst_autocompletepopupspecresolver specs/tst_autocompletepopupspecresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_datefieldspecresolver specs/tst_datefieldspecresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_buttonelevationspec specs/tst_buttonelevationspec.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_selectionspecresolver specs/tst_selectionspecresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_selectionresolvedspec specs/tst_selectionresolvedspec.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_componenttokenoverrides specs/tst_componenttokenoverrides.cpp qtmaterial3_theme qtmaterial3_core qtmaterial3_specs)
qtm3_add_test(tst_phase4specs specs/tst_phase4specs.cpp qtmaterial3_theme qtmaterial3_core qtmaterial3_specs)

qtm3_add_test(tst_statelayerpainter effects/tst_statelayerpainter.cpp qtmaterial3_effects)
qtm3_add_test(tst_ripplecontroller effects/tst_ripplecontroller.cpp qtmaterial3_effects)
qtm3_add_test(tst_focusindicator effects/tst_focusindicator.cpp qtmaterial3_effects)
qtm3_add_test(tst_shadowrenderer effects/tst_shadowrenderer.cpp qtmaterial3_effects)
qtm3_add_test(tst_paintcache effects/tst_paintcache.cpp qtmaterial3_effects)
qtm3_add_test(tst_elevationrenderer effects/tst_elevationrenderer.cpp qtmaterial3_effects)
qtm3_add_test(tst_transitioncontroller effects/tst_transitioncontroller.cpp qtmaterial3_effects)

qtm3_add_test(tst_textbutton widgets/buttons/tst_textbutton.cpp qtmaterial3_widgets)
qtm3_add_test(tst_button_text_layout widgets/buttons/tst_button_text_layout.cpp qtmaterial3_widgets)
qtm3_add_test(tst_button_render_helper_layout widgets/buttons/tst_button_render_helper_layout.cpp qtmaterial3_widgets)
qtm3_add_test(tst_button_icon_hidpi widgets/buttons/tst_button_icon_hidpi.cpp qtmaterial3_widgets)
qtm3_add_test(tst_button_change_events widgets/buttons/tst_button_change_events.cpp qtmaterial3_widgets)
qtm3_add_test(tst_button_keyboard_activation widgets/buttons/tst_button_keyboard_activation.cpp qtmaterial3_widgets)
qtm3_add_test(tst_button_touch_target widgets/buttons/tst_button_touch_target.cpp qtmaterial3_widgets)
qtm3_add_test(tst_button_text_accessibility widgets/buttons/tst_button_text_accessibility.cpp qtmaterial3_widgets)
qtm3_add_test(tst_button_constructor_api widgets/buttons/tst_button_constructor_api.cpp qtmaterial3_widgets)
qtm3_add_test(tst_filledbutton widgets/buttons/tst_filledbutton.cpp qtmaterial3_widgets)
qtm3_add_test(tst_iconbutton widgets/buttons/tst_iconbutton.cpp qtmaterial3_widgets)
qtm3_add_test(tst_icon_fab_accessibility widgets/buttons/tst_icon_fab_accessibility.cpp qtmaterial3_widgets)
qtm3_add_test(tst_filledtonalbutton widgets/buttons/tst_filledtonalbutton.cpp qtmaterial3_widgets)
qtm3_add_test(tst_outlinedbutton widgets/buttons/tst_outlinedbutton.cpp qtmaterial3_widgets)
qtm3_add_test(tst_outlinedbutton_rendering widgets/buttons/tst_outlinedbutton_rendering.cpp qtmaterial3_widgets)
qtm3_add_test(tst_elevatedbutton widgets/buttons/tst_elevatedbutton.cpp qtmaterial3_widgets)
qtm3_add_test(tst_elevatedbutton_shadow widgets/buttons/tst_elevatedbutton_shadow.cpp qtmaterial3_widgets)
qtm3_add_test(tst_fab widgets/buttons/tst_fab.cpp qtmaterial3_widgets)
qtm3_add_test(tst_extendedfab widgets/buttons/tst_extendedfab.cpp qtmaterial3_widgets)
qtm3_add_test(tst_extendedfab_maturity widgets/buttons/tst_extendedfab_maturity.cpp qtmaterial3_widgets)
qtm3_add_test(tst_button_contracts widgets/buttons/tst_button_contracts.cpp qtmaterial3_widgets)

qtm3_add_test(tst_checkbox widgets/selection/tst_checkbox.cpp qtmaterial3_widgets)
qtm3_add_test(tst_switch widgets/selection/tst_switch.cpp qtmaterial3_widgets)
qtm3_add_test(tst_radiobutton widgets/selection/tst_radiobutton.cpp qtmaterial3_widgets)
qtm3_add_test(tst_segmentedbutton widgets/selection/tst_segmentedbutton.cpp qtmaterial3_widgets)
qtm3_add_test(tst_compactresolvedwidgets widgets/selection/tst_compactresolvedwidgets.cpp qtmaterial3_widgets)

qtm3_add_test(tst_dialog widgets/surfaces/tst_dialog.cpp qtmaterial3_widgets)
qtm3_add_test(tst_navigationdrawer widgets/surfaces/tst_navigationdrawer.cpp qtmaterial3_widgets)
qtm3_add_test(tst_bottomsheet widgets/surfaces/tst_bottomsheet.cpp qtmaterial3_widgets)
qtm3_add_test(tst_bottomsheet_drag_robustness widgets/surfaces/tst_bottomsheet_drag_robustness.cpp qtmaterial3_widgets)
qtm3_add_test(tst_banner widgets/surfaces/tst_banner.cpp qtmaterial3_widgets)
qtm3_add_test(tst_card widgets/surfaces/tst_card.cpp qtmaterial3_widgets)
qtm3_add_test(tst_topappbar widgets/surfaces/tst_topappbar.cpp qtmaterial3_widgets)
qtm3_add_test(tst_bottomappbar widgets/surfaces/tst_bottomappbar.cpp qtmaterial3_widgets)

qtm3_add_test(tst_outlinedtextfield widgets/inputs/tst_outlinedtextfield.cpp qtmaterial3_widgets)
qtm3_add_test(tst_filledtextfield widgets/inputs/tst_filledtextfield.cpp qtmaterial3_widgets)
qtm3_add_test(tst_textfield_validation_group widgets/inputs/tst_textfield_validation_group.cpp qtmaterial3_widgets)
qtm3_add_test(tst_textfield_validation_summary widgets/inputs/tst_textfield_validation_summary.cpp qtmaterial3_widgets)

# qtm3_add_test(tst_textfieldshellhelper_rtl widgets/inputs/tst_textfieldshellhelper_rtl.cpp qtmaterial3_widgets)
# target_include_directories(tst_textfieldshellhelper_rtl PRIVATE
#     ${CMAKE_SOURCE_DIR}/src/widgets/inputs/private
# )
qtm3_add_test(tst_textfield_accessibility widgets/inputs/tst_textfield_accessibility.cpp qtmaterial3_widgets)


function(qtm3_add_textfieldshellhelper_private_test target source)
    qtm3_add_test(${target}
        ${source}
        qtmaterial3_theme
        qtmaterial3_core
        qtmaterial3_specs
        qtmaterial3_effects
    )

    target_sources(${target} PRIVATE
        ${CMAKE_SOURCE_DIR}/src/widgets/inputs/private/qtmaterialtextfieldshellhelper_p.cpp
    )

    target_include_directories(${target} PRIVATE
        ${CMAKE_SOURCE_DIR}/src/widgets/inputs/private
    )
endfunction()

qtm3_add_textfieldshellhelper_private_test(
    tst_textfieldshellhelper_rtl
    widgets/inputs/tst_textfieldshellhelper_rtl.cpp
)

qtm3_add_textfieldshellhelper_private_test(
    tst_textfieldshellhelper
    widgets/inputs/tst_textfieldshellhelper.cpp
)

qtm3_add_test(tst_autocompletepopup widgets/inputs/tst_autocompletepopup.cpp qtmaterial3_widgets)
qtm3_add_test(tst_autocompletepopup_themecontext widgets/inputs/tst_autocompletepopup_themecontext.cpp qtmaterial3_widgets)
qtm3_add_test(tst_autocompleteresolvedwidget widgets/inputs/tst_autocompleteresolvedwidget.cpp qtmaterial3_widgets)
qtm3_add_test(tst_datefield widgets/inputs/tst_datefield.cpp qtmaterial3_widgets)
qtm3_add_test(tst_datefield_affordances widgets/inputs/tst_datefield_affordances.cpp qtmaterial3_widgets)

qtm3_add_test(tst_listitem widgets/data/tst_listitem.cpp qtmaterial3_widgets)
qtm3_add_test(tst_data_resolved_widgets widgets/data/tst_data_resolved_widgets.cpp qtmaterial3_widgets)
qtm3_add_test(tst_list widgets/data/tst_list.cpp qtmaterial3_widgets)
qtm3_add_test(tst_table widgets/data/tst_table.cpp qtmaterial3_widgets)
qtm3_add_test(tst_themedtable widgets/data/tst_themedtable.cpp qtmaterial3_widgets)
qtm3_add_test(tst_table_resolvedspec widgets/data/tst_table_resolvedspec.cpp qtmaterial3_widgets)
qtm3_add_test(tst_gridlist widgets/data/tst_gridlist.cpp qtmaterial3_widgets)
qtm3_add_test(tst_themedgridlist widgets/data/tst_themedgridlist.cpp qtmaterial3_widgets)
qtm3_add_test(tst_carousel widgets/data/tst_carousel.cpp qtmaterial3_widgets)
qtm3_add_test(tst_themedcarousel widgets/data/tst_themedcarousel.cpp qtmaterial3_widgets)
qtm3_add_test(tst_datacarouselresolver specs/tst_datacarouselresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_datawidgetresolvedspec specs/tst_datawidgetresolvedspec.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_datepickerresolvedspec specs/tst_datepickerresolvedspec.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_themeddatepicker widgets/data/tst_themeddatepicker.cpp qtmaterial3_widgets)
qtm3_add_test(tst_datepickerstability widgets/data/tst_datepickerstability.cpp qtmaterial3_widgets)
qtm3_add_test(tst_datatableresolver specs/tst_datatableresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_carousel_resolvedspec widgets/data/tst_carousel_resolvedspec.cpp qtmaterial3_widgets)

qtm3_add_test(tst_divider widgets/data/tst_divider.cpp qtmaterial3_widgets)
qtm3_add_test(tst_filledtextfield_render render/inputs/tst_filledtextfield_render.cpp qtmaterial3_widgets)

# Phase 5 note: add accessibility, keyboard, and installed-package consumer checks here.

qtm3_add_test(tst_filledbutton_render render/buttons/tst_filledbutton_render.cpp qtmaterial3_widgets)
qtm3_add_test(tst_outlinedbutton_render render/buttons/tst_outlinedbutton_render.cpp qtmaterial3_widgets)

qtm3_add_test(tst_outlinedtextfield_render render/inputs/tst_outlinedtextfield_render.cpp qtmaterial3_widgets)
qtm3_add_test(tst_themeserializer theme/tst_themeserializer.cpp qtmaterial3_theme)
qtm3_add_test(tst_themeserializer_readmodes theme/tst_themeserializer_readmodes.cpp qtmaterial3_theme)
qtm3_add_test(tst_theme_tokens theme/tst_theme_tokens.cpp qtmaterial3_theme)
qtm3_add_test(tst_theme_color_backend theme/tst_theme_color_backend.cpp qtmaterial3_theme)
qtm3_add_test(tst_theme_mcu_adapter theme/tst_theme_mcu_adapter.cpp qtmaterial3_theme)
qtm3_add_test(tst_theme_mcu_goldens theme/tst_theme_mcu_goldens.cpp qtmaterial3_theme)
target_compile_definitions(tst_theme_mcu_goldens PRIVATE QTMATERIAL3_TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
qtm3_add_test(tst_theme_seed_goldens theme/tst_theme_seed_goldens.cpp qtmaterial3_theme)
target_compile_definitions(tst_theme_seed_goldens PRIVATE QTMATERIAL3_TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

qtm3_add_test(tst_abstractbutton_state core/tst_abstractbutton_state.cpp qtmaterial3_core)


# qtm3_add_test(tst_overlaysurface core/tst_overlaysurface.cpp qtmaterial3_core)
#qtm3_add_test(tst_selectioncontrol_layout core/tst_selectioncontrol_layout.cpp qtmaterial3_core)
qtm3_add_test(tst_inputcontrol widgets/inputs/common/tst_inputcontrol.cpp qtmaterial3_widgets)
qtm3_add_test(tst_scrimwidget effects/tst_scrimwidget.cpp qtmaterial3_effects)
# qtm3_add_test(tst_filledtonalbutton_render tests/render/buttons/tst_filledtonalbutton_render.cpp qtmaterial3_widgets)
qtm3_add_test(tst_progressindicators widgets/progress/tst_progressindicators.cpp qtmaterial3_widgets)
qtm3_add_test(tst_tabs widgets/navigation/tst_tabs.cpp qtmaterial3_widgets)
qtm3_add_test(tst_themedmenu widgets/navigation/tst_themedmenu.cpp qtmaterial3_widgets)
qtm3_add_test(tst_tabs_themecontext widgets/navigation/tst_tabs_themecontext.cpp qtmaterial3_widgets)
qtm3_add_test(tst_tabs_route_robustness widgets/navigation/tst_tabs_route_robustness.cpp qtmaterial3_widgets)
qtm3_add_test(tst_navigationrail widgets/navigation/tst_navigationrail.cpp qtmaterial3_widgets)
qtm3_add_test(tst_navigationrailresolvedwidget widgets/navigation/tst_navigationrailresolvedwidget.cpp qtmaterial3_widgets)
qtm3_add_test(tst_navigationrail_resolvedspec widgets/navigation/tst_navigationrail_resolvedspec.cpp qtmaterial3_widgets)

#qtm3_add_test(tst_data_component_specs specs/tst_data_component_specs.cpp qtmaterial3_specs) # TODO
#qtm3_add_test(tst_data_components widgets/tst_data_components.cpp qtmaterial3_widgets) #TODO

# qtm3_add_test(tst_textfieldshellhelper widgets/inputs/tst_textfieldshellhelper.cpp qtmaterial3_widgets)

# comment don t compile TODO
# qtm3_add_test(tst_textfieldshellhelper widgets/inputs/tst_textfieldshellhelper.cpp qtmaterial3_widgets) 

# target_include_directories(tst_textfieldshellhelper PRIVATE
#     ${CMAKE_SOURCE_DIR}/src/widgets/inputs/private
# )

qtm3_add_test(tst_snackbar widgets/surfaces/tst_snackbar.cpp qtmaterial3_widgets)
qtm3_add_test(tst_snackbarlifecycle widgets/surfaces/tst_snackbarlifecycle.cpp qtmaterial3_widgets)
qtm3_add_test(tst_snackbarhost widgets/surfaces/tst_snackbarhost.cpp qtmaterial3_widgets)

qtm3_add_test(
    tst_keyboard_accessibility_contracts
    accessibility/tst_keyboard_accessibility_contracts.cpp
    qtmaterial3_widgets
    qtmaterial3_core
)
qtm3_add_test(
    tst_textfield_validation_signals
    widgets/inputs/tst_textfield_validation_signals.cpp
    qtmaterial3_widgets
)


qtm3_add_test(
    tst_textfield_error_display_policy
    widgets/inputs/tst_textfield_error_display_policy.cpp
    qtmaterial3_widgets
)

qtm3_add_test(
    tst_textfield_touched_state
    widgets/inputs/tst_textfield_touched_state.cpp
    qtmaterial3_widgets
)

qtm3_add_test(tst_textfield_dirty_state widgets/inputs/tst_textfield_dirty_state.cpp qtmaterial3_widgets)


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
qtm3_add_test(tst_systemtheme theme/tst_systemtheme.cpp qtmaterial3_theme Qt${QT_VERSION_MAJOR}::Widgets)
qtm3_add_test(tst_themevalidation theme/tst_themevalidation.cpp qtmaterial3_theme)
qtm3_add_test(tst_theme_accessibility_tokens theme/tst_theme_accessibility_tokens.cpp qtmaterial3_theme)
target_include_directories(tst_theme_accessibility_tokens PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
qtm3_add_test(tst_systemtheme_accessibility theme/tst_systemtheme_accessibility.cpp qtmaterial3_theme Qt${QT_VERSION_MAJOR}::Widgets)
qtm3_add_test(tst_keyboard_accessibility accessibility/tst_keyboard_accessibility.cpp Qt${QT_VERSION_MAJOR}::Widgets)

option(QTMATERIAL3_BUILD_VISUAL_REGRESSION_TESTS "Build Qt Material 3 visual regression tests" ON)
if(QTMATERIAL3_BUILD_VISUAL_REGRESSION_TESTS)
  qtm3_add_test(tst_theme_visual_regression visual/tst_theme_visual_regression.cpp qtmaterial3_theme qtmaterial3_widgets)
  target_compile_definitions(tst_theme_visual_regression PRIVATE
    QTMATERIAL3_VISUAL_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/visual")
endif()

# Public API guardrail: every public theming header must compile as a standalone include.
qtm3_add_test(tst_public_theming_headers public_headers/tst_public_theming_headers.cpp qtmaterial3_theme)
qtm3_add_test(tst_theme_performance_contracts performance/tst_theme_performance_contracts.cpp qtmaterial3_theme qtmaterial3_core qtmaterial3_specs)

# Release architecture cleanup tests.
qtm3_add_test(tst_interactionstate_contracts core/tst_interactionstate_contracts.cpp qtmaterial3_core qtmaterial3_testing)
qtm3_add_test(tst_automationmetadata testing/tst_automationmetadata.cpp qtmaterial3_testing)
qtm3_add_test(tst_metadataruntime core/tst_metadataruntime.cpp qtmaterial3_core)
qtm3_add_test(tst_specvalidation specs/tst_specvalidation.cpp qtmaterial3_theme qtmaterial3_core qtmaterial3_specs)
qtm3_add_test(tst_componentregistry tooling/tst_componentregistry.cpp qtmaterial3_tooling)
qtm3_add_test(tst_asyncstate core/tst_asyncstate.cpp qtmaterial3_core)
qtm3_add_test(tst_navigationmodel widgets/navigation/model/tst_navigationmodel.cpp qtmaterial3_widgets)
qtm3_add_test(tst_formfieldcontrol widgets/inputs/common/tst_formfieldcontrol.cpp qtmaterial3_widgets)
qtm3_add_test(tst_paletteadapter integration/tst_paletteadapter.cpp qtmaterial3_integration)
qtm3_add_test(tst_widget_migration_contracts widgets/tst_widget_migration_contracts.cpp qtmaterial3_widgets qtmaterial3_core qtmaterial3_testing)

qtm3_add_test(tst_textfield_validation_hardening widgets/inputs/tst_textfield_validation_hardening.cpp qtmaterial3_widgets)

qtm3_add_test(tst_textfield_error_visibility_accessibility widgets/inputs/tst_textfield_error_visibility_accessibility.cpp qtmaterial3_widgets)

qtm3_add_test(
    tst_textfield_readonly
    widgets/inputs/tst_textfield_readonly.cpp
    qtmaterial3_widgets
)

qtm3_add_test(
    tst_textfield_required
    widgets/inputs/tst_textfield_required.cpp
    qtmaterial3_widgets
)



qtm3_add_test(
    tst_navigation_dialog_hidpi_contracts
    accessibility/tst_navigation_dialog_hidpi_contracts.cpp
    qtmaterial3_widgets
    qtmaterial3_core
)


qtm3_add_test(
    tst_visual_smoke_contracts
    visual/tst_visual_smoke_contracts.cpp
    qtmaterial3_widgets
    qtmaterial3_core
)




qtm3_add_test(
    tst_textfield_character_counter
    widgets/inputs/tst_textfield_character_counter.cpp
    qtmaterial3_widgets
)

qtm3_add_test(
    tst_textfield_validation_commit
    widgets/inputs/tst_textfield_validation_commit.cpp
    qtmaterial3_widgets
)


qtm3_add_test(tst_button_icon_size_api widgets/buttons/tst_button_icon_size_api.cpp qtmaterial3_widgets)

# Architecture rules are executable release guardrails.
include(
    "${CMAKE_SOURCE_DIR}/cmake/QtMaterial3ArchitectureGuard.cmake"
)
qtmaterial3_add_architecture_guard()

qtm3_add_test(tst_datalistgridresolver specs/tst_datalistgridresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_data_list_grid_resolved widgets/data/tst_data_list_grid_resolved.cpp qtmaterial3_widgets)
qtm3_add_test(tst_compactspecresolver specs/tst_compactspecresolver.cpp qtmaterial3_theme qtmaterial3_specs)
qtm3_add_test(tst_compactcontrols widgets/compact/tst_compactcontrols.cpp qtmaterial3_widgets)
