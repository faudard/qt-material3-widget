#pragma once

#include <array>
#include <type_traits>
#include <QtGlobal>

#include "qtmaterial/foundation/qtmaterialtokenid.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialdensitytokens.h"
#include "qtmaterial/theme/qtmaterialiconsizetokens.h"

namespace QtMaterial {

enum class AccessibilityRole : std::uint32_t {
    HighContrast = 1u,
    ReducedMotion = 2u,
    MinimumTextContrastRatio = 3u,
    MinimumUiContrastRatio = 4u,
    FocusRingWidth = 5u,
    FocusRingOffset = 6u,
    FocusRingRadiusAdjustment = 7u,
    FocusRingColor = 8u,
    FocusRingOpacity = 9u
};

enum class InteractionRole : std::uint32_t {
    KeyboardFocusVisible = 1u,
    StrongFocusIndicators = 2u,
    HoverFeedbackEnabled = 3u,
    PressFeedbackEnabled = 4u,
    DragFeedbackEnabled = 5u
};

constexpr TokenId tokenId(ColorRole value) noexcept
{
    switch (value) {
    case ColorRole::Primary: return TokenId::fromParts(TokenCategory::Color, 1u);
    case ColorRole::OnPrimary: return TokenId::fromParts(TokenCategory::Color, 2u);
    case ColorRole::PrimaryContainer: return TokenId::fromParts(TokenCategory::Color, 3u);
    case ColorRole::OnPrimaryContainer: return TokenId::fromParts(TokenCategory::Color, 4u);
    case ColorRole::PrimaryFixed: return TokenId::fromParts(TokenCategory::Color, 5u);
    case ColorRole::PrimaryFixedDim: return TokenId::fromParts(TokenCategory::Color, 6u);
    case ColorRole::OnPrimaryFixed: return TokenId::fromParts(TokenCategory::Color, 7u);
    case ColorRole::OnPrimaryFixedVariant: return TokenId::fromParts(TokenCategory::Color, 8u);
    case ColorRole::Secondary: return TokenId::fromParts(TokenCategory::Color, 9u);
    case ColorRole::OnSecondary: return TokenId::fromParts(TokenCategory::Color, 10u);
    case ColorRole::SecondaryContainer: return TokenId::fromParts(TokenCategory::Color, 11u);
    case ColorRole::OnSecondaryContainer: return TokenId::fromParts(TokenCategory::Color, 12u);
    case ColorRole::SecondaryFixed: return TokenId::fromParts(TokenCategory::Color, 13u);
    case ColorRole::SecondaryFixedDim: return TokenId::fromParts(TokenCategory::Color, 14u);
    case ColorRole::OnSecondaryFixed: return TokenId::fromParts(TokenCategory::Color, 15u);
    case ColorRole::OnSecondaryFixedVariant: return TokenId::fromParts(TokenCategory::Color, 16u);
    case ColorRole::Tertiary: return TokenId::fromParts(TokenCategory::Color, 17u);
    case ColorRole::OnTertiary: return TokenId::fromParts(TokenCategory::Color, 18u);
    case ColorRole::TertiaryContainer: return TokenId::fromParts(TokenCategory::Color, 19u);
    case ColorRole::OnTertiaryContainer: return TokenId::fromParts(TokenCategory::Color, 20u);
    case ColorRole::TertiaryFixed: return TokenId::fromParts(TokenCategory::Color, 21u);
    case ColorRole::TertiaryFixedDim: return TokenId::fromParts(TokenCategory::Color, 22u);
    case ColorRole::OnTertiaryFixed: return TokenId::fromParts(TokenCategory::Color, 23u);
    case ColorRole::OnTertiaryFixedVariant: return TokenId::fromParts(TokenCategory::Color, 24u);
    case ColorRole::Error: return TokenId::fromParts(TokenCategory::Color, 25u);
    case ColorRole::OnError: return TokenId::fromParts(TokenCategory::Color, 26u);
    case ColorRole::ErrorContainer: return TokenId::fromParts(TokenCategory::Color, 27u);
    case ColorRole::OnErrorContainer: return TokenId::fromParts(TokenCategory::Color, 28u);
    case ColorRole::Background: return TokenId::fromParts(TokenCategory::Color, 29u);
    case ColorRole::OnBackground: return TokenId::fromParts(TokenCategory::Color, 30u);
    case ColorRole::Surface: return TokenId::fromParts(TokenCategory::Color, 31u);
    case ColorRole::OnSurface: return TokenId::fromParts(TokenCategory::Color, 32u);
    case ColorRole::SurfaceDim: return TokenId::fromParts(TokenCategory::Color, 33u);
    case ColorRole::SurfaceBright: return TokenId::fromParts(TokenCategory::Color, 34u);
    case ColorRole::SurfaceContainerLowest: return TokenId::fromParts(TokenCategory::Color, 35u);
    case ColorRole::SurfaceContainerLow: return TokenId::fromParts(TokenCategory::Color, 36u);
    case ColorRole::SurfaceContainer: return TokenId::fromParts(TokenCategory::Color, 37u);
    case ColorRole::SurfaceContainerHigh: return TokenId::fromParts(TokenCategory::Color, 38u);
    case ColorRole::SurfaceContainerHighest: return TokenId::fromParts(TokenCategory::Color, 39u);
    case ColorRole::SurfaceVariant: return TokenId::fromParts(TokenCategory::Color, 40u);
    case ColorRole::OnSurfaceVariant: return TokenId::fromParts(TokenCategory::Color, 41u);
    case ColorRole::SurfaceTint: return TokenId::fromParts(TokenCategory::Color, 42u);
    case ColorRole::Outline: return TokenId::fromParts(TokenCategory::Color, 43u);
    case ColorRole::OutlineVariant: return TokenId::fromParts(TokenCategory::Color, 44u);
    case ColorRole::InverseSurface: return TokenId::fromParts(TokenCategory::Color, 45u);
    case ColorRole::InverseOnSurface: return TokenId::fromParts(TokenCategory::Color, 46u);
    case ColorRole::InversePrimary: return TokenId::fromParts(TokenCategory::Color, 47u);
    case ColorRole::Shadow: return TokenId::fromParts(TokenCategory::Color, 48u);
    case ColorRole::Scrim: return TokenId::fromParts(TokenCategory::Color, 49u);
    }
    return TokenId();
}

constexpr TokenId tokenId(TypeRole value) noexcept
{
    switch (value) {
    case TypeRole::DisplayLarge: return TokenId::fromParts(TokenCategory::Typography, 1u);
    case TypeRole::DisplayMedium: return TokenId::fromParts(TokenCategory::Typography, 2u);
    case TypeRole::DisplaySmall: return TokenId::fromParts(TokenCategory::Typography, 3u);
    case TypeRole::HeadlineLarge: return TokenId::fromParts(TokenCategory::Typography, 4u);
    case TypeRole::HeadlineMedium: return TokenId::fromParts(TokenCategory::Typography, 5u);
    case TypeRole::HeadlineSmall: return TokenId::fromParts(TokenCategory::Typography, 6u);
    case TypeRole::TitleLarge: return TokenId::fromParts(TokenCategory::Typography, 7u);
    case TypeRole::TitleMedium: return TokenId::fromParts(TokenCategory::Typography, 8u);
    case TypeRole::TitleSmall: return TokenId::fromParts(TokenCategory::Typography, 9u);
    case TypeRole::BodyLarge: return TokenId::fromParts(TokenCategory::Typography, 10u);
    case TypeRole::BodyMedium: return TokenId::fromParts(TokenCategory::Typography, 11u);
    case TypeRole::BodySmall: return TokenId::fromParts(TokenCategory::Typography, 12u);
    case TypeRole::LabelLarge: return TokenId::fromParts(TokenCategory::Typography, 13u);
    case TypeRole::LabelMedium: return TokenId::fromParts(TokenCategory::Typography, 14u);
    case TypeRole::LabelSmall: return TokenId::fromParts(TokenCategory::Typography, 15u);
    }
    return TokenId();
}

constexpr TokenId tokenId(ShapeRole value) noexcept
{
    switch (value) {
    case ShapeRole::None: return TokenId::fromParts(TokenCategory::Shape, 1u);
    case ShapeRole::ExtraSmall: return TokenId::fromParts(TokenCategory::Shape, 2u);
    case ShapeRole::Small: return TokenId::fromParts(TokenCategory::Shape, 3u);
    case ShapeRole::Medium: return TokenId::fromParts(TokenCategory::Shape, 4u);
    case ShapeRole::Large: return TokenId::fromParts(TokenCategory::Shape, 5u);
    case ShapeRole::ExtraLarge: return TokenId::fromParts(TokenCategory::Shape, 6u);
    case ShapeRole::Full: return TokenId::fromParts(TokenCategory::Shape, 7u);
    }
    return TokenId();
}

constexpr TokenId tokenId(ElevationRole value) noexcept
{
    switch (value) {
    case ElevationRole::Level0: return TokenId::fromParts(TokenCategory::Elevation, 1u);
    case ElevationRole::Level1: return TokenId::fromParts(TokenCategory::Elevation, 2u);
    case ElevationRole::Level2: return TokenId::fromParts(TokenCategory::Elevation, 3u);
    case ElevationRole::Level3: return TokenId::fromParts(TokenCategory::Elevation, 4u);
    case ElevationRole::Level4: return TokenId::fromParts(TokenCategory::Elevation, 5u);
    case ElevationRole::Level5: return TokenId::fromParts(TokenCategory::Elevation, 6u);
    }
    return TokenId();
}

constexpr TokenId tokenId(MotionToken value) noexcept
{
    switch (value) {
    case MotionToken::Short1: return TokenId::fromParts(TokenCategory::Motion, 1u);
    case MotionToken::Short2: return TokenId::fromParts(TokenCategory::Motion, 2u);
    case MotionToken::Short3: return TokenId::fromParts(TokenCategory::Motion, 3u);
    case MotionToken::Short4: return TokenId::fromParts(TokenCategory::Motion, 4u);
    case MotionToken::Medium1: return TokenId::fromParts(TokenCategory::Motion, 5u);
    case MotionToken::Medium2: return TokenId::fromParts(TokenCategory::Motion, 6u);
    case MotionToken::Medium3: return TokenId::fromParts(TokenCategory::Motion, 7u);
    case MotionToken::Medium4: return TokenId::fromParts(TokenCategory::Motion, 8u);
    case MotionToken::Long1: return TokenId::fromParts(TokenCategory::Motion, 9u);
    case MotionToken::Long2: return TokenId::fromParts(TokenCategory::Motion, 10u);
    case MotionToken::Long3: return TokenId::fromParts(TokenCategory::Motion, 11u);
    case MotionToken::Long4: return TokenId::fromParts(TokenCategory::Motion, 12u);
    }
    return TokenId();
}

constexpr TokenId tokenId(StateLayerRole value) noexcept
{
    switch (value) {
    case StateLayerRole::Hover: return TokenId::fromParts(TokenCategory::StateLayer, 1u);
    case StateLayerRole::Focus: return TokenId::fromParts(TokenCategory::StateLayer, 2u);
    case StateLayerRole::Press: return TokenId::fromParts(TokenCategory::StateLayer, 3u);
    case StateLayerRole::Drag: return TokenId::fromParts(TokenCategory::StateLayer, 4u);
    }
    return TokenId();
}

constexpr TokenId tokenId(DensityRole value) noexcept
{
    switch (value) {
    case DensityRole::Compact: return TokenId::fromParts(TokenCategory::Density, 1u);
    case DensityRole::Default: return TokenId::fromParts(TokenCategory::Density, 2u);
    case DensityRole::Comfortable: return TokenId::fromParts(TokenCategory::Density, 3u);
    }
    return TokenId();
}

constexpr TokenId tokenId(IconSizeRole value) noexcept
{
    switch (value) {
    case IconSizeRole::ExtraSmall: return TokenId::fromParts(TokenCategory::IconSize, 1u);
    case IconSizeRole::Small: return TokenId::fromParts(TokenCategory::IconSize, 2u);
    case IconSizeRole::Medium: return TokenId::fromParts(TokenCategory::IconSize, 3u);
    case IconSizeRole::Large: return TokenId::fromParts(TokenCategory::IconSize, 4u);
    case IconSizeRole::ExtraLarge: return TokenId::fromParts(TokenCategory::IconSize, 5u);
    }
    return TokenId();
}

constexpr TokenId tokenId(AccessibilityRole value) noexcept
{
    return TokenId::fromParts(
        TokenCategory::Accessibility,
        static_cast<std::uint32_t>(value));
}

constexpr TokenId tokenId(InteractionRole value) noexcept
{
    return TokenId::fromParts(
        TokenCategory::Interaction,
        static_cast<std::uint32_t>(value));
}

inline constexpr std::array<ColorRole, 49> allColorRoles() noexcept
{
    return { ColorRole::Primary, ColorRole::OnPrimary, ColorRole::PrimaryContainer, ColorRole::OnPrimaryContainer, ColorRole::PrimaryFixed, ColorRole::PrimaryFixedDim, ColorRole::OnPrimaryFixed, ColorRole::OnPrimaryFixedVariant, ColorRole::Secondary, ColorRole::OnSecondary, ColorRole::SecondaryContainer, ColorRole::OnSecondaryContainer, ColorRole::SecondaryFixed, ColorRole::SecondaryFixedDim, ColorRole::OnSecondaryFixed, ColorRole::OnSecondaryFixedVariant, ColorRole::Tertiary, ColorRole::OnTertiary, ColorRole::TertiaryContainer, ColorRole::OnTertiaryContainer, ColorRole::TertiaryFixed, ColorRole::TertiaryFixedDim, ColorRole::OnTertiaryFixed, ColorRole::OnTertiaryFixedVariant, ColorRole::Error, ColorRole::OnError, ColorRole::ErrorContainer, ColorRole::OnErrorContainer, ColorRole::Background, ColorRole::OnBackground, ColorRole::Surface, ColorRole::OnSurface, ColorRole::SurfaceDim, ColorRole::SurfaceBright, ColorRole::SurfaceContainerLowest, ColorRole::SurfaceContainerLow, ColorRole::SurfaceContainer, ColorRole::SurfaceContainerHigh, ColorRole::SurfaceContainerHighest, ColorRole::SurfaceVariant, ColorRole::OnSurfaceVariant, ColorRole::SurfaceTint, ColorRole::Outline, ColorRole::OutlineVariant, ColorRole::InverseSurface, ColorRole::InverseOnSurface, ColorRole::InversePrimary, ColorRole::Shadow, ColorRole::Scrim };
}

inline constexpr std::array<TypeRole, 15> allTypeRoles() noexcept
{
    return { TypeRole::DisplayLarge, TypeRole::DisplayMedium, TypeRole::DisplaySmall, TypeRole::HeadlineLarge, TypeRole::HeadlineMedium, TypeRole::HeadlineSmall, TypeRole::TitleLarge, TypeRole::TitleMedium, TypeRole::TitleSmall, TypeRole::BodyLarge, TypeRole::BodyMedium, TypeRole::BodySmall, TypeRole::LabelLarge, TypeRole::LabelMedium, TypeRole::LabelSmall };
}

inline constexpr std::array<ShapeRole, 7> allShapeRoles() noexcept
{
    return { ShapeRole::None, ShapeRole::ExtraSmall, ShapeRole::Small, ShapeRole::Medium, ShapeRole::Large, ShapeRole::ExtraLarge, ShapeRole::Full };
}

inline constexpr std::array<ElevationRole, 6> allElevationRoles() noexcept
{
    return { ElevationRole::Level0, ElevationRole::Level1, ElevationRole::Level2, ElevationRole::Level3, ElevationRole::Level4, ElevationRole::Level5 };
}

inline constexpr std::array<MotionToken, 12> allMotionTokens() noexcept
{
    return { MotionToken::Short1, MotionToken::Short2, MotionToken::Short3, MotionToken::Short4, MotionToken::Medium1, MotionToken::Medium2, MotionToken::Medium3, MotionToken::Medium4, MotionToken::Long1, MotionToken::Long2, MotionToken::Long3, MotionToken::Long4 };
}

inline constexpr std::array<StateLayerRole, 4> allStateLayerRoles() noexcept
{
    return { StateLayerRole::Hover, StateLayerRole::Focus, StateLayerRole::Press, StateLayerRole::Drag };
}

inline constexpr std::array<DensityRole, 3> allDensityRoles() noexcept
{
    return { DensityRole::Compact, DensityRole::Default, DensityRole::Comfortable };
}

inline constexpr std::array<IconSizeRole, 5> allIconSizeRoles() noexcept
{
    return { IconSizeRole::ExtraSmall, IconSizeRole::Small, IconSizeRole::Medium, IconSizeRole::Large, IconSizeRole::ExtraLarge };
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
inline size_t qHash(TokenId id, size_t seed = 0) noexcept
{
    return ::qHash(id.raw(), seed);
}
#else
inline uint qHash(TokenId id, uint seed = 0) noexcept
{
    return ::qHash(id.raw(), seed);
}
#endif

} // namespace QtMaterial
