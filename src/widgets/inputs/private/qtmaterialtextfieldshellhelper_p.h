#pragma once

#include <QRect>
#include <QString>

#include "qtmaterial/core/qtmaterialinteractionstate.h"
#include "qtmaterial/specs/qtmaterialtextfieldspec.h"
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

    struct ElidedText {
        QString labelText;
        QString supportingText;
        QString errorText;
    };

    static Layout layoutFor(const QRect& bounds, const TextFieldSpec& spec, const Theme& theme, Variant variant);
    static ElidedText elidedTextFor(const Layout& layout,
                                    const TextFieldSpec& spec,
                                    const QString& label,
                                    const QString& supportingText,
                                    const QString& errorText,
                                    const QFont& font);

    static void paintShell(
        QPainter* painter,
        const Layout& layout,
        const Theme& theme,
        const TextFieldSpec& spec,
        const QtMaterialInteractionState& state,
        Variant variant,
        const ElidedText& text,
        bool hasError,
        const QFont& font);

private:
    static QColor effectiveLabelColor(const TextFieldSpec& spec, bool focused, bool hasError, bool enabled);
    static QColor effectiveOutlineColor(const TextFieldSpec& spec, bool focused, bool hasError, bool enabled);
    static QColor effectiveSupportingColor(const TextFieldSpec& spec, bool hasError, bool enabled);
};

} // namespace QtMaterial
