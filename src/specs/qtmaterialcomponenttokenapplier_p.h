#pragma once

#include <QStringList>

#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/specs/qtmaterialcardspec.h"
#include "qtmaterial/specs/qtmaterialcheckboxspec.h"
#include "qtmaterial/specs/qtmaterialdialogspec.h"
#include "qtmaterial/specs/qtmaterialfabspec.h"
#include "qtmaterial/specs/qtmaterialiconbuttonspec.h"
#include "qtmaterial/specs/qtmaterialradiobuttonspec.h"
#include "qtmaterial/specs/qtmaterialswitchspec.h"
#include "qtmaterial/specs/qtmaterialtextfieldspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

ComponentTokenOverride mergedComponentOverride(const Theme& theme, const QStringList& componentNames);

void applyButtonComponentTokens(const Theme& theme, const QStringList& componentNames, ButtonSpec* spec);
void applyFabComponentTokens(const Theme& theme, const QStringList& componentNames, FabSpec* spec);
void applyIconButtonComponentTokens(const Theme& theme, const QStringList& componentNames, IconButtonSpec* spec);
void applyCheckboxComponentTokens(const Theme& theme, const QStringList& componentNames, CheckboxSpec* spec);
void applyRadioButtonComponentTokens(const Theme& theme, const QStringList& componentNames, RadioButtonSpec* spec);
void applySwitchComponentTokens(const Theme& theme, const QStringList& componentNames, SwitchSpec* spec);
void applyTextFieldComponentTokens(const Theme& theme, const QStringList& componentNames, TextFieldSpec* spec);
void applyCardComponentTokens(const Theme& theme, const QStringList& componentNames, CardSpec* spec);
void applyDialogComponentTokens(const Theme& theme, const QStringList& componentNames, DialogSpec* spec);

} // namespace QtMaterial
