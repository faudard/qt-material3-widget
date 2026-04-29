#pragma once

#include <QMetaType>
#include <QtGlobal>

#include <type_traits>

namespace QtMaterial {

enum class ThemeMode {
    Light,
    Dark
};

enum class ContrastMode {
    Standard,
    Medium,
    High
};

enum class ColorRole {
    Primary,
    OnPrimary,
    PrimaryContainer,
    OnPrimaryContainer,
    PrimaryFixed,
    PrimaryFixedDim,
    OnPrimaryFixed,
    OnPrimaryFixedVariant,
    Secondary,
    OnSecondary,
    SecondaryContainer,
    OnSecondaryContainer,
    SecondaryFixed,
    SecondaryFixedDim,
    OnSecondaryFixed,
    OnSecondaryFixedVariant,
    Tertiary,
    OnTertiary,
    TertiaryContainer,
    OnTertiaryContainer,
    TertiaryFixed,
    TertiaryFixedDim,
    OnTertiaryFixed,
    OnTertiaryFixedVariant,
    Error,
    OnError,
    ErrorContainer,
    OnErrorContainer,
    Background,
    OnBackground,
    Surface,
    OnSurface,
    SurfaceDim,
    SurfaceBright,
    SurfaceContainerLowest,
    SurfaceContainerLow,
    SurfaceContainer,
    SurfaceContainerHigh,
    SurfaceContainerHighest,
    SurfaceVariant,
    OnSurfaceVariant,
    SurfaceTint,
    Outline,
    OutlineVariant,
    InverseSurface,
    InverseOnSurface,
    InversePrimary,
    Shadow,
    Scrim
};

enum class TypeRole {
    DisplayLarge,
    DisplayMedium,
    DisplaySmall,
    HeadlineLarge,
    HeadlineMedium,
    HeadlineSmall,
    TitleLarge,
    TitleMedium,
    TitleSmall,
    BodyLarge,
    BodyMedium,
    BodySmall,
    LabelLarge,
    LabelMedium,
    LabelSmall
};

enum class ShapeRole {
    None,
    ExtraSmall,
    Small,
    Medium,
    Large,
    ExtraLarge,
    Full
};

enum class ElevationRole {
    Level0,
    Level1,
    Level2,
    Level3,
    Level4,
    Level5
};

enum class MotionToken {
    Short1,
    Short2,
    Short3,
    Short4,
    Medium1,
    Medium2,
    Medium3,
    Medium4,
    Long1,
    Long2,
    Long3,
    Long4
};

enum class StateLayerRole {
    Hover,
    Focus,
    Press,
    Drag
};

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

template <typename Enum,
         typename = typename std::enable_if<std::is_enum<Enum>::value>::type>
inline size_t qtmaterialEnumHash(Enum value, size_t seed = 0) noexcept
{
    using Underlying = typename std::underlying_type<Enum>::type;
    return ::qHash(static_cast<Underlying>(value), seed);
}

inline size_t qHash(ThemeMode value, size_t seed = 0) noexcept
{
    return qtmaterialEnumHash(value, seed);
}

inline size_t qHash(ContrastMode value, size_t seed = 0) noexcept
{
    return qtmaterialEnumHash(value, seed);
}

inline size_t qHash(ColorRole value, size_t seed = 0) noexcept
{
    return qtmaterialEnumHash(value, seed);
}

inline size_t qHash(TypeRole value, size_t seed = 0) noexcept
{
    return qtmaterialEnumHash(value, seed);
}

inline size_t qHash(ShapeRole value, size_t seed = 0) noexcept
{
    return qtmaterialEnumHash(value, seed);
}

inline size_t qHash(ElevationRole value, size_t seed = 0) noexcept
{
    return qtmaterialEnumHash(value, seed);
}

inline size_t qHash(MotionToken value, size_t seed = 0) noexcept
{
    return qtmaterialEnumHash(value, seed);
}

inline size_t qHash(StateLayerRole value, size_t seed = 0) noexcept
{
    return qtmaterialEnumHash(value, seed);
}

#else

template <typename Enum,
         typename = typename std::enable_if<std::is_enum<Enum>::value>::type>
inline uint qtmaterialEnumHash(Enum value, uint seed = 0) noexcept
{
    using Underlying = typename std::underlying_type<Enum>::type;
    return ::qHash(static_cast<Underlying>(value), seed);
}

inline uint qHash(ThemeMode value, uint seed = 0) noexcept
{
    return qtmaterialEnumHash(value, seed);
}

inline uint qHash(ContrastMode value, uint seed = 0) noexcept
{
    return qtmaterialEnumHash(value, seed);
}

inline uint qHash(ColorRole value, uint seed = 0) noexcept
{
    return qtmaterialEnumHash(value, seed);
}

inline uint qHash(TypeRole value, uint seed = 0) noexcept
{
    return qtmaterialEnumHash(value, seed);
}

inline uint qHash(ShapeRole value, uint seed = 0) noexcept
{
    return qtmaterialEnumHash(value, seed);
}

inline uint qHash(ElevationRole value, uint seed = 0) noexcept
{
    return qtmaterialEnumHash(value, seed);
}

inline uint qHash(MotionToken value, uint seed = 0) noexcept
{
    return qtmaterialEnumHash(value, seed);
}

inline uint qHash(StateLayerRole value, uint seed = 0) noexcept
{
    return qtmaterialEnumHash(value, seed);
}

#endif

} // namespace QtMaterial

Q_DECLARE_METATYPE(QtMaterial::ThemeMode)
Q_DECLARE_METATYPE(QtMaterial::ContrastMode)
Q_DECLARE_METATYPE(QtMaterial::ColorRole)
Q_DECLARE_METATYPE(QtMaterial::TypeRole)
Q_DECLARE_METATYPE(QtMaterial::ShapeRole)
Q_DECLARE_METATYPE(QtMaterial::ElevationRole)
Q_DECLARE_METATYPE(QtMaterial::MotionToken)
Q_DECLARE_METATYPE(QtMaterial::StateLayerRole)