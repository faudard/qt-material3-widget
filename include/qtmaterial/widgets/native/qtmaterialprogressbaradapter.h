#pragma once

#include "qtmaterial/qtmaterialglobal.h"

class QProgressBar;
class QWidget;

namespace QtMaterial {

/**
 * Opt-in Material 3 rendering for an existing QProgressBar.
 *
 * Range/value, orientation, inverted appearance, format/text visibility,
 * accessibility and valueChanged semantics remain owned by QProgressBar.
 */
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialProgressBarAdapter final
{
public:
    static void apply(QProgressBar* progressBar);
    static void remove(QProgressBar* progressBar);
    static bool isApplied(const QProgressBar* progressBar);

    static void setOptOut(QProgressBar* progressBar, bool excluded);
    static bool isOptedOut(const QProgressBar* progressBar);

    static int applyToDescendants(QWidget* root);

    static const char* appliedPropertyName() noexcept;
    static const char* optOutPropertyName() noexcept;
};

} // namespace QtMaterial
