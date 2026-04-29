#include "qtmaterial/theme/qtmaterialcolorscheme.h"

namespace QtMaterial {
namespace {

constexpr ColorRole kRequiredColorRoles[] = {
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
    ColorRole::Scrim,
};

} // namespace

ColorScheme::ColorScheme() = default;
ColorScheme::~ColorScheme() = default;

bool ColorScheme::contains(ColorRole role) const
{
    return m_colors.contains(role);
}

QColor ColorScheme::color(ColorRole role) const
{
    return m_colors.value(role, QColor());
}

void ColorScheme::setColor(ColorRole role, const QColor& color)
{
    m_colors.insert(role, color);
}

void ColorScheme::clear()
{
    m_colors.clear();
}

bool ColorScheme::isComplete() const
{
    return missingRoles().isEmpty();
}

QList<ColorRole> ColorScheme::missingRoles() const
{
    QList<ColorRole> missing;
    for (ColorRole role : kRequiredColorRoles) {
        if (!contains(role) || !color(role).isValid()) {
            missing.append(role);
        }
    }
    return missing;
}

} // namespace QtMaterial
