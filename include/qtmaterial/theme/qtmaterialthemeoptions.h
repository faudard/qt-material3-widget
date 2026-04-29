#pragma once

#include <QColor>
#include <QtGlobal>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {

/**
 * User-facing preference for light/dark mode.
 *
 * ThemeMode remains the resolved mode stored in a built Theme. FollowSystem is
 * only a preference and must be resolved by SystemTheme/ThemeManager before a
 * Theme is built.
 */
enum class ThemePreference {
    Light,
    Dark,
    FollowSystem
};

/** Material dynamic color variant used when a seed color generates a scheme. */
enum class ThemeVariant {
    TonalSpot,
    Expressive
};

/**
 * Policy for selecting the color-generation backend.
 *
 * Auto picks Material Color Utilities when it is compiled in and otherwise uses
 * the deterministic fallback. ForceMaterialColorUtilities records a hard user
 * requirement and reports a diagnostic if this build cannot satisfy it.
 */
enum class ColorBackendPolicy {
    Auto,
    PreferMaterialColorUtilities,
    ForceMaterialColorUtilities,
    ForceFallback
};

inline bool isExpressiveThemeVariant(ThemeVariant variant) noexcept
{
    return variant == ThemeVariant::Expressive;
}

inline bool colorBackendPolicyAllowsMaterialColorUtilities(ColorBackendPolicy policy) noexcept
{
    return policy != ColorBackendPolicy::ForceFallback;
}

inline bool colorBackendPolicyRequiresMaterialColorUtilities(ColorBackendPolicy policy) noexcept
{
    return policy == ColorBackendPolicy::ForceMaterialColorUtilities;
}

class QTMATERIAL3_THEME_EXPORT ThemeOptions {
public:
    /** Canonical input structure used by ThemeBuilder. */
    ThemeOptions();
    ~ThemeOptions();

    QColor sourceColor;

    /** Resolved mode consumed by ThemeBuilder. Never set this to FollowSystem. */
    ThemeMode mode;

    /** User preference used by SystemTheme/ThemeManager settings. */
    ThemePreference preference;

    ContrastMode contrast;
    ThemeVariant variant;
    ColorBackendPolicy backendPolicy;

    bool operator==(const ThemeOptions& other) const noexcept
    {
        return sourceColor == other.sourceColor && mode == other.mode
            && preference == other.preference && contrast == other.contrast
            && variant == other.variant && backendPolicy == other.backendPolicy;
    }

    bool operator!=(const ThemeOptions& other) const noexcept
    {
        return !(*this == other);
    }
};

} // namespace QtMaterial

Q_DECLARE_METATYPE(QtMaterial::ThemePreference)
Q_DECLARE_METATYPE(QtMaterial::ThemeVariant)
Q_DECLARE_METATYPE(QtMaterial::ColorBackendPolicy)
