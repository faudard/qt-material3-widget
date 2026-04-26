#include "qtmaterialmcuadapter_p.h"

#include "qtmaterial/theme/qtmaterialcolorbackend.h"

#include <QColor>
#include <QtGlobal>

#if defined(QTMATERIAL3_HAS_MCU) && QTMATERIAL3_HAS_MCU
#include "cpp/cam/hct.h"
#include "cpp/dynamiccolor/material_dynamic_colors.h"
#include "cpp/scheme/scheme_expressive.h"
#include "cpp/scheme/scheme_tonal_spot.h"
#include "cpp/utils/utils.h"
#endif

namespace QtMaterial::detail {

bool mcuAdapterCompiledIn() noexcept
{
#if defined(QTMATERIAL3_HAS_MCU) && QTMATERIAL3_HAS_MCU
    return true;
#else
    return false;
#endif
}

#if defined(QTMATERIAL3_HAS_MCU) && QTMATERIAL3_HAS_MCU
namespace {

using material_color_utilities::Argb;
using material_color_utilities::DynamicScheme;
using material_color_utilities::Hct;
using material_color_utilities::MaterialDynamicColors;
using material_color_utilities::SchemeExpressive;
using material_color_utilities::SchemeTonalSpot;

Argb toArgb(const QColor& color)
{
    const QColor rgb = color.toRgb();
    return material_color_utilities::ArgbFromRgb(rgb.red(), rgb.green(), rgb.blue());
}

QColor fromArgb(Argb argb)
{
    return QColor(material_color_utilities::RedFromInt(argb),
                  material_color_utilities::GreenFromInt(argb),
                  material_color_utilities::BlueFromInt(argb),
                  material_color_utilities::AlphaFromInt(argb));
}

template <typename DynamicColorFactory>
void setDynamicColor(ColorScheme& scheme,
                     ColorRole role,
                     const DynamicScheme& dynamicScheme,
                     DynamicColorFactory factory)
{
    auto dynamicColor = factory();
    scheme.setColor(role, fromArgb(dynamicColor.GetArgb(dynamicScheme)));
}

ColorScheme colorSchemeFromDynamicScheme(const DynamicScheme& dynamicScheme)
{
    ColorScheme scheme;

    setDynamicColor(scheme, ColorRole::Primary, dynamicScheme, MaterialDynamicColors::Primary);
    setDynamicColor(scheme, ColorRole::OnPrimary, dynamicScheme, MaterialDynamicColors::OnPrimary);
    setDynamicColor(scheme, ColorRole::PrimaryContainer, dynamicScheme, MaterialDynamicColors::PrimaryContainer);
    setDynamicColor(scheme, ColorRole::OnPrimaryContainer, dynamicScheme, MaterialDynamicColors::OnPrimaryContainer);
    setDynamicColor(scheme, ColorRole::PrimaryFixed, dynamicScheme, MaterialDynamicColors::PrimaryFixed);
    setDynamicColor(scheme, ColorRole::PrimaryFixedDim, dynamicScheme, MaterialDynamicColors::PrimaryFixedDim);
    setDynamicColor(scheme, ColorRole::OnPrimaryFixed, dynamicScheme, MaterialDynamicColors::OnPrimaryFixed);
    setDynamicColor(scheme, ColorRole::OnPrimaryFixedVariant, dynamicScheme, MaterialDynamicColors::OnPrimaryFixedVariant);

    setDynamicColor(scheme, ColorRole::Secondary, dynamicScheme, MaterialDynamicColors::Secondary);
    setDynamicColor(scheme, ColorRole::OnSecondary, dynamicScheme, MaterialDynamicColors::OnSecondary);
    setDynamicColor(scheme, ColorRole::SecondaryContainer, dynamicScheme, MaterialDynamicColors::SecondaryContainer);
    setDynamicColor(scheme, ColorRole::OnSecondaryContainer, dynamicScheme, MaterialDynamicColors::OnSecondaryContainer);
    setDynamicColor(scheme, ColorRole::SecondaryFixed, dynamicScheme, MaterialDynamicColors::SecondaryFixed);
    setDynamicColor(scheme, ColorRole::SecondaryFixedDim, dynamicScheme, MaterialDynamicColors::SecondaryFixedDim);
    setDynamicColor(scheme, ColorRole::OnSecondaryFixed, dynamicScheme, MaterialDynamicColors::OnSecondaryFixed);
    setDynamicColor(scheme, ColorRole::OnSecondaryFixedVariant, dynamicScheme, MaterialDynamicColors::OnSecondaryFixedVariant);

    setDynamicColor(scheme, ColorRole::Tertiary, dynamicScheme, MaterialDynamicColors::Tertiary);
    setDynamicColor(scheme, ColorRole::OnTertiary, dynamicScheme, MaterialDynamicColors::OnTertiary);
    setDynamicColor(scheme, ColorRole::TertiaryContainer, dynamicScheme, MaterialDynamicColors::TertiaryContainer);
    setDynamicColor(scheme, ColorRole::OnTertiaryContainer, dynamicScheme, MaterialDynamicColors::OnTertiaryContainer);
    setDynamicColor(scheme, ColorRole::TertiaryFixed, dynamicScheme, MaterialDynamicColors::TertiaryFixed);
    setDynamicColor(scheme, ColorRole::TertiaryFixedDim, dynamicScheme, MaterialDynamicColors::TertiaryFixedDim);
    setDynamicColor(scheme, ColorRole::OnTertiaryFixed, dynamicScheme, MaterialDynamicColors::OnTertiaryFixed);
    setDynamicColor(scheme, ColorRole::OnTertiaryFixedVariant, dynamicScheme, MaterialDynamicColors::OnTertiaryFixedVariant);

    setDynamicColor(scheme, ColorRole::Error, dynamicScheme, MaterialDynamicColors::Error);
    setDynamicColor(scheme, ColorRole::OnError, dynamicScheme, MaterialDynamicColors::OnError);
    setDynamicColor(scheme, ColorRole::ErrorContainer, dynamicScheme, MaterialDynamicColors::ErrorContainer);
    setDynamicColor(scheme, ColorRole::OnErrorContainer, dynamicScheme, MaterialDynamicColors::OnErrorContainer);

    setDynamicColor(scheme, ColorRole::Background, dynamicScheme, MaterialDynamicColors::Background);
    setDynamicColor(scheme, ColorRole::OnBackground, dynamicScheme, MaterialDynamicColors::OnBackground);
    setDynamicColor(scheme, ColorRole::Surface, dynamicScheme, MaterialDynamicColors::Surface);
    setDynamicColor(scheme, ColorRole::OnSurface, dynamicScheme, MaterialDynamicColors::OnSurface);
    setDynamicColor(scheme, ColorRole::SurfaceDim, dynamicScheme, MaterialDynamicColors::SurfaceDim);
    setDynamicColor(scheme, ColorRole::SurfaceBright, dynamicScheme, MaterialDynamicColors::SurfaceBright);
    setDynamicColor(scheme, ColorRole::SurfaceContainerLowest, dynamicScheme, MaterialDynamicColors::SurfaceContainerLowest);
    setDynamicColor(scheme, ColorRole::SurfaceContainerLow, dynamicScheme, MaterialDynamicColors::SurfaceContainerLow);
    setDynamicColor(scheme, ColorRole::SurfaceContainer, dynamicScheme, MaterialDynamicColors::SurfaceContainer);
    setDynamicColor(scheme, ColorRole::SurfaceContainerHigh, dynamicScheme, MaterialDynamicColors::SurfaceContainerHigh);
    setDynamicColor(scheme, ColorRole::SurfaceContainerHighest, dynamicScheme, MaterialDynamicColors::SurfaceContainerHighest);
    setDynamicColor(scheme, ColorRole::SurfaceVariant, dynamicScheme, MaterialDynamicColors::SurfaceVariant);
    setDynamicColor(scheme, ColorRole::OnSurfaceVariant, dynamicScheme, MaterialDynamicColors::OnSurfaceVariant);
    setDynamicColor(scheme, ColorRole::SurfaceTint, dynamicScheme, MaterialDynamicColors::SurfaceTint);
    setDynamicColor(scheme, ColorRole::Outline, dynamicScheme, MaterialDynamicColors::Outline);
    setDynamicColor(scheme, ColorRole::OutlineVariant, dynamicScheme, MaterialDynamicColors::OutlineVariant);
    setDynamicColor(scheme, ColorRole::InverseSurface, dynamicScheme, MaterialDynamicColors::InverseSurface);
    setDynamicColor(scheme, ColorRole::InverseOnSurface, dynamicScheme, MaterialDynamicColors::InverseOnSurface);
    setDynamicColor(scheme, ColorRole::InversePrimary, dynamicScheme, MaterialDynamicColors::InversePrimary);
    setDynamicColor(scheme, ColorRole::Shadow, dynamicScheme, MaterialDynamicColors::Shadow);
    setDynamicColor(scheme, ColorRole::Scrim, dynamicScheme, MaterialDynamicColors::Scrim);

    return scheme;
}

McuColorSchemeResult generateWithSchemeType(const QColor& seed,
                                            ThemeMode mode,
                                            ContrastMode contrast,
                                            bool expressive)
{
    McuColorSchemeResult result;

    if (!seed.isValid()) {
        result.diagnostic = QStringLiteral("Invalid seed color; MCU generation was not attempted.");
        return result;
    }

    const Hct sourceHct(toArgb(seed));
    const bool isDark = mode == ThemeMode::Dark;
    const double contrastLevel = contrastModeToMaterialColorUtilitiesLevel(contrast);

    if (expressive) {
        SchemeExpressive dynamicScheme(sourceHct, isDark, contrastLevel);
        result.colorScheme = colorSchemeFromDynamicScheme(dynamicScheme);
    } else {
        SchemeTonalSpot dynamicScheme(sourceHct, isDark, contrastLevel);
        result.colorScheme = colorSchemeFromDynamicScheme(dynamicScheme);
    }

    result.ok = true;
    result.diagnostic = expressive
        ? QStringLiteral("Generated color scheme with MCU SchemeExpressive.")
        : QStringLiteral("Generated color scheme with MCU SchemeTonalSpot.");
    return result;
}

} // namespace
#endif

McuColorSchemeResult generateMcuColorScheme(const ThemeOptions& options)
{
    return generateMcuColorScheme(options.sourceColor,
                                  options.mode,
                                  options.contrast,
                                  options.expressive);
}

McuColorSchemeResult generateMcuColorScheme(const QColor& seed,
                                            ThemeMode mode,
                                            ContrastMode contrast,
                                            bool expressive)
{
#if defined(QTMATERIAL3_HAS_MCU) && QTMATERIAL3_HAS_MCU
    return generateWithSchemeType(seed, mode, contrast, expressive);
#else
    Q_UNUSED(seed);
    Q_UNUSED(mode);
    Q_UNUSED(contrast);
    Q_UNUSED(expressive);

    McuColorSchemeResult result;
    result.diagnostic = QStringLiteral("Material Color Utilities adapter is not compiled into this build.");
    return result;
#endif
}

} // namespace QtMaterial::detail
