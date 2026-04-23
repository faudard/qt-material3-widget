#include <QSignalSpy>
#include <QtTest/QtTest>

#include "qtmaterial/widgets/progress/qtmaterialcircularprogressindicator.h"
#include "qtmaterial/widgets/progress/qtmateriallinearprogressindicator.h"

using QtMaterial::ProgressIndicatorSpec;
using QtMaterial::QtMaterialCircularProgressIndicator;
using QtMaterial::QtMaterialLinearProgressIndicator;

class tst_ProgressIndicators : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void linearValueIsClamped();
    void circularValueIsClamped();
    void duplicateModeChangesDoNotEmit();
    void publicApiMutatorsEmitSpecChanged();
    void resetApiRestoresDefaults();
    void sizeHintsFollowSpec();
    void specRoundTrip();
    void indeterminateModeCanBeShownAndHidden();
};

void tst_ProgressIndicators::linearValueIsClamped()
{
    QtMaterialLinearProgressIndicator indicator;
    QSignalSpy spy(&indicator, &QtMaterialLinearProgressIndicator::valueChanged);

    indicator.setValue(-1.0);
    QCOMPARE(indicator.value(), 0.0);
    QCOMPARE(spy.count(), 0);

    indicator.setValue(0.42);
    QCOMPARE(indicator.value(), 0.42);
    QCOMPARE(spy.count(), 1);

    indicator.setValue(2.0);
    QCOMPARE(indicator.value(), 1.0);
    QCOMPARE(spy.count(), 2);
}

void tst_ProgressIndicators::circularValueIsClamped()
{
    QtMaterialCircularProgressIndicator indicator;
    QSignalSpy spy(&indicator, &QtMaterialCircularProgressIndicator::valueChanged);

    indicator.setValue(-1.0);
    QCOMPARE(indicator.value(), 0.0);
    QCOMPARE(spy.count(), 0);

    indicator.setValue(0.75);
    QCOMPARE(indicator.value(), 0.75);
    QCOMPARE(spy.count(), 1);

    indicator.setValue(9.0);
    QCOMPARE(indicator.value(), 1.0);
    QCOMPARE(spy.count(), 2);
}

void tst_ProgressIndicators::duplicateModeChangesDoNotEmit()
{
    QtMaterialLinearProgressIndicator linear;
    QSignalSpy linearSpy(&linear, &QtMaterialLinearProgressIndicator::modeChanged);
    linear.setMode(QtMaterialLinearProgressIndicator::Mode::Indeterminate);
    linear.setMode(QtMaterialLinearProgressIndicator::Mode::Indeterminate);
    QCOMPARE(linear.mode(), QtMaterialLinearProgressIndicator::Mode::Indeterminate);
    QCOMPARE(linearSpy.count(), 1);

    QtMaterialCircularProgressIndicator circular;
    QSignalSpy circularSpy(&circular, &QtMaterialCircularProgressIndicator::modeChanged);
    circular.setMode(QtMaterialCircularProgressIndicator::Mode::Indeterminate);
    circular.setMode(QtMaterialCircularProgressIndicator::Mode::Indeterminate);
    QCOMPARE(circular.mode(), QtMaterialCircularProgressIndicator::Mode::Indeterminate);
    QCOMPARE(circularSpy.count(), 1);
}

void tst_ProgressIndicators::publicApiMutatorsEmitSpecChanged()
{
    QtMaterialLinearProgressIndicator linear;
    QSignalSpy linearSpy(&linear, &QtMaterialLinearProgressIndicator::specChanged);
    linear.setActiveColor(Qt::red);
    linear.setTrackColor(Qt::blue);
    linear.setTrackGap(6);
    linear.setStopIndicatorSize(8);
    QCOMPARE(linear.activeColor(), QColor(Qt::red));
    QCOMPARE(linear.trackColor(), QColor(Qt::blue));
    QCOMPARE(linear.trackGap(), 6);
    QCOMPARE(linear.stopIndicatorSize(), 8);
    QCOMPARE(linearSpy.count(), 4);

    QtMaterialCircularProgressIndicator circular;
    QSignalSpy circularSpy(&circular, &QtMaterialCircularProgressIndicator::specChanged);
    circular.setActiveColor(Qt::green);
    circular.setTrackColor(Qt::yellow);
    circular.setTrackGap(5);
    circular.setStrokeWidth(7);
    QCOMPARE(circular.activeColor(), QColor(Qt::green));
    QCOMPARE(circular.trackColor(), QColor(Qt::yellow));
    QCOMPARE(circular.trackGap(), 5);
    QCOMPARE(circular.strokeWidth(), 7);
    QCOMPARE(circularSpy.count(), 4);
}

void tst_ProgressIndicators::resetApiRestoresDefaults()
{
    QtMaterialLinearProgressIndicator linear;
    linear.setValue(0.5);
    linear.resetValue();
    QCOMPARE(linear.value(), 0.0);
    linear.setActiveColor(Qt::red);
    linear.resetActiveColor();
    QVERIFY(!linear.activeColor().isValid());

    QtMaterialCircularProgressIndicator circular;
    circular.setValue(0.8);
    circular.resetValue();
    QCOMPARE(circular.value(), 0.0);
    circular.setTrackColor(Qt::blue);
    circular.resetTrackColor();
    QVERIFY(!circular.trackColor().isValid());
}

void tst_ProgressIndicators::sizeHintsFollowSpec()
{
    ProgressIndicatorSpec spec;
    spec.linearHeight = 6;
    spec.circularSize = QSize(56, 56);

    QtMaterialLinearProgressIndicator linear(spec);
    QVERIFY(linear.sizeHint().height() >= 6);
    QVERIFY(linear.minimumSizeHint().width() <= linear.sizeHint().width());

    QtMaterialCircularProgressIndicator circular(spec);
    QCOMPARE(circular.sizeHint(), QSize(56, 56));
    QCOMPARE(circular.minimumSizeHint(), QSize(24, 24));
}

void tst_ProgressIndicators::specRoundTrip()
{
    ProgressIndicatorSpec spec;
    spec.activeColor = QColor("#6750A4");
    spec.trackColor = QColor("#E7E0EC");
    spec.linearHeight = 8;
    spec.circularStrokeWidth = 6;
    spec.trackGap = 3;
    spec.stopIndicatorSize = 5;

    QtMaterialLinearProgressIndicator linear;
    QSignalSpy linearSpy(&linear, &QtMaterialLinearProgressIndicator::specChanged);
    linear.setSpec(spec);
    QCOMPARE(linear.spec().activeColor, spec.activeColor);
    QCOMPARE(linear.spec().trackColor, spec.trackColor);
    QCOMPARE(linear.spec().linearHeight, 8);
    QCOMPARE(linear.spec().trackGap, 3);
    QCOMPARE(linearSpy.count(), 1);

    QtMaterialCircularProgressIndicator circular;
    QSignalSpy circularSpy(&circular, &QtMaterialCircularProgressIndicator::specChanged);
    circular.setSpec(spec);
    QCOMPARE(circular.spec().circularStrokeWidth, 6);
    QCOMPARE(circular.spec().trackGap, 3);
    QCOMPARE(circularSpy.count(), 1);
}

void tst_ProgressIndicators::indeterminateModeCanBeShownAndHidden()
{
    QtMaterialLinearProgressIndicator linear;
    linear.setMode(QtMaterialLinearProgressIndicator::Mode::Indeterminate);
    linear.show();
    QVERIFY(QTest::qWaitForWindowExposed(&linear));
    linear.hide();
    QCOMPARE(linear.mode(), QtMaterialLinearProgressIndicator::Mode::Indeterminate);

    QtMaterialCircularProgressIndicator circular;
    circular.setMode(QtMaterialCircularProgressIndicator::Mode::Indeterminate);
    circular.show();
    QVERIFY(QTest::qWaitForWindowExposed(&circular));
    circular.hide();
    QCOMPARE(circular.mode(), QtMaterialCircularProgressIndicator::Mode::Indeterminate);
}

QTEST_MAIN(tst_ProgressIndicators)
#include "tst_progressindicators.moc"
