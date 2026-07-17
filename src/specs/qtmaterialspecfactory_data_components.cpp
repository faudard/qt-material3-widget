#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"

#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/specs/qtmaterialdataspecresolver.h"

namespace QtMaterial {
namespace {

QColor roleOr(const Theme& theme, ColorRole role, const QColor& fallback)
{
    const QColor candidate = theme.colorScheme().color(role);
    return candidate.isValid() ? candidate : fallback;
}

} // namespace

// These free functions are intentionally additive. If the project prefers central
// SpecFactory static methods, move the function bodies into SpecFactory.

DatePickerSpec makeDatePickerSpec(const Theme& theme)
{
    DatePickerSpec spec = defaultDatePickerSpec();

    spec.backgroundColor =
        roleOr(theme, ColorRole::SurfaceContainerHigh, spec.backgroundColor);
    spec.foregroundColor =
        roleOr(theme, ColorRole::OnSurface, spec.foregroundColor);
    spec.headlineColor =
        roleOr(theme, ColorRole::OnSurface, spec.headlineColor);
    spec.selectedDateColor =
        roleOr(theme, ColorRole::Primary, spec.selectedDateColor);
    spec.selectedDateTextColor =
        roleOr(theme, ColorRole::OnPrimary, spec.selectedDateTextColor);
    spec.todayOutlineColor =
        roleOr(theme, ColorRole::Primary, spec.todayOutlineColor);
    spec.disabledTextColor =
        roleOr(theme, ColorRole::OnSurfaceVariant, spec.disabledTextColor);

    spec.hoverColor = spec.selectedDateColor;
    spec.hoverColor.setAlpha(20);

    spec.focusRingColor =
        roleOr(theme, ColorRole::Primary, spec.focusRingColor);

    spec.headlineFont = theme.typography().headlineSmall;
    spec.dayFont = theme.typography().bodyMedium;

    return spec;
}

TableSpec makeTableSpec(const Theme& theme)
{
    return DataSpecResolver().tableSpec(theme);
}

GridListSpec makeGridListSpec(const Theme& theme)
{
    GridListSpec spec = defaultGridListSpec();

    spec.backgroundColor =
        roleOr(theme, ColorRole::Surface, spec.backgroundColor);
    spec.foregroundColor =
        roleOr(theme, ColorRole::OnSurface, spec.foregroundColor);
    spec.itemBackgroundColor =
        roleOr(theme, ColorRole::SurfaceContainer, spec.itemBackgroundColor);
    spec.itemSelectedColor =
        roleOr(theme, ColorRole::SecondaryContainer, spec.itemSelectedColor);
    spec.itemSelectedTextColor =
        roleOr(theme, ColorRole::OnSecondaryContainer, spec.itemSelectedTextColor);
    spec.supportingTextColor =
        roleOr(theme, ColorRole::OnSurfaceVariant, spec.supportingTextColor);

    spec.itemHoverColor =
        roleOr(theme, ColorRole::Primary, spec.itemHoverColor);
    spec.itemHoverColor.setAlpha(15);

    spec.focusRingColor =
        roleOr(theme, ColorRole::Primary, spec.focusRingColor);

    spec.titleFont = theme.typography().titleMedium;
    spec.supportingFont = theme.typography().bodySmall;

    return spec;
}

CarouselSpec makeCarouselSpec(const Theme& theme)
{
    CarouselSpec spec = defaultCarouselSpec();

    spec.backgroundColor =
        roleOr(theme, ColorRole::SurfaceContainerLow, spec.backgroundColor);
    spec.foregroundColor =
        roleOr(theme, ColorRole::OnSurface, spec.foregroundColor);
    spec.pageIndicatorColor =
        roleOr(theme, ColorRole::OutlineVariant, spec.pageIndicatorColor);
    spec.activePageIndicatorColor =
        roleOr(theme, ColorRole::Primary, spec.activePageIndicatorColor);
    spec.navigationButtonColor =
        roleOr(theme, ColorRole::Primary, spec.navigationButtonColor);
    spec.navigationButtonTextColor =
        roleOr(theme, ColorRole::OnPrimary, spec.navigationButtonTextColor);
    spec.focusRingColor =
        roleOr(theme, ColorRole::Primary, spec.focusRingColor);

    spec.labelFont = theme.typography().labelLarge;

    return spec;
}

} // namespace QtMaterial