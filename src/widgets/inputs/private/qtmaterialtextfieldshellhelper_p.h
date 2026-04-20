#pragma once

#include <QRect>

#include "qtmaterial/specs/qtmaterialtextfieldspec.h"
#include "qtmaterial/core/qtmaterialinteractionstate.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

class QPainter;
class QFont;

namespace QtMaterial {

class QtMaterialTextFieldShellHelper
{
public:
    enum class Variant {
        Outlined,
        Filled,
    };

    struct Layout {
        QRect containerRect;
        QRect labelRect;
        QRect editorRect;
        QRect supportingRect;
        QRect focusRect;
        qreal radius = 0.0;
    };

    static Layout layoutFor(const QRect& bounds, const TextFieldSpec& spec, const Theme& theme, Variant variant);

    static void paintShell(
        QPainter* painter,
        const Layout& layout,
        const Theme& theme,
        const TextFieldSpec& spec,
        const QtMaterialInteractionState& state,
        Variant variant,
        const QString& label,
        const QString& supportingText,
        const QString& errorText,
        bool hasError,
        const QFont& font);

private:
    static QColor effectiveLabelColor(const TextFieldSpec& spec, bool focused, bool hasError);
    static QColor effectiveOutlineColor(const TextFieldSpec& spec, bool focused, bool hasError);
};

} // namespace QtMaterial
