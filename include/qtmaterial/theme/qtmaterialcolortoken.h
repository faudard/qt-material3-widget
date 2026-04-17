#pragma once

#include <QtCore/QHash>
#include <QtCore/qglobal.h>
#include <QtCore/qhashfunctions.h>
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

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)

template <typename Enum>
inline uint qtmaterialEnumHash(Enum value, uint seed = 0) noexcept
{
    static_assert(std::is_enum<Enum>::value, "qtmaterialEnumHash requires an enum type");
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