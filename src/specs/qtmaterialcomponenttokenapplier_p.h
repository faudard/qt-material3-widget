#pragma once

#include <QStringList>

#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/specs/qtmaterialcardspec.h"
#include "qtmaterial/specs/qtmaterialchipspec.h"
#include "qtmaterial/specs/qtmaterialcheckboxspec.h"
#include "qtmaterial/specs/qtmaterialdialogspec.h"
#include "qtmaterial/specs/qtmaterialfabspec.h"
#include "qtmaterial/specs/qtmaterialiconbuttonspec.h"
#include "qtmaterial/specs/qtmaterialradiobuttonspec.h"
#include "qtmaterial/specs/qtmaterialsegmentedbuttonspec.h"
#include "qtmaterial/specs/qtmaterialswitchspec.h"
#include "qtmaterial/specs/qtmaterialtextfieldspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/specs/qtmaterialautocompletepopupspec.h"
#include "qtmaterial/specs/qtmaterialautocompletespec.h"
#include "qtmaterial/specs/qtmaterialdatefieldspec.h"
#include "qtmaterial/specs/qtmaterialnavigationrailspec.h"
#include "qtmaterial/specs/qtmaterialmenuspec.h"

namespace QtMaterial {

ComponentTokenOverride mergedComponentOverride(const Theme& theme, const QStringList& componentNames);

void applyButtonComponentTokens(const Theme& theme, const QStringList& componentNames, ButtonSpec* spec);
void applyFabComponentTokens(const Theme& theme, const QStringList& componentNames, FabSpec* spec);
void applyIconButtonComponentTokens(const Theme& theme, const QStringList& componentNames, IconButtonSpec* spec);
void applyCheckboxComponentTokens(const Theme& theme, const QStringList& componentNames, CheckboxSpec* spec);
void applyRadioButtonComponentTokens(const Theme& theme, const QStringList& componentNames, RadioButtonSpec* spec);
void applySwitchComponentTokens(const Theme& theme, const QStringList& componentNames, SwitchSpec* spec);
void applyAutocompletePopupComponentTokens(const Theme& theme, const QStringList& componentNames, AutocompletePopupSpec* spec);
void applyDateFieldComponentTokens(const Theme& theme, const QStringList& componentNames, DateFieldSpec* spec);
void applyNavigationRailComponentTokens(const Theme& theme, const QStringList& componentNames, NavigationRailSpec* spec);
void applyTextFieldComponentTokens(const Theme& theme, const QStringList& componentNames, TextFieldSpec* spec);
void applyCardComponentTokens(const Theme& theme, const QStringList& componentNames, CardSpec* spec);
void applyDialogComponentTokens(const Theme& theme, const QStringList& componentNames, DialogSpec* spec);

void applyChipComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    ChipSpec* spec);
void applySegmentedButtonComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    SegmentedButtonSpec* spec);

void applyAutocompleteComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    AutocompleteSpec* spec);

void applyMenuComponentTokens(
    const Theme& theme,
    const QStringList& componentNames,
    MenuSpec* spec);

} // namespace QtMaterial
