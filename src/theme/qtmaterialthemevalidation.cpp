#include "qtmaterial/theme/qtmaterialthemevalidation.h"

#include <QtGlobal>

#include <cmath>
#include <utility>

namespace QtMaterial {
namespace {

void addIssue(ThemeValidationResult& result, ThemeValidationSeverity severity, QString path, QString message)
{
    result.issues.append({ severity, std::move(path), std::move(message) });
}

void addError(ThemeValidationResult& result, QString path, QString message)
{
    addIssue(result, ThemeValidationSeverity::Error, std::move(path), std::move(message));
}

void addWarning(ThemeValidationResult& result, QString path, QString message)
{
    addIssue(result, ThemeValidationSeverity::Warning, std::move(path), std::move(message));
}

double channelToLinear(double channel)
{
    channel /= 255.0;
    if (channel <= 0.03928) {
        return channel / 12.92;
    }
    return std::pow((channel + 0.055) / 1.055, 2.4);
}

double relativeLuminance(const QColor& color)
{
    const QColor rgb = color.toRgb();
    return 0.2126 * channelToLinear(rgb.red())
        + 0.7152 * channelToLinear(rgb.green())
        + 0.0722 * channelToLinear(rgb.blue());
}

struct ContrastPair {
    ColorRole foreground;
    ColorRole background;
    const char* path;
    bool text;
};

constexpr ContrastPair kContrastPairs[] = {
    { ColorRole::OnPrimary, ColorRole::Primary, "colorScheme.OnPrimary/Primary", true },
    { ColorRole::OnPrimaryContainer, ColorRole::PrimaryContainer, "colorScheme.OnPrimaryContainer/PrimaryContainer", true },
    { ColorRole::OnSecondary, ColorRole::Secondary, "colorScheme.OnSecondary/Secondary", true },
    { ColorRole::OnSecondaryContainer, ColorRole::SecondaryContainer, "colorScheme.OnSecondaryContainer/SecondaryContainer", true },
    { ColorRole::OnTertiary, ColorRole::Tertiary, "colorScheme.OnTertiary/Tertiary", true },
    { ColorRole::OnTertiaryContainer, ColorRole::TertiaryContainer, "colorScheme.OnTertiaryContainer/TertiaryContainer", true },
    { ColorRole::OnError, ColorRole::Error, "colorScheme.OnError/Error", true },
    { ColorRole::OnErrorContainer, ColorRole::ErrorContainer, "colorScheme.OnErrorContainer/ErrorContainer", true },
    { ColorRole::OnBackground, ColorRole::Background, "colorScheme.OnBackground/Background", true },
    { ColorRole::OnSurface, ColorRole::Surface, "colorScheme.OnSurface/Surface", true },
    { ColorRole::OnSurfaceVariant, ColorRole::SurfaceVariant, "colorScheme.OnSurfaceVariant/SurfaceVariant", true },
    { ColorRole::InverseOnSurface, ColorRole::InverseSurface, "colorScheme.InverseOnSurface/InverseSurface", true },
    { ColorRole::Outline, ColorRole::Surface, "colorScheme.Outline/Surface", false },
    // OutlineVariant is intentionally lower-emphasis. Components that need
    // guaranteed non-text UI contrast should use Outline, not OutlineVariant.
};

bool opacityInRange(qreal value)
{
    return value >= 0.0 && value <= 1.0;
}

} // namespace

QString colorRoleName(ColorRole role)
{
    switch (role) {
    case ColorRole::Primary: return QStringLiteral("Primary");
    case ColorRole::OnPrimary: return QStringLiteral("OnPrimary");
    case ColorRole::PrimaryContainer: return QStringLiteral("PrimaryContainer");
    case ColorRole::OnPrimaryContainer: return QStringLiteral("OnPrimaryContainer");
    case ColorRole::PrimaryFixed: return QStringLiteral("PrimaryFixed");
    case ColorRole::PrimaryFixedDim: return QStringLiteral("PrimaryFixedDim");
    case ColorRole::OnPrimaryFixed: return QStringLiteral("OnPrimaryFixed");
    case ColorRole::OnPrimaryFixedVariant: return QStringLiteral("OnPrimaryFixedVariant");
    case ColorRole::Secondary: return QStringLiteral("Secondary");
    case ColorRole::OnSecondary: return QStringLiteral("OnSecondary");
    case ColorRole::SecondaryContainer: return QStringLiteral("SecondaryContainer");
    case ColorRole::OnSecondaryContainer: return QStringLiteral("OnSecondaryContainer");
    case ColorRole::SecondaryFixed: return QStringLiteral("SecondaryFixed");
    case ColorRole::SecondaryFixedDim: return QStringLiteral("SecondaryFixedDim");
    case ColorRole::OnSecondaryFixed: return QStringLiteral("OnSecondaryFixed");
    case ColorRole::OnSecondaryFixedVariant: return QStringLiteral("OnSecondaryFixedVariant");
    case ColorRole::Tertiary: return QStringLiteral("Tertiary");
    case ColorRole::OnTertiary: return QStringLiteral("OnTertiary");
    case ColorRole::TertiaryContainer: return QStringLiteral("TertiaryContainer");
    case ColorRole::OnTertiaryContainer: return QStringLiteral("OnTertiaryContainer");
    case ColorRole::TertiaryFixed: return QStringLiteral("TertiaryFixed");
    case ColorRole::TertiaryFixedDim: return QStringLiteral("TertiaryFixedDim");
    case ColorRole::OnTertiaryFixed: return QStringLiteral("OnTertiaryFixed");
    case ColorRole::OnTertiaryFixedVariant: return QStringLiteral("OnTertiaryFixedVariant");
    case ColorRole::Error: return QStringLiteral("Error");
    case ColorRole::OnError: return QStringLiteral("OnError");
    case ColorRole::ErrorContainer: return QStringLiteral("ErrorContainer");
    case ColorRole::OnErrorContainer: return QStringLiteral("OnErrorContainer");
    case ColorRole::Background: return QStringLiteral("Background");
    case ColorRole::OnBackground: return QStringLiteral("OnBackground");
    case ColorRole::Surface: return QStringLiteral("Surface");
    case ColorRole::OnSurface: return QStringLiteral("OnSurface");
    case ColorRole::SurfaceDim: return QStringLiteral("SurfaceDim");
    case ColorRole::SurfaceBright: return QStringLiteral("SurfaceBright");
    case ColorRole::SurfaceContainerLowest: return QStringLiteral("SurfaceContainerLowest");
    case ColorRole::SurfaceContainerLow: return QStringLiteral("SurfaceContainerLow");
    case ColorRole::SurfaceContainer: return QStringLiteral("SurfaceContainer");
    case ColorRole::SurfaceContainerHigh: return QStringLiteral("SurfaceContainerHigh");
    case ColorRole::SurfaceContainerHighest: return QStringLiteral("SurfaceContainerHighest");
    case ColorRole::SurfaceVariant: return QStringLiteral("SurfaceVariant");
    case ColorRole::OnSurfaceVariant: return QStringLiteral("OnSurfaceVariant");
    case ColorRole::SurfaceTint: return QStringLiteral("SurfaceTint");
    case ColorRole::Outline: return QStringLiteral("Outline");
    case ColorRole::OutlineVariant: return QStringLiteral("OutlineVariant");
    case ColorRole::InverseSurface: return QStringLiteral("InverseSurface");
    case ColorRole::InverseOnSurface: return QStringLiteral("InverseOnSurface");
    case ColorRole::InversePrimary: return QStringLiteral("InversePrimary");
    case ColorRole::Shadow: return QStringLiteral("Shadow");
    case ColorRole::Scrim: return QStringLiteral("Scrim");
    }
    return QStringLiteral("Unknown");
}

double colorContrastRatio(const QColor& foreground, const QColor& background)
{
    if (!foreground.isValid() || !background.isValid()) {
        return 0.0;
    }
    const double fg = QtMaterial::relativeLuminance(foreground);
    const double bg = QtMaterial::relativeLuminance(background);
    const double lighter = qMax(fg, bg);
    const double darker = qMin(fg, bg);
    return (lighter + 0.05) / (darker + 0.05);
}

bool ThemeValidationResult::isValid() const
{
    return !hasErrors();
}

bool ThemeValidationResult::hasErrors() const
{
    for (const ThemeValidationIssue& issue : issues) {
        if (issue.severity == ThemeValidationSeverity::Error) {
            return true;
        }
    }
    return false;
}

QStringList ThemeValidationResult::errors() const
{
    QStringList values;
    for (const ThemeValidationIssue& issue : issues) {
        if (issue.severity == ThemeValidationSeverity::Error) {
            values.append(QStringLiteral("%1: %2").arg(issue.path, issue.message));
        }
    }
    return values;
}

QStringList ThemeValidationResult::warnings() const
{
    QStringList values;
    for (const ThemeValidationIssue& issue : issues) {
        if (issue.severity == ThemeValidationSeverity::Warning) {
            values.append(QStringLiteral("%1: %2").arg(issue.path, issue.message));
        }
    }
    return values;
}

ThemeValidationResult validateColorScheme(const ColorScheme& scheme, const ThemeValidationPolicy& policy)
{
    ThemeValidationResult result;

    if (policy.requireCompleteColorScheme) {
        for (ColorRole role : scheme.missingRoles()) {
            addError(result,
                QStringLiteral("colorScheme.%1").arg(colorRoleName(role)),
                QStringLiteral("Required Material 3 color role is missing or invalid."));
        }
    }

    if (policy.validateContrast) {
        for (const ContrastPair& pair : kContrastPairs) {
            if (!scheme.contains(pair.foreground) || !scheme.contains(pair.background)) {
                continue;
            }
            const double ratio = colorContrastRatio(scheme.color(pair.foreground), scheme.color(pair.background));
            const double minimum = pair.text ? policy.minimumTextContrastRatio : policy.minimumUiContrastRatio;
            if (ratio < minimum) {
                addError(result,
                    QString::fromLatin1(pair.path),
                    QStringLiteral("Contrast ratio %1 is below required minimum %2.")
                        .arg(ratio, 0, 'f', 2)
                        .arg(minimum, 0, 'f', 2));
            }
        }
    }

    return result;
}

ThemeValidationResult validateTheme(const Theme& theme, const ThemeValidationPolicy& policy)
{
    ThemeValidationPolicy effectivePolicy = policy;
    effectivePolicy.minimumTextContrastRatio = qMax(effectivePolicy.minimumTextContrastRatio,
        theme.accessibility().minimumTextContrastRatio);
    effectivePolicy.minimumUiContrastRatio = qMax(effectivePolicy.minimumUiContrastRatio,
        theme.accessibility().minimumUiContrastRatio);

    ThemeValidationResult result = validateColorScheme(theme.colorScheme(), effectivePolicy);

    if (!theme.options().sourceColor.isValid()) {
        addError(result, QStringLiteral("source.seedColor"), QStringLiteral("Seed color is invalid."));
    }

    if (policy.validateTokenRanges) {
        const StateLayer& stateLayer = theme.stateLayer();
        if (!stateLayer.color.isValid()) {
            addError(result, QStringLiteral("resolved.stateLayer.color"), QStringLiteral("State layer color is invalid."));
        }
        if (!opacityInRange(stateLayer.hoverOpacity)) {
            addError(result, QStringLiteral("resolved.stateLayer.hoverOpacity"), QStringLiteral("Opacity must be between 0 and 1."));
        }
        if (!opacityInRange(stateLayer.focusOpacity)) {
            addError(result, QStringLiteral("resolved.stateLayer.focusOpacity"), QStringLiteral("Opacity must be between 0 and 1."));
        }
        if (!opacityInRange(stateLayer.pressOpacity)) {
            addError(result, QStringLiteral("resolved.stateLayer.pressOpacity"), QStringLiteral("Opacity must be between 0 and 1."));
        }
        if (!opacityInRange(stateLayer.dragOpacity)) {
            addError(result, QStringLiteral("resolved.stateLayer.dragOpacity"), QStringLiteral("Opacity must be between 0 and 1."));
        }

        const AccessibilityTokens& accessibility = theme.accessibility();
        if (accessibility.minimumTextContrastRatio < 1.0) {
            addWarning(result, QStringLiteral("resolved.accessibility.minimumTextContrastRatio"), QStringLiteral("Text contrast target is unusually low."));
        }
        if (accessibility.minimumUiContrastRatio < 1.0) {
            addWarning(result, QStringLiteral("resolved.accessibility.minimumUiContrastRatio"), QStringLiteral("UI contrast target is unusually low."));
        }
        if (accessibility.focusRing.width < 0 || accessibility.focusRing.offset < 0) {
            addError(result, QStringLiteral("resolved.accessibility.focusRing"), QStringLiteral("Focus ring width and offset must be non-negative."));
        }
        if (!accessibility.focusRing.color.isValid()) {
            addError(result, QStringLiteral("resolved.accessibility.focusRing.color"), QStringLiteral("Focus ring color is invalid."));
        }
    }

    return result;
}

} // namespace QtMaterial
