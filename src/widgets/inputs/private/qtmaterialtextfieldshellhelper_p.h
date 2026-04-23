#pragma once

#include <QColor>
#include <QRect>
#include <QString>

#include "qtmaterial/core/qtmaterialinteractionstate.h"
#include "qtmaterial/specs/qtmaterialtextfieldspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

class QFont;
class QPainter;

namespace QtMaterial {

class QtMaterialTextFieldShellHelper
{
public:
    enum class Variant {
        Outlined,
        Filled,
    };

    enum class EndActionMode {
        None,
        ClearText,
        TogglePasswordVisibility,
        CustomTrailingAction,
    };

    struct Accessories {
        QString prefixText;
        QString suffixText;
        QString customEndActionText;

        bool hasLeadingIcon = false;
        bool hasTrailingIcon = false;
        bool hasCustomEndActionIcon = false;

        EndActionMode endActionMode = EndActionMode::None;
        bool passwordVisible = false;

        Qt::LayoutDirection layoutDirection = Qt::LeftToRight;

        int iconExtent = 18;
        int accessorySpacing = 8;
        int accessoryTextPadding = 4;
        int minimumAccessoryTextWidth = 24;
    };

    struct Layout {
        QRect containerRect;
        QRect labelRect;
        QRect editorRect;
        QRect supportingRect;
        QRect focusRect;

        QRect leadingIconRect;
        QRect prefixRect;
        QRect suffixRect;
        QRect trailingIconRect;
        QRect endActionRect;

        qreal radius = 0.0;
    };

    struct ElidedText {
        QString labelText;
        QString supportingText;
        QString errorText;
        QString prefixText;
        QString suffixText;
        QString endActionText;
    };

    static Layout layoutFor(const QRect& bounds,
                            const TextFieldSpec& spec,
                            const Theme& theme,
                            Variant variant,
                            const Accessories& accessories,
                            const QFont& font);

    static ElidedText elidedTextFor(const Layout& layout,
                                    const TextFieldSpec& spec,
                                    const Accessories& accessories,
                                    const QString& label,
                                    const QString& supportingText,
                                    const QString& errorText,
                                    const QFont& font);

    static void paintShell(QPainter* painter,
                           const Layout& layout,
                           const Theme& theme,
                           const TextFieldSpec& spec,
                           const QtMaterialInteractionState& state,
                           Variant variant,
                           const ElidedText& text,
                           bool hasError,
                           const QFont& font);

private:
    static QColor effectiveLabelColor(const TextFieldSpec& spec,
                                      bool focused,
                                      bool hasError,
                                      bool enabled);

    static QColor effectiveOutlineColor(const TextFieldSpec& spec,
                                        bool focused,
                                        bool hasError,
                                        bool enabled);

    static QColor effectiveSupportingColor(const TextFieldSpec& spec,
                                           bool hasError,
                                           bool enabled);
};

} // namespace QtMaterial