#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/qtmaterialglobal.h"

class QComboBox;
class QWidget;

namespace QtMaterial {

/**
 * Opt-in Material 3 rendering for an existing QComboBox.
 *
 * The adapter preserves QComboBox ownership of its model/view contract,
 * application-provided delegates, editable line edits, popup behavior and
 * selection signals. Only the closed-field QStyle rendering/geometry is
 * replaced.
 */
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialComboBoxAdapter final
{
public:
    static void apply(
        QComboBox* comboBox,
        Density density = Density::Default);

    static void remove(QComboBox* comboBox);
    static bool isApplied(const QComboBox* comboBox);

    static void setDensity(QComboBox* comboBox, Density density);
    static Density density(const QComboBox* comboBox);

    static void setOptOut(QComboBox* comboBox, bool excluded);
    static bool isOptedOut(const QComboBox* comboBox);

    static int applyToDescendants(
        QWidget* root,
        Density density = Density::Default);

    static const char* appliedPropertyName() noexcept;
    static const char* densityPropertyName() noexcept;
    static const char* optOutPropertyName() noexcept;
};

} // namespace QtMaterial
