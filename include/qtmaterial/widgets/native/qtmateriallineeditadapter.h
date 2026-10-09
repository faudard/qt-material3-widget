#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/qtmaterialglobal.h"

class QLineEdit;
class QWidget;

namespace QtMaterial {

/**
 * Opt-in Material 3 rendering for an existing QLineEdit.
 *
 * Validators, input masks, completers, echo modes, actions, text margins,
 * selection and signal semantics remain owned by QLineEdit.
 */
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialLineEditAdapter final
{
public:
    enum class Variant
    {
        Outlined,
        Filled
    };

    static void apply(
        QLineEdit* lineEdit,
        Variant variant = Variant::Outlined,
        Density density = Density::Default);

    static void remove(QLineEdit* lineEdit);
    static bool isApplied(const QLineEdit* lineEdit);

    static void setVariant(QLineEdit* lineEdit, Variant variant);
    static Variant variant(const QLineEdit* lineEdit);

    static void setDensity(QLineEdit* lineEdit, Density density);
    static Density density(const QLineEdit* lineEdit);

    static void setOptOut(QLineEdit* lineEdit, bool excluded);
    static bool isOptedOut(const QLineEdit* lineEdit);

    static int applyToDescendants(
        QWidget* root,
        Variant variant = Variant::Outlined,
        Density density = Density::Default);

    static const char* appliedPropertyName() noexcept;
    static const char* variantPropertyName() noexcept;
    static const char* densityPropertyName() noexcept;
    static const char* optOutPropertyName() noexcept;
};

} // namespace QtMaterial
