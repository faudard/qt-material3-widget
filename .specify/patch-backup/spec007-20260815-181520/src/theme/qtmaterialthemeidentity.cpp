#include "qtmaterial/theme/qtmaterialthemeidentity.h"

#include "qtmaterial/theme/qtmaterialtheme.h"

#include <QList>

#include <algorithm>
#include <array>
#include <type_traits>

namespace QtMaterial {
namespace {

constexpr std::array<ColorRole, 49> kColorRoles = {
    ColorRole::Primary,
    ColorRole::OnPrimary,
    ColorRole::PrimaryContainer,
    ColorRole::OnPrimaryContainer,
    ColorRole::PrimaryFixed,
    ColorRole::PrimaryFixedDim,
    ColorRole::OnPrimaryFixed,
    ColorRole::OnPrimaryFixedVariant,
    ColorRole::Secondary,
    ColorRole::OnSecondary,
    ColorRole::SecondaryContainer,
    ColorRole::OnSecondaryContainer,
    ColorRole::SecondaryFixed,
    ColorRole::SecondaryFixedDim,
    ColorRole::OnSecondaryFixed,
    ColorRole::OnSecondaryFixedVariant,
    ColorRole::Tertiary,
    ColorRole::OnTertiary,
    ColorRole::TertiaryContainer,
    ColorRole::OnTertiaryContainer,
    ColorRole::TertiaryFixed,
    ColorRole::TertiaryFixedDim,
    ColorRole::OnTertiaryFixed,
    ColorRole::OnTertiaryFixedVariant,
    ColorRole::Error,
    ColorRole::OnError,
    ColorRole::ErrorContainer,
    ColorRole::OnErrorContainer,
    ColorRole::Background,
    ColorRole::OnBackground,
    ColorRole::Surface,
    ColorRole::OnSurface,
    ColorRole::SurfaceDim,
    ColorRole::SurfaceBright,
    ColorRole::SurfaceContainerLowest,
    ColorRole::SurfaceContainerLow,
    ColorRole::SurfaceContainer,
    ColorRole::SurfaceContainerHigh,
    ColorRole::SurfaceContainerHighest,
    ColorRole::SurfaceVariant,
    ColorRole::OnSurfaceVariant,
    ColorRole::SurfaceTint,
    ColorRole::Outline,
    ColorRole::OutlineVariant,
    ColorRole::InverseSurface,
    ColorRole::InverseOnSurface,
    ColorRole::InversePrimary,
    ColorRole::Shadow,
    ColorRole::Scrim
};
constexpr std::size_t kCanonicalColorRoleCount = kColorRoles.size();

constexpr std::array<TypeRole, 15> kTypeRoles = {
    TypeRole::DisplayLarge, TypeRole::DisplayMedium, TypeRole::DisplaySmall,
    TypeRole::HeadlineLarge, TypeRole::HeadlineMedium, TypeRole::HeadlineSmall,
    TypeRole::TitleLarge, TypeRole::TitleMedium, TypeRole::TitleSmall,
    TypeRole::BodyLarge, TypeRole::BodyMedium, TypeRole::BodySmall,
    TypeRole::LabelLarge, TypeRole::LabelMedium, TypeRole::LabelSmall
};

constexpr std::array<ShapeRole, 7> kShapeRoles = {
    ShapeRole::None, ShapeRole::ExtraSmall, ShapeRole::Small,
    ShapeRole::Medium, ShapeRole::Large, ShapeRole::ExtraLarge, ShapeRole::Full
};

constexpr std::array<ElevationRole, 6> kElevationRoles = {
    ElevationRole::Level0, ElevationRole::Level1, ElevationRole::Level2,
    ElevationRole::Level3, ElevationRole::Level4, ElevationRole::Level5
};

constexpr std::array<MotionToken, 12> kMotionTokens = {
    MotionToken::Short1, MotionToken::Short2, MotionToken::Short3, MotionToken::Short4,
    MotionToken::Medium1, MotionToken::Medium2, MotionToken::Medium3, MotionToken::Medium4,
    MotionToken::Long1, MotionToken::Long2, MotionToken::Long3, MotionToken::Long4
};

constexpr std::array<DensityRole, 3> kDensityRoles = {
    DensityRole::Compact, DensityRole::Default, DensityRole::Comfortable
};

constexpr std::array<IconSizeRole, 5> kIconSizeRoles = {
    IconSizeRole::ExtraSmall, IconSizeRole::Small, IconSizeRole::Medium,
    IconSizeRole::Large, IconSizeRole::ExtraLarge
};

bool equalTypographyStyle(const TypographyStyle& lhs, const TypographyStyle& rhs)
{
    return lhs.font == rhs.font
        && lhs.lineHeight == rhs.lineHeight
        && lhs.letterSpacing == rhs.letterSpacing;
}

bool equalElevationStyle(const ElevationStyle& lhs, const ElevationStyle& rhs)
{
    return lhs.shadowBlur == rhs.shadowBlur
        && lhs.shadowYOffset == rhs.shadowYOffset
        && lhs.tonalOverlayOpacity == rhs.tonalOverlayOpacity;
}

bool equalMotionStyle(const MotionStyle& lhs, const MotionStyle& rhs)
{
    return lhs.durationMs == rhs.durationMs
        && lhs.easing == rhs.easing;
}

bool equalStateLayer(const StateLayer& lhs, const StateLayer& rhs)
{
    return lhs.color == rhs.color
        && lhs.hoverOpacity == rhs.hoverOpacity
        && lhs.focusOpacity == rhs.focusOpacity
        && lhs.pressOpacity == rhs.pressOpacity
        && lhs.dragOpacity == rhs.dragOpacity;
}

bool equalFocusRing(const FocusRingTokens& lhs, const FocusRingTokens& rhs)
{
    return lhs.width == rhs.width
        && lhs.offset == rhs.offset
        && lhs.radiusAdjustment == rhs.radiusAdjustment
        && lhs.color == rhs.color
        && lhs.opacity == rhs.opacity;
}

bool equalAccessibility(const AccessibilityTokens& lhs, const AccessibilityTokens& rhs)
{
    return lhs.highContrast == rhs.highContrast
        && lhs.reducedMotion == rhs.reducedMotion
        && lhs.minimumTextContrastRatio == rhs.minimumTextContrastRatio
        && lhs.minimumUiContrastRatio == rhs.minimumUiContrastRatio
        && equalFocusRing(lhs.focusRing, rhs.focusRing);
}

bool equalInteractions(const InteractionStateTokens& lhs, const InteractionStateTokens& rhs)
{
    return lhs.keyboardFocusVisible == rhs.keyboardFocusVisible
        && lhs.strongFocusIndicators == rhs.strongFocusIndicators
        && lhs.hoverFeedbackEnabled == rhs.hoverFeedbackEnabled
        && lhs.pressFeedbackEnabled == rhs.pressFeedbackEnabled
        && lhs.dragFeedbackEnabled == rhs.dragFeedbackEnabled;
}

bool equalColorScheme(const ColorScheme& lhs, const ColorScheme& rhs)
{
    for (std::size_t i = 0; i < kCanonicalColorRoleCount; ++i) {
        const ColorRole role = kColorRoles[i];
        if (lhs.contains(role) != rhs.contains(role)) {
            return false;
        }
        if (lhs.contains(role) && lhs.color(role) != rhs.color(role)) {
            return false;
        }
    }
    return true;
}

bool equalTypography(const TypographyScale& lhs, const TypographyScale& rhs)
{
    for (TypeRole role : kTypeRoles) {
        if (lhs.contains(role) != rhs.contains(role)) {
            return false;
        }
        if (lhs.contains(role)
            && !equalTypographyStyle(lhs.style(role), rhs.style(role))) {
            return false;
        }
    }

    // Compatibility aliases are currently public mutable state and therefore
    // participate in Theme value identity until they are removed/migrated.
    return lhs.displayLarge == rhs.displayLarge
        && lhs.displayMedium == rhs.displayMedium
        && lhs.displaySmall == rhs.displaySmall
        && lhs.headlineLarge == rhs.headlineLarge
        && lhs.headlineMedium == rhs.headlineMedium
        && lhs.headlineSmall == rhs.headlineSmall
        && lhs.titleLarge == rhs.titleLarge
        && lhs.titleMedium == rhs.titleMedium
        && lhs.titleSmall == rhs.titleSmall
        && lhs.bodyLarge == rhs.bodyLarge
        && lhs.bodyMedium == rhs.bodyMedium
        && lhs.bodySmall == rhs.bodySmall
        && lhs.labelLarge == rhs.labelLarge
        && lhs.labelMedium == rhs.labelMedium
        && lhs.labelSmall == rhs.labelSmall;
}

bool equalShapes(const ShapeScale& lhs, const ShapeScale& rhs)
{
    for (ShapeRole role : kShapeRoles) {
        if (lhs.contains(role) != rhs.contains(role)) {
            return false;
        }
        if (lhs.contains(role) && lhs.radius(role) != rhs.radius(role)) {
            return false;
        }
    }
    return true;
}

bool equalElevations(const ElevationScale& lhs, const ElevationScale& rhs)
{
    for (ElevationRole role : kElevationRoles) {
        if (lhs.contains(role) != rhs.contains(role)) {
            return false;
        }
        if (lhs.contains(role)
            && !equalElevationStyle(lhs.style(role), rhs.style(role))) {
            return false;
        }
    }
    return true;
}

bool equalMotion(const MotionTokens& lhs, const MotionTokens& rhs)
{
    for (MotionToken token : kMotionTokens) {
        if (lhs.contains(token) != rhs.contains(token)) {
            return false;
        }
        if (lhs.contains(token)
            && !equalMotionStyle(lhs.style(token), rhs.style(token))) {
            return false;
        }
    }
    return true;
}

bool equalDensity(const DensityTokens& lhs, const DensityTokens& rhs)
{
    for (DensityRole role : kDensityRoles) {
        if (lhs.contains(role) != rhs.contains(role)) {
            return false;
        }
        if (lhs.contains(role) && lhs.value(role) != rhs.value(role)) {
            return false;
        }
    }
    return true;
}

bool equalIconSizes(const IconSizeTokens& lhs, const IconSizeTokens& rhs)
{
    for (IconSizeRole role : kIconSizeRoles) {
        if (lhs.contains(role) != rhs.contains(role)) {
            return false;
        }
        if (lhs.contains(role) && lhs.size(role) != rhs.size(role)) {
            return false;
        }
    }
    return true;
}

template <typename Enum, typename Value, typename Equal>
bool equalEnumHash(const QHash<Enum, Value>& lhs,
                   const QHash<Enum, Value>& rhs,
                   Equal equal)
{
    if (lhs.size() != rhs.size()) {
        return false;
    }
    QList<Enum> keys = lhs.keys();
    std::sort(keys.begin(), keys.end(), [](Enum a, Enum b) {
        using U = typename std::underlying_type<Enum>::type;
        return static_cast<U>(a) < static_cast<U>(b);
    });
    for (Enum key : keys) {
        if (!rhs.contains(key) || !equal(lhs.value(key), rhs.value(key))) {
            return false;
        }
    }
    return true;
}

bool equalOverride(const ComponentTokenOverride& lhs,
                   const ComponentTokenOverride& rhs)
{
    if (!equalEnumHash(lhs.colors, rhs.colors,
                       [](const QColor& a, const QColor& b) { return a == b; })) {
        return false;
    }
    if (!equalEnumHash(lhs.typography, rhs.typography, equalTypographyStyle)) {
        return false;
    }
    if (lhs.shapes != rhs.shapes) {
        return false;
    }
    if (!equalEnumHash(lhs.elevations, rhs.elevations, equalElevationStyle)) {
        return false;
    }
    if (!equalEnumHash(lhs.motion, rhs.motion, equalMotionStyle)) {
        return false;
    }
    if (lhs.density != rhs.density || lhs.iconSizes != rhs.iconSizes) {
        return false;
    }
    if (lhs.hasStateLayer != rhs.hasStateLayer) {
        return false;
    }
    if (lhs.hasStateLayer && !equalStateLayer(lhs.stateLayer, rhs.stateLayer)) {
        return false;
    }
    return lhs.custom == rhs.custom;
}

bool equalComponentOverrides(const ComponentTokenOverrides& lhs,
                             const ComponentTokenOverrides& rhs)
{
    const QStringList lhsNames = lhs.componentNames();
    const QStringList rhsNames = rhs.componentNames();
    if (lhsNames != rhsNames) {
        return false;
    }
    for (const QString& name : lhsNames) {
        if (!equalOverride(lhs.overrideFor(name), rhs.overrideFor(name))) {
            return false;
        }
    }
    return true;
}

} // namespace

bool operator==(const Theme& lhs, const Theme& rhs)
{
    return lhs.mode() == rhs.mode()
        && lhs.contrastMode() == rhs.contrastMode()
        && lhs.options() == rhs.options()
        && equalColorScheme(lhs.colorScheme(), rhs.colorScheme())
        && equalTypography(lhs.typography(), rhs.typography())
        && equalShapes(lhs.shapes(), rhs.shapes())
        && equalElevations(lhs.elevations(), rhs.elevations())
        && equalMotion(lhs.motion(), rhs.motion())
        && equalStateLayer(lhs.stateLayer(), rhs.stateLayer())
        && equalAccessibility(lhs.accessibility(), rhs.accessibility())
        && equalInteractions(lhs.interactions(), rhs.interactions())
        && equalDensity(lhs.density(), rhs.density())
        && equalIconSizes(lhs.iconSizes(), rhs.iconSizes())
        && equalComponentOverrides(lhs.componentOverrides(), rhs.componentOverrides());
}

bool operator!=(const Theme& lhs, const Theme& rhs)
{
    return !(lhs == rhs);
}

} // namespace QtMaterial
