#include "qtmaterial/effects/qtmaterialshadowrenderer.h"

#include "qtmaterial/effects/qtmaterialshadowcache.h"

#include <QPainter>
#include <QPixmap>
#include <QtGlobal>
#include <QtMath>

namespace QtMaterial {

void QtMaterialShadowRenderer::paintRoundedShadow(
    QPainter* painter,
    const QRectF& rect,
    qreal radius,
    const QColor& shadowColor,
    int blurRadius,
    int yOffset)
{
    if (!painter
        || !painter->device()
        || rect.isEmpty()
        || !shadowColor.isValid()) {
        return;
    }

    const qreal dpr =
        qMax<qreal>(0.25, painter->device()->devicePixelRatioF());
    const int margin =
        qMax(2, qMax(0, blurRadius) + qAbs(yOffset) + 2);
    const QSize logicalSize(
        qMax(1, qCeil(rect.width()) + margin * 2),
        qMax(1, qCeil(rect.height()) + margin * 2));

    QtMaterialShadowCache cache;
    const QString key = cache.keyFor(
        logicalSize.width(),
        logicalSize.height(),
        radius,
        blurRadius,
        yOffset,
        shadowColor,
        dpr);

    QPixmap shadow;
    if (!cache.find(key, &shadow)) {
        const QSize physicalSize(
            qMax(1, qCeil(logicalSize.width() * dpr)),
            qMax(1, qCeil(logicalSize.height() * dpr)));

        shadow = QPixmap(physicalSize);
        shadow.setDevicePixelRatio(dpr);
        shadow.fill(Qt::transparent);

        QPainter shadowPainter(&shadow);
        shadowPainter.setRenderHint(QPainter::Antialiasing, true);

        QColor fill = shadowColor;
        const int derivedAlpha =
            qMin(255, 32 + qMax(0, blurRadius) * 4);
        fill.setAlpha(
            qMin(
                shadowColor.alpha(),
                derivedAlpha));

        shadowPainter.setPen(Qt::NoPen);
        shadowPainter.setBrush(fill);
        shadowPainter.drawRoundedRect(
            QRectF(
                margin,
                margin + yOffset,
                rect.width(),
                rect.height()),
            qMax<qreal>(0.0, radius),
            qMax<qreal>(0.0, radius));
        shadowPainter.end();

        cache.insert(key, shadow);
    }

    painter->save();
    painter->drawPixmap(
        rect.topLeft() - QPointF(margin, margin),
        shadow);
    painter->restore();
}

} // namespace QtMaterial
