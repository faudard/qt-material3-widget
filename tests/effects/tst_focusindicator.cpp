#include <QtTest/QtTest>

#include <QImage>
#include <QPainter>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"

using namespace QtMaterial;

class tst_FocusIndicator : public QObject
{
    Q_OBJECT

private slots:
    void paintsIntoImage();
    void keyboardVisibilityPolicy_data();
    void keyboardVisibilityPolicy();
    void tokenizedRingPaints();
};

void tst_FocusIndicator::paintsIntoImage()
{
    QImage image(64, 64, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    QtMaterialFocusIndicator::paintRectFocusRing(
        &painter,
        QRectF(8, 8, 48, 48),
        QColor("#00639B"),
        12.0,
        2.0);
    QVERIFY(!image.isNull());
}

void tst_FocusIndicator::keyboardVisibilityPolicy_data()
{
    QTest::addColumn<bool>("focused");
    QTest::addColumn<bool>("enabled");
    QTest::addColumn<bool>("strong");
    QTest::addColumn<bool>("keyboardVisible");
    QTest::addColumn<Qt::FocusReason>("reason");
    QTest::addColumn<bool>("expected");

    QTest::newRow("tab")
        << true << true << false << true
        << Qt::TabFocusReason << true;
    QTest::newRow("mouse-hidden")
        << true << true << false << true
        << Qt::MouseFocusReason << false;
    QTest::newRow("strong-mouse")
        << true << true << true << true
        << Qt::MouseFocusReason << true;
    QTest::newRow("disabled")
        << true << false << true << true
        << Qt::TabFocusReason << false;
    QTest::newRow("not-focused")
        << false << true << true << true
        << Qt::TabFocusReason << false;
}

void tst_FocusIndicator::keyboardVisibilityPolicy()
{
    QFETCH(bool, focused);
    QFETCH(bool, enabled);
    QFETCH(bool, strong);
    QFETCH(bool, keyboardVisible);
    QFETCH(Qt::FocusReason, reason);
    QFETCH(bool, expected);

    QtMaterialInteractionState state;
    state.setFocused(focused);
    state.setEnabled(enabled);

    InteractionStateTokens policy;
    policy.strongFocusIndicators = strong;
    policy.keyboardFocusVisible = keyboardVisible;

    QCOMPARE(
        QtMaterialFocusIndicator::shouldShow(state, reason, policy),
        expected);
}

void tst_FocusIndicator::tokenizedRingPaints()
{
    QImage image(96, 96, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);

    FocusRingTokens tokens;
    tokens.width = 3;
    tokens.offset = 2;
    tokens.radiusAdjustment = 1;
    tokens.opacity = 0.75;
    tokens.color = QColor(QStringLiteral("#00639B"));

    QtMaterialFocusIndicator::paintRectFocusRing(
        &painter,
        QRectF(16, 16, 64, 48),
        12.0,
        tokens);
    painter.end();

    QVERIFY(!image.isNull());
}

QTEST_MAIN(tst_FocusIndicator)
#include "tst_focusindicator.moc"
