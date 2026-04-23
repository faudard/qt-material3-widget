#include <QtTest/QtTest>
#include <QSignalSpy>

#include "qtmaterial/widgets/progress/qtmateriallinearprogressindicator.h"
#include "qtmaterial/widgets/progress/qtmaterialcircularprogressindicator.h"

using QtMaterial::QtMaterialCircularProgressIndicator;
using QtMaterial::QtMaterialLinearProgressIndicator;
using QtMaterial::ProgressIndicatorSpec;

class tst_ProgressIndicators : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void linearValueIsClamped();
    void circularValueIsClamped();
    void modeSignalsAreEmittedOnce();
    void sizeHintsFollowSpec();
    void specRoundTrip();
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

void tst_ProgressIndicators::modeSignalsAreEmittedOnce()
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

void tst_ProgressIndicators::sizeHintsFollowSpec()
{
    ProgressIndicatorSpec spec;
    spec.linearHeight = 6;
    spec.circularSize = QSize(56, 56);

    QtMaterialLinearProgressIndicator linear(spec);
    QCOMPARE(linear.sizeHint().height(), 6);
    QVERIFY(linear.sizeHint().width() >= 160);

    QtMaterialCircularProgressIndicator circular(spec);
    QCOMPARE(circular.sizeHint(), QSize(56, 56));
}

void tst_ProgressIndicators::specRoundTrip()
{
    ProgressIndicatorSpec spec;
    spec.activeColor = QColor(0, 128, 255);
    spec.trackColor = QColor(200, 200, 200);
    spec.linearHeight = 8;
    spec.circularStrokeWidth = 5;
    spec.animationDurationMs = 900;

    QtMaterialLinearProgressIndicator linear;
    QSignalSpy linearSpy(&linear, &QtMaterialLinearProgressIndicator::specChanged);
    linear.setSpec(spec);
    QCOMPARE(linear.spec().activeColor, spec.activeColor);
    QCOMPARE(linear.spec().trackColor, spec.trackColor);
    QCOMPARE(linear.spec().linearHeight, 8);
    QCOMPARE(linearSpy.count(), 1);

    QtMaterialCircularProgressIndicator circular;
    QSignalSpy circularSpy(&circular, &QtMaterialCircularProgressIndicator::specChanged);
    circular.setSpec(spec);
    QCOMPARE(circular.spec().circularStrokeWidth, 5);
    QCOMPARE(circular.spec().animationDurationMs, 900);
    QCOMPARE(circularSpy.count(), 1);
}

QTEST_MAIN(tst_ProgressIndicators)
#include "tst_progressindicators.moc"
