#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"

class QPushButton;
class QWidget;

namespace QtMaterial {

/**
 * Opt-in Material 3 styling for an existing QPushButton.
 *
 * The adapter preserves QPushButton ownership, signals and native behavior. It
 * changes only the per-widget style and a small set of namespaced dynamic
 * properties. Use QtMaterial*Button classes when full Material ripple/motion
 * behavior is required.
 */
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialButtonAdapter final
{
public:
    static void apply(
        QPushButton* button,
        ButtonVariant variant = ButtonVariant::Text,
        Density density = Density::Default);

    static void remove(QPushButton* button);
    static bool isApplied(const QPushButton* button);

    static void setVariant(QPushButton* button, ButtonVariant variant);
    static ButtonVariant variant(const QPushButton* button);

    static void setDensity(QPushButton* button, Density density);
    static Density density(const QPushButton* button);

    static void setOptOut(QPushButton* button, bool excluded);
    static bool isOptedOut(const QPushButton* button);

    /**
     * Applies Material styling to every QPushButton below root, including root
     * itself when it is a QPushButton. Buttons with qtm3MaterialOptOut=true are
     * skipped. Returns the number of eligible buttons adapted.
     */
    static int applyToDescendants(
        QWidget* root,
        ButtonVariant variant = ButtonVariant::Text,
        Density density = Density::Default);

    static const char* appliedPropertyName() noexcept;
    static const char* variantPropertyName() noexcept;
    static const char* densityPropertyName() noexcept;
    static const char* optOutPropertyName() noexcept;
};

} // namespace QtMaterial
