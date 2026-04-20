#pragma once

#include <QColor>
#include <QFont>
#include <QRect>
#include <QString>

#include "qtmaterial/specs/qtmateriallistitemspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

class QPainter;

namespace QtMaterial {

class ListItemRenderHelper
{
public:
    static QFont headlineFont(const ListItemSpec& spec, const Theme& theme);
    static QFont supportingFont(const ListItemSpec& spec, const Theme& theme);

    static QColor containerColorForState(const ListItemSpec& spec,
                                         bool enabled,
                                         bool hovered,
                                         bool selected,
                                         bool pressed);

    static qreal stateLayerOpacity(const StateLayer& stateLayer,
                                   bool hovered,
                                   bool focused,
                                   bool pressed);

    static void paintTexts(QPainter* painter,
                           const QRect& headlineRect,
                           const QRect& supportingRect,
                           const QString& headline,
                           const QString& supporting,
                           const QFont& headlineFont,
                           const QFont& supportingFont,
                           const QColor& headlineColor,
                           const QColor& supportingColor);

    static void paintDivider(QPainter* painter,
                             const QRect& bounds,
                             const ListItemSpec& spec);

    static QString elidedHeadline(const QString& text, const QFontMetrics& fm, int width);
    static QString elidedSupporting(const QString& text, const QFontMetrics& fm, int width);
};

} // namespace QtMaterial
