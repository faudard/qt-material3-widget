#include "qtmaterial/theme/qtmaterialthemebuilder.h"

#include <QColor>

namespace QtMaterial {
namespace {
QColor tone(const QColor& base, qreal factorTowardWhite) {
    QColor result = base;
    result.setRed(result.red() + static_cast<int>((255 - result.red()) * factorTowardWhite));
    result.setGreen(result.green() + static_cast<int>((255 - result.green()) * factorTowardWhite));
    result.setBlue(result.blue() + static_cast<int>((255 - result.blue()) * factorTowardWhite));
    return result;
}
QColor shade(const QColor& base, qreal factorTowardBlack) {
    QColor result = base;
    result.setRed(static_cast<int>(result.red() * (1.0 - factorTowardBlack)));
    result.setGreen(static_cast<int>(result.green() * (1.0 - factorTowardBlack)));
    result.setBlue(static_cast<int>(result.blue() * (1.0 - factorTowardBlack)));
    return result;
}
TypographyStyle makeTypographyStyle(const QString& family, int pointSize, int weight, qreal lineHeight, qreal letterSpacing) {
    TypographyStyle style;
    style.font = QFont(family, pointSize, weight);
    style.lineHeight = lineHeight;
    style.letterSpacing = letterSpacing;
    return style;
}
} // namespace

ThemeBuilder::ThemeBuilder() = default;
ThemeBuilder::~ThemeBuilder() = default;

Theme ThemeBuilder::build(const ThemeOptions& options) const {
    return buildFallbackTheme(options);
}

Theme ThemeBuilder::buildLightFromSeed(const QColor& seed) const {
    ThemeOptions options;
    options.sourceColor = seed;
    options.mode = ThemeMode::Light;
    return build(options);
}

Theme ThemeBuilder::buildDarkFromSeed(const QColor& seed) const {
    ThemeOptions options;
    options.sourceColor = seed;
    options.mode = ThemeMode::Dark;
    return build(options);
}

Theme ThemeBuilder::buildFallbackTheme(const ThemeOptions& options) const {
    Theme theme(options);
    if (options.mode == ThemeMode::Dark) {
        applyFallbackDarkScheme(theme, options.sourceColor);
    } else {
        applyFallbackLightScheme(theme, options.sourceColor);
    }
    applyDefaultTypography(theme);
    applyDefaultShapes(theme);
    applyDefaultElevations(theme);
    applyDefaultMotion(theme);
    applyDefaultStateLayer(theme);
    return theme;
}

void ThemeBuilder::applyDefaultTypography(Theme& theme) const {
    TypographyScale& scale = theme.typography();
    const QString family = QStringLiteral("Sans Serif");
    scale.setStyle(TypeRole::DisplayLarge, makeTypographyStyle(family, 36, QFont::Normal, 44.0, 0.0));
    scale.setStyle(TypeRole::DisplayMedium, makeTypographyStyle(family, 32, QFont::Normal, 40.0, 0.0));
    scale.setStyle(TypeRole::DisplaySmall, makeTypographyStyle(family, 28, QFont::Normal, 36.0, 0.0));
    scale.setStyle(TypeRole::HeadlineLarge, makeTypographyStyle(family, 24, QFont::Normal, 32.0, 0.0));
    scale.setStyle(TypeRole::HeadlineMedium, makeTypographyStyle(family, 22, QFont::Normal, 28.0, 0.0));
    scale.setStyle(TypeRole::HeadlineSmall, makeTypographyStyle(family, 20, QFont::Normal, 24.0, 0.0));
    scale.setStyle(TypeRole::TitleLarge, makeTypographyStyle(family, 18, QFont::DemiBold, 24.0, 0.0));
    scale.setStyle(TypeRole::TitleMedium, makeTypographyStyle(family, 16, QFont::DemiBold, 20.0, 0.1));
    scale.setStyle(TypeRole::TitleSmall, makeTypographyStyle(family, 14, QFont::DemiBold, 20.0, 0.1));
    scale.setStyle(TypeRole::BodyLarge, makeTypographyStyle(family, 16, QFont::Normal, 24.0, 0.1));
    scale.setStyle(TypeRole::BodyMedium, makeTypographyStyle(family, 14, QFont::Normal, 20.0, 0.25));
    scale.setStyle(TypeRole::BodySmall, makeTypographyStyle(family, 12, QFont::Normal, 16.0, 0.4));
    scale.setStyle(TypeRole::LabelLarge, makeTypographyStyle(family, 14, QFont::DemiBold, 20.0, 0.1));
    scale.setStyle(TypeRole::LabelMedium, makeTypographyStyle(family, 12, QFont::DemiBold, 16.0, 0.5));
    scale.setStyle(TypeRole::LabelSmall, makeTypographyStyle(family, 11, QFont::DemiBold, 16.0, 0.5));
}

void ThemeBuilder::applyDefaultShapes(Theme& theme) const {
    ShapeScale& shapes = theme.shapes();
    shapes.setRadius(ShapeRole::None, 0);
    shapes.setRadius(ShapeRole::ExtraSmall, 4);
    shapes.setRadius(ShapeRole::Small, 8);
    shapes.setRadius(ShapeRole::Medium, 12);
    shapes.setRadius(ShapeRole::Large, 16);
    shapes.setRadius(ShapeRole::ExtraLarge, 28);
    shapes.setRadius(ShapeRole::Full, 9999);
}

void ThemeBuilder::applyDefaultElevations(Theme& theme) const {
    ElevationScale& elevations = theme.elevations();
    elevations.setStyle(ElevationRole::Level0, {0, 0, 0.00});
    elevations.setStyle(ElevationRole::Level1, {4, 1, 0.05});
    elevations.setStyle(ElevationRole::Level2, {8, 2, 0.08});
    elevations.setStyle(ElevationRole::Level3, {12, 4, 0.11});
    elevations.setStyle(ElevationRole::Level4, {16, 6, 0.12});
    elevations.setStyle(ElevationRole::Level5, {24, 8, 0.14});
}

void ThemeBuilder::applyDefaultMotion(Theme& theme) const {
    MotionTokens& motion = theme.motion();
    motion.setStyle(MotionToken::Short1, {50, QEasingCurve::OutCubic});
    motion.setStyle(MotionToken::Short2, {100, QEasingCurve::OutCubic});
    motion.setStyle(MotionToken::Short3, {150, QEasingCurve::OutCubic});
    motion.setStyle(MotionToken::Short4, {200, QEasingCurve::OutCubic});
    motion.setStyle(MotionToken::Medium1, {250, QEasingCurve::OutCubic});
    motion.setStyle(MotionToken::Medium2, {300, QEasingCurve::OutCubic});
    motion.setStyle(MotionToken::Medium3, {350, QEasingCurve::OutCubic});
    motion.setStyle(MotionToken::Medium4, {400, QEasingCurve::OutCubic});
    motion.setStyle(MotionToken::Long1, {450, QEasingCurve::OutCubic});
    motion.setStyle(MotionToken::Long2, {500, QEasingCurve::OutCubic});
    motion.setStyle(MotionToken::Long3, {550, QEasingCurve::OutCubic});
    motion.setStyle(MotionToken::Long4, {600, QEasingCurve::OutCubic});
}

void ThemeBuilder::applyDefaultStateLayer(Theme& theme) const {
    StateLayer& stateLayer = theme.stateLayer();
    stateLayer.setColor(theme.colorScheme().color(ColorRole::OnSurface));
    stateLayer.setHoverOpacity(0.08);
    stateLayer.setFocusOpacity(0.10);
    stateLayer.setPressOpacity(0.10);
    stateLayer.setDragOpacity(0.16);
}

void ThemeBuilder::applyFallbackLightScheme(Theme& theme, const QColor& seed) const {
    ColorScheme& scheme = theme.colorScheme();
    const QColor primary = seed;
    const QColor onPrimary = QColor(Qt::white);
    const QColor primaryContainer = tone(seed, 0.78);
    const QColor onPrimaryContainer = shade(seed, 0.78);
    const QColor secondary = shade(seed, 0.22);
    const QColor onSecondary = QColor(Qt::white);
    const QColor secondaryContainer = tone(secondary, 0.74);
    const QColor onSecondaryContainer = shade(secondary, 0.78);
    const QColor tertiary = QColor("#7D5260");
    const QColor onTertiary = QColor(Qt::white);
    const QColor tertiaryContainer = QColor("#FFD8E4");
    const QColor onTertiaryContainer = QColor("#31111D");
    const QColor error = QColor("#B3261E");
    const QColor onError = QColor(Qt::white);
    const QColor errorContainer = QColor("#F9DEDC");
    const QColor onErrorContainer = QColor("#410E0B");
    const QColor background = QColor("#FFFBFE");
    const QColor onBackground = QColor("#1C1B1F");
    const QColor surface = QColor("#FFFBFE");
    const QColor onSurface = QColor("#1C1B1F");
    const QColor surfaceDim = QColor("#DED8E1");
    const QColor surfaceBright = QColor("#FFFBFE");
    const QColor surfaceContainerLowest = QColor("#FFFFFF");
    const QColor surfaceContainerLow = QColor("#F7F2FA");
    const QColor surfaceContainer = QColor("#F3EDF7");
    const QColor surfaceContainerHigh = QColor("#ECE6F0");
    const QColor surfaceContainerHighest = QColor("#E6E0E9");
    const QColor surfaceVariant = QColor("#E7E0EC");
    const QColor onSurfaceVariant = QColor("#49454F");
    const QColor outline = QColor("#79747E");
    const QColor outlineVariant = QColor("#CAC4D0");
    const QColor inverseSurface = QColor("#313033");
    const QColor inverseOnSurface = QColor("#F4EFF4");
    const QColor inversePrimary = tone(seed, 0.50);
    const QColor shadow = QColor(0, 0, 0, 255);
    const QColor scrim = QColor(0, 0, 0, 255);

    scheme.setColor(ColorRole::Primary, primary);
    scheme.setColor(ColorRole::OnPrimary, onPrimary);
    scheme.setColor(ColorRole::PrimaryContainer, primaryContainer);
    scheme.setColor(ColorRole::OnPrimaryContainer, onPrimaryContainer);
    scheme.setColor(ColorRole::Secondary, secondary);
    scheme.setColor(ColorRole::OnSecondary, onSecondary);
    scheme.setColor(ColorRole::SecondaryContainer, secondaryContainer);
    scheme.setColor(ColorRole::OnSecondaryContainer, onSecondaryContainer);
    scheme.setColor(ColorRole::Tertiary, tertiary);
    scheme.setColor(ColorRole::OnTertiary, onTertiary);
    scheme.setColor(ColorRole::TertiaryContainer, tertiaryContainer);
    scheme.setColor(ColorRole::OnTertiaryContainer, onTertiaryContainer);
    scheme.setColor(ColorRole::Error, error);
    scheme.setColor(ColorRole::OnError, onError);
    scheme.setColor(ColorRole::ErrorContainer, errorContainer);
    scheme.setColor(ColorRole::OnErrorContainer, onErrorContainer);
    scheme.setColor(ColorRole::Background, background);
    scheme.setColor(ColorRole::OnBackground, onBackground);
    scheme.setColor(ColorRole::Surface, surface);
    scheme.setColor(ColorRole::OnSurface, onSurface);
    scheme.setColor(ColorRole::SurfaceDim, surfaceDim);
    scheme.setColor(ColorRole::SurfaceBright, surfaceBright);
    scheme.setColor(ColorRole::SurfaceContainerLowest, surfaceContainerLowest);
    scheme.setColor(ColorRole::SurfaceContainerLow, surfaceContainerLow);
    scheme.setColor(ColorRole::SurfaceContainer, surfaceContainer);
    scheme.setColor(ColorRole::SurfaceContainerHigh, surfaceContainerHigh);
    scheme.setColor(ColorRole::SurfaceContainerHighest, surfaceContainerHighest);
    scheme.setColor(ColorRole::SurfaceVariant, surfaceVariant);
    scheme.setColor(ColorRole::OnSurfaceVariant, onSurfaceVariant);
    scheme.setColor(ColorRole::Outline, outline);
    scheme.setColor(ColorRole::OutlineVariant, outlineVariant);
    scheme.setColor(ColorRole::InverseSurface, inverseSurface);
    scheme.setColor(ColorRole::InverseOnSurface, inverseOnSurface);
    scheme.setColor(ColorRole::InversePrimary, inversePrimary);
    scheme.setColor(ColorRole::Shadow, shadow);
    scheme.setColor(ColorRole::Scrim, scrim);
}

void ThemeBuilder::applyFallbackDarkScheme(Theme& theme, const QColor& seed) const {
    ColorScheme& scheme = theme.colorScheme();
    const QColor primary = tone(seed, 0.35);
    const QColor onPrimary = shade(seed, 0.70);
    const QColor primaryContainer = shade(seed, 0.58);
    const QColor onPrimaryContainer = tone(seed, 0.72);
    const QColor secondary = tone(shade(seed, 0.22), 0.35);
    const QColor onSecondary = shade(secondary, 0.72);
    const QColor secondaryContainer = shade(secondary, 0.58);
    const QColor onSecondaryContainer = tone(secondary, 0.72);
    const QColor tertiary = QColor("#EFB8C8");
    const QColor onTertiary = QColor("#492532");
    const QColor tertiaryContainer = QColor("#633B48");
    const QColor onTertiaryContainer = QColor("#FFD8E4");
    const QColor error = QColor("#F2B8B5");
    const QColor onError = QColor("#601410");
    const QColor errorContainer = QColor("#8C1D18");
    const QColor onErrorContainer = QColor("#F9DEDC");
    const QColor background = QColor("#1C1B1F");
    const QColor onBackground = QColor("#E6E1E5");
    const QColor surface = QColor("#141218");
    const QColor onSurface = QColor("#E6E1E5");
    const QColor surfaceDim = QColor("#141218");
    const QColor surfaceBright = QColor("#3B383E");
    const QColor surfaceContainerLowest = QColor("#0F0D13");
    const QColor surfaceContainerLow = QColor("#1D1B20");
    const QColor surfaceContainer = QColor("#211F26");
    const QColor surfaceContainerHigh = QColor("#2B2930");
    const QColor surfaceContainerHighest = QColor("#36343B");
    const QColor surfaceVariant = QColor("#49454F");
    const QColor onSurfaceVariant = QColor("#CAC4D0");
    const QColor outline = QColor("#938F99");
    const QColor outlineVariant = QColor("#49454F");
    const QColor inverseSurface = QColor("#E6E1E5");
    const QColor inverseOnSurface = QColor("#313033");
    const QColor inversePrimary = seed;
    const QColor shadow = QColor(0, 0, 0, 255);
    const QColor scrim = QColor(0, 0, 0, 255);

    scheme.setColor(ColorRole::Primary, primary);
    scheme.setColor(ColorRole::OnPrimary, onPrimary);
    scheme.setColor(ColorRole::PrimaryContainer, primaryContainer);
    scheme.setColor(ColorRole::OnPrimaryContainer, onPrimaryContainer);
    scheme.setColor(ColorRole::Secondary, secondary);
    scheme.setColor(ColorRole::OnSecondary, onSecondary);
    scheme.setColor(ColorRole::SecondaryContainer, secondaryContainer);
    scheme.setColor(ColorRole::OnSecondaryContainer, onSecondaryContainer);
    scheme.setColor(ColorRole::Tertiary, tertiary);
    scheme.setColor(ColorRole::OnTertiary, onTertiary);
    scheme.setColor(ColorRole::TertiaryContainer, tertiaryContainer);
    scheme.setColor(ColorRole::OnTertiaryContainer, onTertiaryContainer);
    scheme.setColor(ColorRole::Error, error);
    scheme.setColor(ColorRole::OnError, onError);
    scheme.setColor(ColorRole::ErrorContainer, errorContainer);
    scheme.setColor(ColorRole::OnErrorContainer, onErrorContainer);
    scheme.setColor(ColorRole::Background, background);
    scheme.setColor(ColorRole::OnBackground, onBackground);
    scheme.setColor(ColorRole::Surface, surface);
    scheme.setColor(ColorRole::OnSurface, onSurface);
    scheme.setColor(ColorRole::SurfaceDim, surfaceDim);
    scheme.setColor(ColorRole::SurfaceBright, surfaceBright);
    scheme.setColor(ColorRole::SurfaceContainerLowest, surfaceContainerLowest);
    scheme.setColor(ColorRole::SurfaceContainerLow, surfaceContainerLow);
    scheme.setColor(ColorRole::SurfaceContainer, surfaceContainer);
    scheme.setColor(ColorRole::SurfaceContainerHigh, surfaceContainerHigh);
    scheme.setColor(ColorRole::SurfaceContainerHighest, surfaceContainerHighest);
    scheme.setColor(ColorRole::SurfaceVariant, surfaceVariant);
    scheme.setColor(ColorRole::OnSurfaceVariant, onSurfaceVariant);
    scheme.setColor(ColorRole::Outline, outline);
    scheme.setColor(ColorRole::OutlineVariant, outlineVariant);
    scheme.setColor(ColorRole::InverseSurface, inverseSurface);
    scheme.setColor(ColorRole::InverseOnSurface, inverseOnSurface);
    scheme.setColor(ColorRole::InversePrimary, inversePrimary);
    scheme.setColor(ColorRole::Shadow, shadow);
    scheme.setColor(ColorRole::Scrim, scrim);
}

} // namespace QtMaterial
