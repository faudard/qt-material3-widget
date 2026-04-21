#include "qtmaterial/theme/qtmaterialthemebuilder.h"

#include <QtGui/QFont>

namespace QtMaterial {

namespace {

QColor tone(const QColor& base, qreal factorTowardWhite)
{
    QColor result = base;
    result.setRed(result.red() + static_cast<int>((255 - result.red()) * factorTowardWhite));
    result.setGreen(result.green() + static_cast<int>((255 - result.green()) * factorTowardWhite));
    result.setBlue(result.blue() + static_cast<int>((255 - result.blue()) * factorTowardWhite));
    return result;
}

QColor shade(const QColor& base, qreal factorTowardBlack)
{
    QColor result = base;
    result.setRed(static_cast<int>(result.red() * (1.0 - factorTowardBlack)));
    result.setGreen(static_cast<int>(result.green() * (1.0 - factorTowardBlack)));
    result.setBlue(static_cast<int>(result.blue() * (1.0 - factorTowardBlack)));
    return result;
}

QFont defaultFont(int pointSize, int weight = QFont::Normal)
{
    QFont font;
    font.setPointSize(pointSize);
    font.setWeight(weight);
    return font;
}

} // namespace

ThemeBuilder::ThemeBuilder() = default;
ThemeBuilder::~ThemeBuilder() = default;

Theme ThemeBuilder::build(const ThemeOptions& options) const
{
    return buildFallbackTheme(options);
}

Theme ThemeBuilder::buildLightFromSeed(const QColor& seed) const
{
    ThemeOptions options;
    options.sourceColor = seed;
    options.mode = ThemeMode::Light;
    return build(options);
}

Theme ThemeBuilder::buildDarkFromSeed(const QColor& seed) const
{
    ThemeOptions options;
    options.sourceColor = seed;
    options.mode = ThemeMode::Dark;
    return build(options);
}

ColorScheme ThemeBuilder::buildLightSchemeFromSeed(const QColor& seed) const
{
    return buildLightFromSeed(seed).colorScheme();
}

ColorScheme ThemeBuilder::buildDarkSchemeFromSeed(const QColor& seed) const
{
    return buildDarkFromSeed(seed).colorScheme();
}

Theme ThemeBuilder::buildFallbackTheme(const ThemeOptions& options) const
{
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

void ThemeBuilder::applyFallbackLightScheme(Theme& theme, const QColor& seed) const
{
    auto& scheme = theme.colorScheme();

    scheme.setColor(ColorRole::Primary, seed);
    scheme.setColor(ColorRole::OnPrimary, QColor(Qt::white));
    scheme.setColor(ColorRole::PrimaryContainer, tone(seed, 0.78));
    scheme.setColor(ColorRole::OnPrimaryContainer, shade(seed, 0.78));

    scheme.setColor(ColorRole::Secondary, shade(seed, 0.22));
    scheme.setColor(ColorRole::OnSecondary, QColor(Qt::white));
    scheme.setColor(ColorRole::SecondaryContainer, tone(shade(seed, 0.22), 0.74));
    scheme.setColor(ColorRole::OnSecondaryContainer, shade(shade(seed, 0.22), 0.78));

    scheme.setColor(ColorRole::Tertiary, QColor("#7D5260"));
    scheme.setColor(ColorRole::OnTertiary, QColor(Qt::white));
    scheme.setColor(ColorRole::TertiaryContainer, QColor("#FFD8E4"));
    scheme.setColor(ColorRole::OnTertiaryContainer, QColor("#31111D"));

    scheme.setColor(ColorRole::Error, QColor("#B3261E"));
    scheme.setColor(ColorRole::OnError, QColor(Qt::white));
    scheme.setColor(ColorRole::ErrorContainer, QColor("#F9DEDC"));
    scheme.setColor(ColorRole::OnErrorContainer, QColor("#410E0B"));

    scheme.setColor(ColorRole::Background, QColor("#FFFBFE"));
    scheme.setColor(ColorRole::OnBackground, QColor("#1C1B1F"));
    scheme.setColor(ColorRole::Surface, QColor("#FFFBFE"));
    scheme.setColor(ColorRole::OnSurface, QColor("#1C1B1F"));

    scheme.setColor(ColorRole::SurfaceDim, QColor("#DED8E1"));
    scheme.setColor(ColorRole::SurfaceBright, QColor("#FFFBFE"));
    scheme.setColor(ColorRole::SurfaceContainerLowest, QColor("#FFFFFF"));
    scheme.setColor(ColorRole::SurfaceContainerLow, QColor("#F7F2FA"));
    scheme.setColor(ColorRole::SurfaceContainer, QColor("#F3EDF7"));
    scheme.setColor(ColorRole::SurfaceContainerHigh, QColor("#ECE6F0"));
    scheme.setColor(ColorRole::SurfaceContainerHighest, QColor("#E6E0E9"));

    scheme.setColor(ColorRole::SurfaceVariant, QColor("#E7E0EC"));
    scheme.setColor(ColorRole::OnSurfaceVariant, QColor("#49454F"));
    scheme.setColor(ColorRole::Outline, QColor("#79747E"));
    scheme.setColor(ColorRole::OutlineVariant, QColor("#CAC4D0"));

    scheme.setColor(ColorRole::InverseSurface, QColor("#313033"));
    scheme.setColor(ColorRole::InverseOnSurface, QColor("#F4EFF4"));
    scheme.setColor(ColorRole::InversePrimary, tone(seed, 0.50));

    scheme.setColor(ColorRole::Shadow, QColor(0, 0, 0, 255));
    scheme.setColor(ColorRole::Scrim, QColor(0, 0, 0, 255));
}

void ThemeBuilder::applyFallbackDarkScheme(Theme& theme, const QColor& seed) const
{
    auto& scheme = theme.colorScheme();

    scheme.setColor(ColorRole::Primary, tone(seed, 0.35));
    scheme.setColor(ColorRole::OnPrimary, shade(seed, 0.70));
    scheme.setColor(ColorRole::PrimaryContainer, shade(seed, 0.58));
    scheme.setColor(ColorRole::OnPrimaryContainer, tone(seed, 0.72));

    const QColor secondary = tone(shade(seed, 0.22), 0.35);
    scheme.setColor(ColorRole::Secondary, secondary);
    scheme.setColor(ColorRole::OnSecondary, shade(secondary, 0.72));
    scheme.setColor(ColorRole::SecondaryContainer, shade(secondary, 0.58));
    scheme.setColor(ColorRole::OnSecondaryContainer, tone(secondary, 0.72));

    scheme.setColor(ColorRole::Tertiary, QColor("#EFB8C8"));
    scheme.setColor(ColorRole::OnTertiary, QColor("#492532"));
    scheme.setColor(ColorRole::TertiaryContainer, QColor("#633B48"));
    scheme.setColor(ColorRole::OnTertiaryContainer, QColor("#FFD8E4"));

    scheme.setColor(ColorRole::Error, QColor("#F2B8B5"));
    scheme.setColor(ColorRole::OnError, QColor("#601410"));
    scheme.setColor(ColorRole::ErrorContainer, QColor("#8C1D18"));
    scheme.setColor(ColorRole::OnErrorContainer, QColor("#F9DEDC"));

    scheme.setColor(ColorRole::Background, QColor("#1C1B1F"));
    scheme.setColor(ColorRole::OnBackground, QColor("#E6E1E5"));
    scheme.setColor(ColorRole::Surface, QColor("#141218"));
    scheme.setColor(ColorRole::OnSurface, QColor("#E6E1E5"));

    scheme.setColor(ColorRole::SurfaceDim, QColor("#141218"));
    scheme.setColor(ColorRole::SurfaceBright, QColor("#3B383E"));
    scheme.setColor(ColorRole::SurfaceContainerLowest, QColor("#0F0D13"));
    scheme.setColor(ColorRole::SurfaceContainerLow, QColor("#1D1B20"));
    scheme.setColor(ColorRole::SurfaceContainer, QColor("#211F26"));
    scheme.setColor(ColorRole::SurfaceContainerHigh, QColor("#2B2930"));
    scheme.setColor(ColorRole::SurfaceContainerHighest, QColor("#36343B"));

    scheme.setColor(ColorRole::SurfaceVariant, QColor("#49454F"));
    scheme.setColor(ColorRole::OnSurfaceVariant, QColor("#CAC4D0"));
    scheme.setColor(ColorRole::Outline, QColor("#938F99"));
    scheme.setColor(ColorRole::OutlineVariant, QColor("#49454F"));

    scheme.setColor(ColorRole::InverseSurface, QColor("#E6E1E5"));
    scheme.setColor(ColorRole::InverseOnSurface, QColor("#313033"));
    scheme.setColor(ColorRole::InversePrimary, seed);

    scheme.setColor(ColorRole::Shadow, QColor(0, 0, 0, 255));
    scheme.setColor(ColorRole::Scrim, QColor(0, 0, 0, 255));
}

void ThemeBuilder::applyDefaultTypography(Theme& theme) const
{
    auto& t = theme.typography();

    t.setStyle(TypeRole::DisplayLarge, { defaultFont(36, QFont::Medium), 44.0, 0.0 });
    t.setStyle(TypeRole::DisplayMedium, { defaultFont(32, QFont::Medium), 40.0, 0.0 });
    t.setStyle(TypeRole::DisplaySmall, { defaultFont(28, QFont::Medium), 36.0, 0.0 });

    t.setStyle(TypeRole::HeadlineLarge, { defaultFont(26, QFont::Medium), 32.0, 0.0 });
    t.setStyle(TypeRole::HeadlineMedium, { defaultFont(24, QFont::Medium), 30.0, 0.0 });
    t.setStyle(TypeRole::HeadlineSmall, { defaultFont(22, QFont::Medium), 28.0, 0.0 });

    t.setStyle(TypeRole::TitleLarge, { defaultFont(20, QFont::Medium), 26.0, 0.0 });
    t.setStyle(TypeRole::TitleMedium, { defaultFont(16, QFont::DemiBold), 24.0, 0.1 });
    t.setStyle(TypeRole::TitleSmall, { defaultFont(14, QFont::DemiBold), 20.0, 0.1 });

    t.setStyle(TypeRole::BodyLarge, { defaultFont(16, QFont::Normal), 24.0, 0.2 });
    t.setStyle(TypeRole::BodyMedium, { defaultFont(14, QFont::Normal), 20.0, 0.2 });
    t.setStyle(TypeRole::BodySmall, { defaultFont(12, QFont::Normal), 16.0, 0.4 });

    t.setStyle(TypeRole::LabelLarge, { defaultFont(14, QFont::DemiBold), 20.0, 0.1 });
    t.setStyle(TypeRole::LabelMedium, { defaultFont(12, QFont::DemiBold), 16.0, 0.4 });
    t.setStyle(TypeRole::LabelSmall, { defaultFont(11, QFont::DemiBold), 16.0, 0.5 });
}

void ThemeBuilder::applyDefaultShapes(Theme& theme) const
{
    auto& s = theme.shapes();
    s.setRadius(ShapeRole::None, 0);
    s.setRadius(ShapeRole::ExtraSmall, 4);
    s.setRadius(ShapeRole::Small, 8);
    s.setRadius(ShapeRole::Medium, 12);
    s.setRadius(ShapeRole::Large, 16);
    s.setRadius(ShapeRole::ExtraLarge, 28);
    s.setRadius(ShapeRole::Full, 999);
}

void ThemeBuilder::applyDefaultElevations(Theme& theme) const
{
    auto& e = theme.elevations();
    e.setStyle(ElevationRole::Level0, { 0, 0, 0.00 });
    e.setStyle(ElevationRole::Level1, { 4, 1, 0.05 });
    e.setStyle(ElevationRole::Level2, { 8, 2, 0.08 });
    e.setStyle(ElevationRole::Level3, { 12, 4, 0.11 });
    e.setStyle(ElevationRole::Level4, { 16, 6, 0.12 });
    e.setStyle(ElevationRole::Level5, { 20, 8, 0.14 });
}

void ThemeBuilder::applyDefaultMotion(Theme& theme) const
{
    auto& m = theme.motion();
    m.setStyle(MotionToken::Short1, { 50, QEasingCurve(QEasingCurve::OutCubic) });
    m.setStyle(MotionToken::Short2, { 100, QEasingCurve(QEasingCurve::OutCubic) });
    m.setStyle(MotionToken::Short3, { 150, QEasingCurve(QEasingCurve::OutCubic) });
    m.setStyle(MotionToken::Short4, { 200, QEasingCurve(QEasingCurve::OutCubic) });
    m.setStyle(MotionToken::Medium1, { 250, QEasingCurve(QEasingCurve::OutCubic) });
    m.setStyle(MotionToken::Medium2, { 300, QEasingCurve(QEasingCurve::OutCubic) });
    m.setStyle(MotionToken::Medium3, { 350, QEasingCurve(QEasingCurve::OutCubic) });
    m.setStyle(MotionToken::Medium4, { 400, QEasingCurve(QEasingCurve::OutCubic) });
    m.setStyle(MotionToken::Long1, { 450, QEasingCurve(QEasingCurve::OutCubic) });
    m.setStyle(MotionToken::Long2, { 500, QEasingCurve(QEasingCurve::OutCubic) });
    m.setStyle(MotionToken::Long3, { 550, QEasingCurve(QEasingCurve::OutCubic) });
    m.setStyle(MotionToken::Long4, { 600, QEasingCurve(QEasingCurve::OutCubic) });
}

void ThemeBuilder::applyDefaultStateLayer(Theme& theme) const
{
    auto& s = theme.stateLayer();
    s.color = theme.colorScheme().color(ColorRole::OnSurface);
    s.hoverOpacity = 0.08;
    s.focusOpacity = 0.10;
    s.pressOpacity = 0.10;
    s.dragOpacity = 0.16;
}

} // namespace QtMaterial
