#include <QtTest/QtTest>

#include <QFocusEvent>
#include <QImage>
#include <QPainter>

#include "qtmaterial/widgets/inputs/qtmaterialrangeslider.h"
#include "qtmaterial/widgets/inputs/qtmaterialslider.h"

using namespace QtMaterial;

namespace {

class ExposedRangeSlider final : public QtMaterialRangeSlider
{
public:
    using QtMaterialRangeSlider::focusInEvent;
    using QtMaterialRangeSlider::focusNextPrevChild;
};

QImage renderAtDpr(QWidget& widget, const QSize& logicalSize, qreal dpr)
{
    widget.resize(logicalSize);
    widget.ensurePolished();

    QImage image(
        qMax(1, qRound(logicalSize.width() * dpr)),
        qMax(1, qRound(logicalSize.height() * dpr)),
        QImage::Format_ARGB32_Premultiplied);
    image.setDevicePixelRatio(dpr);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    widget.render(&painter);
    painter.end();
    return image;
}

} // namespace

class tst_SliderMaturity : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void accessibleValuesStaySynchronized();
    void rangeSliderKeyboardFollowsVisualDirection();
    void rangeSliderKeyboardCanReachBothHandles();
    void homeEndRespectActiveHandleBounds();
    void desktopDprRenderingSmoke_data();
    void desktopDprRenderingSmoke();
};

void tst_SliderMaturity::accessibleValuesStaySynchronized()
{
    QtMaterialSlider slider;
    QCOMPARE(slider.accessibleName(), QStringLiteral("Slider"));
    QCOMPARE(slider.accessibleDescription(), QStringLiteral("Value 0"));

    slider.setValue(42);
    QCOMPARE(slider.accessibleDescription(), QStringLiteral("Value 42"));

    ExposedRangeSlider range;
    QCOMPARE(range.accessibleName(), QStringLiteral("Range slider"));
    QVERIFY(range.accessibleDescription().contains(QStringLiteral("Lower 25")));
    QVERIFY(range.accessibleDescription().contains(QStringLiteral("upper 75")));
    QVERIFY(range.accessibleDescription().contains(QStringLiteral("Lower handle active")));

    range.setValues(20, 80);
    QVERIFY(range.accessibleDescription().contains(QStringLiteral("Lower 20")));
    QVERIFY(range.accessibleDescription().contains(QStringLiteral("upper 80")));

    range.setRange(30, 70);
    QVERIFY(range.accessibleDescription().contains(QStringLiteral("Lower 30")));
    QVERIFY(range.accessibleDescription().contains(QStringLiteral("upper 70")));
}

void tst_SliderMaturity::rangeSliderKeyboardFollowsVisualDirection()
{
    ExposedRangeSlider range;
    range.setRange(0, 100);
    range.setValues(25, 75);

    range.setOrientation(Qt::Horizontal);
    range.setLayoutDirection(Qt::LeftToRight);
    QTest::keyClick(&range, Qt::Key_Right);
    QCOMPARE(range.lowerValue(), 26);

    range.setValues(25, 75);
    range.setLayoutDirection(Qt::RightToLeft);
    QTest::keyClick(&range, Qt::Key_Right);
    QCOMPARE(range.lowerValue(), 24);

    range.setValues(25, 75);
    range.setOrientation(Qt::Vertical);
    range.setLayoutDirection(Qt::RightToLeft);
    QTest::keyClick(&range, Qt::Key_Up);
    QCOMPARE(range.lowerValue(), 26);
    QTest::keyClick(&range, Qt::Key_Down);
    QCOMPARE(range.lowerValue(), 25);
}

void tst_SliderMaturity::rangeSliderKeyboardCanReachBothHandles()
{
    ExposedRangeSlider range;
    range.setRange(0, 100);
    range.setValues(25, 75);

    QVERIFY(range.focusNextPrevChild(true));
    QVERIFY(range.accessibleDescription().contains(QStringLiteral("Upper handle active")));

    QTest::keyClick(&range, Qt::Key_Right);
    QCOMPARE(range.lowerValue(), 25);
    QCOMPARE(range.upperValue(), 76);

    QVERIFY(range.focusNextPrevChild(false));
    QVERIFY(range.accessibleDescription().contains(QStringLiteral("Lower handle active")));

    QTest::keyClick(&range, Qt::Key_Left);
    QCOMPARE(range.lowerValue(), 24);
    QCOMPARE(range.upperValue(), 76);

    QFocusEvent backwardEntry(QEvent::FocusIn, Qt::BacktabFocusReason);
    range.focusInEvent(&backwardEntry);
    QVERIFY(range.accessibleDescription().contains(QStringLiteral("Upper handle active")));
}

void tst_SliderMaturity::homeEndRespectActiveHandleBounds()
{
    ExposedRangeSlider range;
    range.setRange(0, 100);
    range.setValues(25, 75);

    QTest::keyClick(&range, Qt::Key_Home);
    QCOMPARE(range.lowerValue(), 0);
    QCOMPARE(range.upperValue(), 75);

    range.setValues(25, 75);
    QTest::keyClick(&range, Qt::Key_End);
    QCOMPARE(range.lowerValue(), 75);
    QCOMPARE(range.upperValue(), 75);

    range.setValues(25, 75);
    QVERIFY(range.focusNextPrevChild(true));

    QTest::keyClick(&range, Qt::Key_End);
    QCOMPARE(range.lowerValue(), 25);
    QCOMPARE(range.upperValue(), 100);

    range.setValues(25, 75);
    QTest::keyClick(&range, Qt::Key_Home);
    QCOMPARE(range.lowerValue(), 25);
    QCOMPARE(range.upperValue(), 25);
}

void tst_SliderMaturity::desktopDprRenderingSmoke_data()
{
    QTest::addColumn<qreal>("dpr");
    QTest::newRow("100-percent") << qreal(1.00);
    QTest::newRow("125-percent") << qreal(1.25);
    QTest::newRow("150-percent") << qreal(1.50);
    QTest::newRow("175-percent") << qreal(1.75);
    QTest::newRow("200-percent") << qreal(2.00);
}

void tst_SliderMaturity::desktopDprRenderingSmoke()
{
    QFETCH(qreal, dpr);
    QtMaterialSlider slider(Qt::Horizontal);
    slider.setRange(0, 100);
    slider.setValue(64);
    const QImage sliderImage = renderAtDpr(slider, QSize(240, 48), dpr);
    QVERIFY(!sliderImage.isNull());
    QCOMPARE(sliderImage.devicePixelRatio(), dpr);

    ExposedRangeSlider range;
    range.setRange(0, 100);
    range.setValues(20, 80);
    const QImage rangeImage = renderAtDpr(range, QSize(240, 48), dpr);
    QVERIFY(!rangeImage.isNull());
    QCOMPARE(rangeImage.devicePixelRatio(), dpr);

    range.setOrientation(Qt::Vertical);
    const QImage verticalImage = renderAtDpr(range, QSize(48, 240), dpr);
    QVERIFY(!verticalImage.isNull());
    QCOMPARE(verticalImage.devicePixelRatio(), dpr);
}

QTEST_MAIN(tst_SliderMaturity)
#include "tst_slider_maturity.moc"
