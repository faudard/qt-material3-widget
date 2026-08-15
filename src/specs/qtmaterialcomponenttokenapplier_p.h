#pragma once

#include <QVector>

#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/specs/qtmaterialcardspec.h"
#include "qtmaterial/specs/qtmaterialchipspec.h"
#include "qtmaterial/specs/qtmaterialcheckboxspec.h"
#include "qtmaterial/specs/qtmaterialdialogspec.h"
#include "qtmaterial/specs/qtmaterialfabspec.h"
#include "qtmaterial/specs/qtmaterialiconbuttonspec.h"
#include "qtmaterial/specs/qtmaterialradiobuttonspec.h"
#include "qtmaterial/specs/qtmaterialsegmentedbuttonspec.h"
#include "qtmaterial/specs/qtmaterialsnackbarspec.h"
#include "qtmaterial/specs/qtmaterialswitchspec.h"
#include "qtmaterial/specs/qtmaterialtabsspec.h"
#include "qtmaterial/specs/qtmaterialtextfieldspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/specs/qtmaterialautocompletepopupspec.h"
#include "qtmaterial/specs/qtmaterialautocompletespec.h"
#include "qtmaterial/specs/qtmaterialdatefieldspec.h"
#include "qtmaterial/specs/qtmaterialnavigationrailspec.h"
#include "qtmaterial/specs/qtmaterialmenuspec.h"

namespace QtMaterial {

ComponentTokenOverride mergedComponentOverride(const Theme& theme, const QVector<ComponentId>& componentIds);

void applyButtonComponentTokens(const Theme& theme, const QVector<ComponentId>& componentIds, ButtonSpec* spec);
void applyFabComponentTokens(const Theme& theme, const QVector<ComponentId>& componentIds, FabSpec* spec);
void applyIconButtonComponentTokens(const Theme& theme, const QVector<ComponentId>& componentIds, IconButtonSpec* spec);
void applyCheckboxComponentTokens(const Theme& theme, const QVector<ComponentId>& componentIds, CheckboxSpec* spec);
void applyRadioButtonComponentTokens(const Theme& theme, const QVector<ComponentId>& componentIds, RadioButtonSpec* spec);
void applySwitchComponentTokens(const Theme& theme, const QVector<ComponentId>& componentIds, SwitchSpec* spec);
void applyAutocompletePopupComponentTokens(const Theme& theme, const QVector<ComponentId>& componentIds, AutocompletePopupSpec* spec);
void applyDateFieldComponentTokens(const Theme& theme, const QVector<ComponentId>& componentIds, DateFieldSpec* spec);
void applyNavigationRailComponentTokens(const Theme& theme, const QVector<ComponentId>& componentIds, NavigationRailSpec* spec);
void applyTextFieldComponentTokens(const Theme& theme, const QVector<ComponentId>& componentIds, TextFieldSpec* spec);
void applyCardComponentTokens(const Theme& theme, const QVector<ComponentId>& componentIds, CardSpec* spec);
void applyDialogComponentTokens(const Theme& theme, const QVector<ComponentId>& componentIds, DialogSpec* spec);

void applyChipComponentTokens(
    const Theme& theme,
    const QVector<ComponentId>& componentIds,
    ChipSpec* spec);
void applySegmentedButtonComponentTokens(
    const Theme& theme,
    const QVector<ComponentId>& componentIds,
    SegmentedButtonSpec* spec);

void applyAutocompleteComponentTokens(
    const Theme& theme,
    const QVector<ComponentId>& componentIds,
    AutocompleteSpec* spec);

void applyMenuComponentTokens(
    const Theme& theme,
    const QVector<ComponentId>& componentIds,
    MenuSpec* spec);

void applySnackbarComponentTokens(
    const Theme& theme,
    const QVector<ComponentId>& componentIds,
    SnackbarSpec* spec);

void applyTabsComponentTokens(
    const Theme& theme,
    const QVector<ComponentId>& componentIds,
    TabsSpec* spec);

} // namespace QtMaterial
