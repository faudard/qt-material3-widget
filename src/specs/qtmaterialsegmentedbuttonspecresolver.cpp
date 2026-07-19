#include "qtmaterial/specs/qtmaterialsegmentedbuttonspecresolver.h"

#include "private/qtmaterialdensitymetrics_p.h"

#include <QtGlobal>

namespace QtMaterial {

SegmentedButtonSpec SegmentedButtonSpecResolver::segmentedButtonSpec(
    const Theme& theme,
    Density density) const
{
    SegmentedButtonSpec spec;
    spec.containerColor = Qt::transparent;
    spec.selectedContainerColor =
        theme.colorScheme().color(ColorRole::SecondaryContainer);
    spec.labelColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.selectedLabelColor =
        theme.colorScheme().color(ColorRole::OnSecondaryContainer);
    spec.outlineColor = theme.colorScheme().color(ColorRole::Outline);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.focusRingColor = theme.colorScheme().color(ColorRole::Primary);
    spec.disabledLabelColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.disabledOutlineColor =
        theme.colorScheme().color(ColorRole::OutlineVariant);
    spec.segmentHeight =
        SpecsPrivate::adjustedMetric(40, density, -4, 4);
    spec.touchTarget = QSize(48, qMax(48, spec.segmentHeight));
    return spec;
}

} // namespace QtMaterial
