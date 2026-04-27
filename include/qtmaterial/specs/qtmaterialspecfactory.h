#pragma once

#include "qtmaterial/core/qtmaterialdensity.h"
#include "qtmaterial/specs/qtmaterialautocompletepopupspec.h"
#include "qtmaterial/specs/qtmaterialautocompletespec.h"
#include "qtmaterial/specs/qtmaterialappbarspec.h"
#include "qtmaterial/specs/qtmaterialbannerspec.h"
#include "qtmaterial/specs/qtmaterialbottomsheetspec.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/specs/qtmaterialcardspec.h"
#include "qtmaterial/specs/qtmaterialcheckboxspec.h"
#include "qtmaterial/specs/qtmaterialchipspec.h"
#include "qtmaterial/specs/qtmaterialdatefieldspec.h"
#include "qtmaterial/specs/qtmaterialdialogspec.h"
#include "qtmaterial/specs/qtmaterialdividerspec.h"
#include "qtmaterial/specs/qtmaterialfabspec.h"
#include "qtmaterial/specs/qtmaterialiconbuttonspec.h"
#include "qtmaterial/specs/qtmateriallistitemspec.h"
#include "qtmaterial/specs/qtmaterialmenuspec.h"
#include "qtmaterial/specs/qtmaterialnavigationdrawerspec.h"
#include "qtmaterial/specs/qtmaterialnavigationrailspec.h"
#include "qtmaterial/specs/qtmaterialradiobuttonspec.h"
#include "qtmaterial/specs/qtmaterialsegmentedbuttonspec.h"
#include "qtmaterial/specs/qtmaterialsnackbarspec.h"
#include "qtmaterial/specs/qtmaterialswitchspec.h"
#include "qtmaterial/specs/qtmaterialtextfieldspec.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

class QTMATERIAL3_SPECS_EXPORT SpecFactory {
public:
    SpecFactory();
    ~SpecFactory();

    ButtonSpec textButtonSpec(const Theme& theme, Density density = Density::Default) const;
    ButtonSpec filledButtonSpec(const Theme& theme, Density density = Density::Default) const;
    ButtonSpec filledTonalButtonSpec(const Theme& theme, Density density = Density::Default) const;
    ButtonSpec outlinedButtonSpec(const Theme& theme, Density density = Density::Default) const;
    ButtonSpec elevatedButtonSpec(const Theme& theme, Density density = Density::Default) const;
    FabSpec fabSpec(const Theme& theme, Density density = Density::Default) const;
    FabSpec extendedFabSpec(const Theme& theme, Density density = Density::Default) const;
    IconButtonSpec iconButtonSpec(const Theme& theme, Density density = Density::Default) const;
    CheckboxSpec checkboxSpec(const Theme& theme, Density density = Density::Default) const;
    RadioButtonSpec radioButtonSpec(const Theme& theme, Density density = Density::Default) const;
    SwitchSpec switchSpec(const Theme& theme, Density density = Density::Default) const;
    DialogSpec dialogSpec(const Theme& theme) const;
    NavigationDrawerSpec navigationDrawerSpec(const Theme& theme) const;
    BottomSheetSpec bottomSheetSpec(const Theme& theme) const;
    BannerSpec bannerSpec(const Theme& theme) const;
    CardSpec cardSpec(const Theme& theme) const;
    AppBarSpec topAppBarSpec(const Theme& theme) const;
    AppBarSpec bottomAppBarSpec(const Theme& theme) const;
    TextFieldSpec outlinedTextFieldSpec(const Theme& theme, Density density = Density::Default) const;
    TextFieldSpec filledTextFieldSpec(const Theme& theme, Density density = Density::Default) const;
    AutocompletePopupSpec autocompletePopupSpec(const Theme& theme) const;
    DateFieldSpec dateFieldSpec(const Theme& theme) const;
    ListItemSpec listItemSpec(const Theme& theme) const;
    SnackbarSpec snackbarSpec(const Theme& theme) const;
    DividerSpec dividerSpec(const Theme& theme) const;

    ChipSpec assistChipSpec(const Theme& theme, Density density = Density::Default) const;
    ChipSpec filterChipSpec(const Theme& theme, Density density = Density::Default) const;
    ChipSpec inputChipSpec(const Theme& theme, Density density = Density::Default) const;
    ChipSpec suggestionChipSpec(const Theme& theme, Density density = Density::Default) const;
    SegmentedButtonSpec segmentedButtonSpec(const Theme& theme, Density density = Density::Default) const;
    NavigationRailSpec navigationRailSpec(const Theme& theme) const;
    MenuSpec menuSpec(const Theme& theme, Density density = Density::Default) const;
    AutocompleteSpec autocompleteSpec(const Theme& theme, Density density = Density::Default) const;

private:
    int buttonHeightForDensity(Density density) const;
};

} // namespace QtMaterial
