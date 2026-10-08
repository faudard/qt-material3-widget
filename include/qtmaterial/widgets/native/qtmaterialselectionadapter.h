#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/qtmaterialglobal.h"

class QCheckBox;
class QRadioButton;
class QWidget;

namespace QtMaterial {

/**
 * Opt-in Material 3 rendering for native QCheckBox and QRadioButton controls.
 *
 * Native ownership, signals, tristate/auto-exclusive semantics and widget
 * types stay unchanged. Full Material ripple/motion behavior remains the
 * responsibility of first-class QtMaterial selection widgets.
 */
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSelectionAdapter final
{
public:
    static void apply(
        QCheckBox* checkbox,
        Density density = Density::Default);
    static void apply(
        QRadioButton* radio,
        Density density = Density::Default);

    static void remove(QCheckBox* checkbox);
    static void remove(QRadioButton* radio);

    static bool isApplied(const QCheckBox* checkbox);
    static bool isApplied(const QRadioButton* radio);

    static void setDensity(QCheckBox* checkbox, Density density);
    static void setDensity(QRadioButton* radio, Density density);

    static Density density(const QCheckBox* checkbox);
    static Density density(const QRadioButton* radio);

    static void setOptOut(QCheckBox* checkbox, bool excluded);
    static void setOptOut(QRadioButton* radio, bool excluded);

    static bool isOptedOut(const QCheckBox* checkbox);
    static bool isOptedOut(const QRadioButton* radio);

    /**
     * Adapts all native QCheckBox/QRadioButton descendants. Controls with
     * qtm3MaterialOptOut=true are skipped. Returns the adapted control count.
     */
    static int applyToDescendants(
        QWidget* root,
        Density density = Density::Default);

    static const char* appliedPropertyName() noexcept;
    static const char* densityPropertyName() noexcept;
    static const char* optOutPropertyName() noexcept;
};

} // namespace QtMaterial
