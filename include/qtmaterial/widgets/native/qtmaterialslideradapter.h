#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/qtmaterialglobal.h"

class QSlider;
class QWidget;

namespace QtMaterial {

/**
 * Opt-in Material 3 rendering for an existing QSlider.
 *
 * The adapter preserves the native QSlider object, range/value, orientation,
 * tick settings, inversion flags, signals and keyboard/mouse behavior. Material
 * geometry is exposed through the same QStyle sub-control contract used by
 * QSlider, so painting and hit testing remain aligned.
 */
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSliderAdapter final
{
public:
    static void apply(
        QSlider* slider,
        Density density = Density::Default);

    static void remove(QSlider* slider);
    static bool isApplied(const QSlider* slider);

    static void setDensity(QSlider* slider, Density density);
    static Density density(const QSlider* slider);

    static void setOptOut(QSlider* slider, bool excluded);
    static bool isOptedOut(const QSlider* slider);

    static int applyToDescendants(
        QWidget* root,
        Density density = Density::Default);

    static const char* appliedPropertyName() noexcept;
    static const char* densityPropertyName() noexcept;
    static const char* optOutPropertyName() noexcept;
};

} // namespace QtMaterial
