#pragma once

#include <QtCore/QHash>
#include <type_traits>

namespace QtMaterial {

enum class ThemeMode { Light, Dark };
enum class ContrastMode { Standard, Medium, High };

enum class ColorRole {
    Primary, OnPrimary, PrimaryContainer, OnPrimaryContainer,
    Secondary, OnSecondary, SecondaryContainer, OnSecondaryContainer,
    Tertiary, OnTertiary, TertiaryContainer, OnTertiaryContainer,
    Error, OnError, ErrorContainer, OnErrorContainer,
    Background, OnBackground,
    Surface, OnSurface, SurfaceDim, SurfaceBright,
    SurfaceContainerLowest, SurfaceContainerLow, SurfaceContainer,
    SurfaceContainerHigh, SurfaceContainerHighest,
    SurfaceVariant, OnSurfaceVariant,
    Outline, OutlineVariant,
    InverseSurface, InverseOnSurface, InversePrimary,
    Shadow, Scrim
};

enum class TypeRole {
    DisplayLarge, DisplayMedium, DisplaySmall,
    HeadlineLarge, HeadlineMedium, HeadlineSmall,
    TitleLarge, TitleMedium, TitleSmall,
    BodyLarge, BodyMedium, BodySmall,
    LabelLarge, LabelMedium, LabelSmall
};

enum class ShapeRole { None, ExtraSmall, Small, Medium, Large, ExtraLarge, Full };
enum class ElevationRole { Level0, Level1, Level2, Level3, Level4, Level5 };
enum class MotionToken { Short1, Short2, Short3, Short4, Medium1, Medium2, Medium3, Medium4, Long1, Long2, Long3, Long4 };
enum class StateLayerRole { Hover, Focus, Press, Drag };

} // namespace QtMaterial

template <typename Enum, typename = std::enable_if_t<std::is_enum_v<Enum>>>
inline size_t qtmaterialEnumHash(Enum value, size_t seed = 0) noexcept
{
    using Underlying = std::underlying_type_t<Enum>;
    return qHash(static_cast<Underlying>(value), seed);
}

inline size_t qHash(QtMaterial::ThemeMode value, size_t seed = 0) noexcept { return qtmaterialEnumHash(value, seed); }
inline size_t qHash(QtMaterial::ContrastMode value, size_t seed = 0) noexcept { return qtmaterialEnumHash(value, seed); }
inline size_t qHash(QtMaterial::ColorRole value, size_t seed = 0) noexcept { return qtmaterialEnumHash(value, seed); }
inline size_t qHash(QtMaterial::TypeRole value, size_t seed = 0) noexcept { return qtmaterialEnumHash(value, seed); }
inline size_t qHash(QtMaterial::ShapeRole value, size_t seed = 0) noexcept { return qtmaterialEnumHash(value, seed); }
inline size_t qHash(QtMaterial::ElevationRole value, size_t seed = 0) noexcept { return qtmaterialEnumHash(value, seed); }
inline size_t qHash(QtMaterial::MotionToken value, size_t seed = 0) noexcept { return qtmaterialEnumHash(value, seed); }
inline size_t qHash(QtMaterial::StateLayerRole value, size_t seed = 0) noexcept { return qtmaterialEnumHash(value, seed); }
