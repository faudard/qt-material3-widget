#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

namespace {
QColor roleOr(const QColor& candidate, const QColor& fallback)
{
    return candidate.isValid() ? candidate : fallback;
}
} // namespace

// These free functions are intentionally additive. If the project prefers central
// SpecFactory static methods, move the function bodies into SpecFactory.

DatePickerSpec makeDatePickerSpec(const Theme& theme)
{
    DatePickerSpec spec = defaultDatePickerSpec();
    spec.backgroundColor = roleOr(theme.colorScheme().surfaceContainerHigh, spec.backgroundColor);
    spec.foregroundColor = roleOr(theme.colorScheme().onSurface, spec.foregroundColor);
    spec.headlineColor = roleOr(theme.colorScheme().onSurface, spec.headlineColor);
    spec.selectedDateColor = roleOr(theme.colorScheme().primary, spec.selectedDateColor);
    spec.selectedDateTextColor = roleOr(theme.colorScheme().onPrimary, spec.selectedDateTextColor);
    spec.todayOutlineColor = roleOr(theme.colorScheme().primary, spec.todayOutlineColor);
    spec.disabledTextColor = roleOr(theme.colorScheme().onSurfaceVariant, spec.disabledTextColor);
    spec.hoverColor = spec.selectedDateColor;
    spec.hoverColor.setAlpha(20);
    spec.focusRingColor = roleOr(theme.colorScheme().primary, spec.focusRingColor);
    spec.headlineFont = theme.typography().headlineSmall;
    spec.dayFont = theme.typography().bodyMedium;
    return spec;
}

TableSpec makeTableSpec(const Theme& theme)
{
    TableSpec spec = defaultTableSpec();
    spec.backgroundColor = roleOr(theme.colorScheme().surface, spec.backgroundColor);
    spec.foregroundColor = roleOr(theme.colorScheme().onSurface, spec.foregroundColor);
    spec.headerBackgroundColor = roleOr(theme.colorScheme().surfaceContainer, spec.headerBackgroundColor);
    spec.headerForegroundColor = roleOr(theme.colorScheme().onSurfaceVariant, spec.headerForegroundColor);
    spec.gridColor = roleOr(theme.colorScheme().outlineVariant, spec.gridColor);
    spec.rowSelectedColor = roleOr(theme.colorScheme().secondaryContainer, spec.rowSelectedColor);
    spec.rowSelectedTextColor = roleOr(theme.colorScheme().onSecondaryContainer, spec.rowSelectedTextColor);
    spec.rowHoverColor = roleOr(theme.colorScheme().primary, spec.rowHoverColor);
    spec.rowHoverColor.setAlpha(15);
    spec.focusRingColor = roleOr(theme.colorScheme().primary, spec.focusRingColor);
    spec.headerFont = theme.typography().labelLarge;
    spec.bodyFont = theme.typography().bodyMedium;
    return spec;
}

GridListSpec makeGridListSpec(const Theme& theme)
{
    GridListSpec spec = defaultGridListSpec();
    spec.backgroundColor = roleOr(theme.colorScheme().surface, spec.backgroundColor);
    spec.foregroundColor = roleOr(theme.colorScheme().onSurface, spec.foregroundColor);
    spec.itemBackgroundColor = roleOr(theme.colorScheme().surfaceContainer, spec.itemBackgroundColor);
    spec.itemSelectedColor = roleOr(theme.colorScheme().secondaryContainer, spec.itemSelectedColor);
    spec.itemSelectedTextColor = roleOr(theme.colorScheme().onSecondaryContainer, spec.itemSelectedTextColor);
    spec.supportingTextColor = roleOr(theme.colorScheme().onSurfaceVariant, spec.supportingTextColor);
    spec.itemHoverColor = roleOr(theme.colorScheme().primary, spec.itemHoverColor);
    spec.itemHoverColor.setAlpha(15);
    spec.focusRingColor = roleOr(theme.colorScheme().primary, spec.focusRingColor);
    spec.titleFont = theme.typography().titleMedium;
    spec.supportingFont = theme.typography().bodySmall;
    return spec;
}

CarouselSpec makeCarouselSpec(const Theme& theme)
{
    CarouselSpec spec = defaultCarouselSpec();
    spec.backgroundColor = roleOr(theme.colorScheme().surfaceContainerLow, spec.backgroundColor);
    spec.foregroundColor = roleOr(theme.colorScheme().onSurface, spec.foregroundColor);
    spec.pageIndicatorColor = roleOr(theme.colorScheme().outlineVariant, spec.pageIndicatorColor);
    spec.activePageIndicatorColor = roleOr(theme.colorScheme().primary, spec.activePageIndicatorColor);
    spec.navigationButtonColor = roleOr(theme.colorScheme().primary, spec.navigationButtonColor);
    spec.navigationButtonTextColor = roleOr(theme.colorScheme().onPrimary, spec.navigationButtonTextColor);
    spec.focusRingColor = roleOr(theme.colorScheme().primary, spec.focusRingColor);
    spec.labelFont = theme.typography().labelLarge;
    return spec;
}

} // namespace QtMaterial
