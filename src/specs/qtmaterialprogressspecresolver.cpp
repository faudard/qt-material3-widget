#include "qtmaterial/specs/qtmaterialprogressspecresolver.h"

#include <QtGlobal>

namespace QtMaterial {

ProgressIndicatorSpec ProgressSpecResolver::resolve(
    const Theme& theme,
    const ProgressIndicatorSpec& requested) const
{
    ProgressIndicatorSpec resolved = requested;

    if (!resolved.activeColor.isValid()) {
        resolved.activeColor =
            theme.colorScheme().color(resolved.activeColorRole);
    }
    if (!resolved.trackColor.isValid()) {
        resolved.trackColor =
            theme.colorScheme().color(resolved.trackColorRole);
    }
    if (!resolved.stopIndicatorColor.isValid()) {
        resolved.stopIndicatorColor = resolved.activeColor;
    }

    if (resolved.cornerRadius < 0.0) {
        if (resolved.shapeRole == ShapeRole::Full) {
            // Dynamic pill radius: computed from the actual indicator height.
            resolved.cornerRadius = -1.0;
        } else if (theme.shapes().contains(resolved.shapeRole)) {
            resolved.cornerRadius =
                qMax<qreal>(
                    0.0,
                    theme.shapes().radius(resolved.shapeRole));
        } else {
            resolved.cornerRadius = 0.0;
        }
    }

    return resolved;
}

} // namespace QtMaterial
