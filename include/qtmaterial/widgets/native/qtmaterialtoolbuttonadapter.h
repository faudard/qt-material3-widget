#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/qtmaterialglobal.h"

class QToolButton;
class QWidget;

namespace QtMaterial {

/**
 * Opt-in Material 3 rendering for an existing QToolButton.
 *
 * Default actions, menus, popup modes, autoRaise, checkable state,
 * toolButtonStyle, ownership and signals remain owned by QToolButton.
 */
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialToolButtonAdapter final
{
public:
    static void apply(
        QToolButton* button,
        ButtonVariant variant = ButtonVariant::Text,
        Density density = Density::Default);

    static void remove(QToolButton* button);
    static bool isApplied(const QToolButton* button);

    static void setVariant(
        QToolButton* button,
        ButtonVariant variant);
    static ButtonVariant variant(const QToolButton* button);

    static void setDensity(
        QToolButton* button,
        Density density);
    static Density density(const QToolButton* button);

    static void setOptOut(
        QToolButton* button,
        bool excluded);
    static bool isOptedOut(const QToolButton* button);

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
