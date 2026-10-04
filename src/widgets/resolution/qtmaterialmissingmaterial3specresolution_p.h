#pragma once

#include <QColor>
#include <QPalette>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {
namespace MissingMaterial3SpecResolution {

inline QColor resolvedColor(
    const Theme& theme,
    ColorRole role,
    const QPalette& palette,
    QPalette::ColorRole fallback)
{
    const QColor color = theme.colorScheme().color(role);
    return color.isValid()
        ? color
        : palette.color(fallback);
}

struct NavigationBarSpec
{
    QColor containerColor;
    QColor activeIndicatorColor;
    QColor activeIconColor;
    QColor activeLabelColor;
    QColor inactiveColor;
    QColor focusRingColor;
};

inline NavigationBarSpec navigationBarSpec(
    const Theme& theme,
    const QPalette& palette)
{
    return {
        resolvedColor(
            theme,
            ColorRole::SurfaceContainer,
            palette,
            QPalette::Window),
        resolvedColor(
            theme,
            ColorRole::SecondaryContainer,
            palette,
            QPalette::Highlight),
        resolvedColor(
            theme,
            ColorRole::OnSecondaryContainer,
            palette,
            QPalette::HighlightedText),
        resolvedColor(
            theme,
            ColorRole::OnSurface,
            palette,
            QPalette::WindowText),
        resolvedColor(
            theme,
            ColorRole::OnSurfaceVariant,
            palette,
            QPalette::WindowText),
        resolvedColor(
            theme,
            ColorRole::Primary,
            palette,
            QPalette::Highlight)
    };
}

struct SideSheetSpec
{
    QColor containerColor;
};

inline SideSheetSpec sideSheetSpec(
    const Theme& theme,
    const QPalette& palette)
{
    return {
        resolvedColor(
            theme,
            ColorRole::SurfaceContainerLow,
            palette,
            QPalette::Window)
    };
}

struct TooltipSpec
{
    QColor containerColor;
    QColor contentColor;
};

inline TooltipSpec tooltipSpec(
    const Theme& theme,
    const QPalette& palette)
{
    return {
        resolvedColor(
            theme,
            ColorRole::InverseSurface,
            palette,
            QPalette::ToolTipBase),
        resolvedColor(
            theme,
            ColorRole::InverseOnSurface,
            palette,
            QPalette::ToolTipText)
    };
}

struct BadgeSpec
{
    QColor containerColor;
    QColor contentColor;
};

inline BadgeSpec badgeSpec(
    const Theme& theme,
    const QPalette& palette)
{
    return {
        resolvedColor(
            theme,
            ColorRole::Error,
            palette,
            QPalette::Highlight),
        resolvedColor(
            theme,
            ColorRole::OnError,
            palette,
            QPalette::HighlightedText)
    };
}

} // namespace MissingMaterial3SpecResolution
} // namespace QtMaterial
