#pragma once

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class Theme;

/**
 * Structural value comparison for Theme.
 *
 * This is a model-level semantic comparison. It must not depend on ThemeIO,
 * JSON/XML serialization, QObject identity, pointer identity, hash iteration
 * order, or process-global state.
 */
QTMATERIAL3_THEME_MODEL_EXPORT bool operator==(const Theme& lhs, const Theme& rhs);
QTMATERIAL3_THEME_MODEL_EXPORT bool operator!=(const Theme& lhs, const Theme& rhs);

} // namespace QtMaterial
