#include <QtTest/QtTest>

#include <QImage>
#include <QPainter>

#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"

using namespace QtMaterial;

class tst_StateLayerPainter : public QObject
{
    Q_OBJECT

private slots:
    void paintsIntoImage();
    void interactionPriority_data();
    void interactionPriority();
    void policyCanSuppressPointerFeedback();
};

void tst_StateLayerPainter::paintsIntoImage()
{
    QImage image(64, 64, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    QtMaterialStateLayerPainter::paintRect(
        &painter,
        QRectF(8, 8, 48, 48),
        QColor("#6750A4"),
        0.12,
        12.0);
    QVERIFY(!image.isNull());
}

void tst_StateLayerPainter::interactionPriority_data()
{
    QTest::addColumn<QString>("stateName");
    QTest::addColumn<qreal>("expected");

    QTest::newRow("hover") << QStringLiteral("hover") << 0.08;
    QTest::newRow("focus") << QStringLiteral("focus") << 0.12;
    QTest::newRow("press") << QStringLiteral("press") << 0.14;
    QTest::newRow("drag") << QStringLiteral("drag") << 0.18;
    QTest::newRow("disabled") << QStringLiteral("disabled") << 0.0;
}

void tst_StateLayerPainter::interactionPriority()
{
    QFETCH(QString, stateName);
    QFETCH(qreal, expected);

    QtMaterialInteractionState state;
    StateLayer layer;
    layer.hoverOpacity = 0.08;
    layer.focusOpacity = 0.12;
    layer.pressOpacity = 0.14;
    layer.dragOpacity = 0.18;
    InteractionStateTokens policy;

    if (stateName == QStringLiteral("hover")) {
        state.setHovered(true);
    } else if (stateName == QStringLiteral("focus")) {
        state.setHovered(true);
        state.setFocused(true);
    } else if (stateName == QStringLiteral("press")) {
        state.setHovered(true);
        state.setFocused(true);
        state.setPressed(true);
    } else if (stateName == QStringLiteral("drag")) {
        state.setHovered(true);
        state.setFocused(true);
        state.setPressed(true);
        state.setDragged(true);
    } else {
        state.setHovered(true);
        state.setEnabled(false);
    }

    QCOMPARE(
        QtMaterialStateLayerPainter::opacityForState(state, layer, policy),
        expected);
}

void tst_StateLayerPainter::policyCanSuppressPointerFeedback()
{
    QtMaterialInteractionState state;
    state.setHovered(true);
    state.setPressed(true);

    StateLayer layer;
    layer.hoverOpacity = 0.08;
    layer.pressOpacity = 0.12;

    InteractionStateTokens policy;
    policy.hoverFeedbackEnabled = false;
    policy.pressFeedbackEnabled = false;

    QCOMPARE(
        QtMaterialStateLayerPainter::opacityForState(state, layer, policy),
        0.0);
}

QTEST_MAIN(tst_StateLayerPainter)
#include "tst_statelayerpainter.moc"
