#include "qtmaterial/core/qtmaterialpaletteadapter.h"

namespace QtMaterial {

QColor QtMaterialPaletteAdapter::roleOrFallback(const Theme& theme, ColorRole role, const QColor& fallback)
{
    const QColor color = theme.colorScheme().color(role);
    return color.isValid() ? color : fallback;
}

QPalette QtMaterialPaletteAdapter::toPalette(const Theme& theme, const QPalette& base)
{
    QPalette palette = base;

    const QColor surface = roleOrFallback(theme, ColorRole::Surface, base.color(QPalette::Window));
    const QColor onSurface = roleOrFallback(theme, ColorRole::OnSurface, base.color(QPalette::WindowText));
    const QColor primary = roleOrFallback(theme, ColorRole::Primary, base.color(QPalette::Highlight));
    const QColor onPrimary = roleOrFallback(theme, ColorRole::OnPrimary, base.color(QPalette::HighlightedText));
    const QColor surfaceVariant = roleOrFallback(theme, ColorRole::SurfaceVariant, base.color(QPalette::Base));
    const QColor onSurfaceVariant = roleOrFallback(theme, ColorRole::OnSurfaceVariant, base.color(QPalette::Text));
    const QColor outline = roleOrFallback(theme, ColorRole::Outline, base.color(QPalette::Mid));

    palette.setColor(QPalette::Window, surface);
    palette.setColor(QPalette::WindowText, onSurface);
    palette.setColor(QPalette::Base, surfaceVariant);
    palette.setColor(QPalette::AlternateBase, surface);
    palette.setColor(QPalette::Text, onSurfaceVariant);
    palette.setColor(QPalette::Button, surfaceVariant);
    palette.setColor(QPalette::ButtonText, onSurface);
    palette.setColor(QPalette::Highlight, primary);
    palette.setColor(QPalette::HighlightedText, onPrimary);
    palette.setColor(QPalette::ToolTipBase, surfaceVariant);
    palette.setColor(QPalette::ToolTipText, onSurfaceVariant);
    palette.setColor(QPalette::Mid, outline);

    QColor disabledText = onSurface;
    disabledText.setAlphaF(0.38);
    palette.setColor(QPalette::Disabled, QPalette::WindowText, disabledText);
    palette.setColor(QPalette::Disabled, QPalette::Text, disabledText);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledText);

    return palette;
}

} // namespace QtMaterial
