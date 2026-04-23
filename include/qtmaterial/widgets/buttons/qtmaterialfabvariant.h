#pragma once

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

/**
 * @brief Color variant used by Material floating action buttons.
 *
 * The variant controls the resolved container, content, state-layer and focus
 * colors while leaving sizing, shape, elevation and motion tokens unchanged.
 */
enum class QTMATERIAL3_WIDGETS_EXPORT QtMaterialFabVariant {
    Primary,
    Secondary,
    Tertiary,
    Surface,
};

} // namespace QtMaterial
