#include <QtTest/QtTest>

#include <QImage>
#include <QPainter>
#include <QWidget>

#include "qtmaterial/effects/qtmaterialripplecontroller.h"

class tst_RippleController : public QObject
{
    Q_OBJECT

private slots:
    void activatesAfterAdd();
    void disabledSuppressesRipple();
    void reducedMotionSuppressesAnimatedRipple();
    void centerOriginUsesTargetCenter();
    void boundedAndUnboundedPaint();
};

void tst_RippleController::activatesAfterAdd()
{
    QWidget widget;
    widget.resize(80, 40);
    QtMaterial::QtMaterialRippleController controller(&widget);
    QVERIFY(!controller.isActive());
    controller.addRipple(QPointF(10, 10));
    QVERIFY(controller.isActive());
}

void tst_RippleController::disabledSuppressesRipple()
{
    QWidget widget;
    QtMaterial::QtMaterialRippleController controller(&widget);
    controller.setEnabled(false);
    controller.addRipple(QPointF(10, 10));
    QVERIFY(!controller.isActive());

    controller.setEnabled(true);
    widget.setEnabled(false);
    controller.addRipple(QPointF(10, 10));
    QVERIFY(!controller.isActive());
}

void tst_RippleController::reducedMotionSuppressesAnimatedRipple()
{
    QWidget widget;
    QtMaterial::QtMaterialRippleController controller(&widget);
    controller.setReducedMotion(true);
    controller.addRipple(QPointF(10, 10));
    QVERIFY(!controller.isActive());
}

void tst_RippleController::centerOriginUsesTargetCenter()
{
    QWidget widget;
    widget.resize(100, 60);
    QtMaterial::QtMaterialRippleController controller(&widget);
    controller.setOriginMode(
        QtMaterial::QtMaterialRippleController::OriginMode::Center);
    controller.addRipple(QPointF(1, 1));
    QVERIFY(controller.isActive());

    QImage image(100, 60, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    controller.paint(&painter, QColor(QStringLiteral("#6750A4")));
    painter.end();
    QVERIFY(!image.isNull());
}

void tst_RippleController::boundedAndUnboundedPaint()
{
    QWidget widget;
    widget.resize(80, 40);
    QtMaterial::QtMaterialRippleController controller(&widget);
    controller.setClipRect(QRectF(10, 5, 60, 30), 8.0);
    controller.addRipple(QPointF(20, 20));

    QImage image(80, 40, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    controller.setBoundsMode(
        QtMaterial::QtMaterialRippleController::BoundsMode::Bounded);
    controller.paint(&painter, QColor(QStringLiteral("#6750A4")));
    controller.setBoundsMode(
        QtMaterial::QtMaterialRippleController::BoundsMode::Unbounded);
    controller.paint(&painter, QColor(QStringLiteral("#6750A4")));
    painter.end();

    QVERIFY(!image.isNull());
}

QTEST_MAIN(tst_RippleController)
#include "tst_ripplecontroller.moc"
