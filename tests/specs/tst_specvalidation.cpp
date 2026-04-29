#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialspecvalidation.h"

class tst_SpecValidation : public QObject
{
    Q_OBJECT

private slots:
    void rejectsInvalidButtonGeometry();
    void warnsForSmallTouchTarget();
    void acceptsMinimalSnackbarSpec();
};

void tst_SpecValidation::rejectsInvalidButtonGeometry()
{
    QtMaterial::ButtonSpec spec;
    spec.labelColor = Qt::black;
    spec.iconColor = Qt::black;
    spec.stateLayerColor = Qt::black;
    spec.containerHeight = 0;

    const auto result = QtMaterial::validateButtonSpec(spec);
    QVERIFY(result.hasErrors());
}

void tst_SpecValidation::warnsForSmallTouchTarget()
{
    QtMaterial::IconButtonSpec spec;
    spec.iconColor = Qt::black;
    spec.stateLayerColor = Qt::black;
    spec.touchTarget = QSize(32, 32);

    const auto result = QtMaterial::validateIconButtonSpec(spec);
    QVERIFY(!result.issues().isEmpty());
    QVERIFY(result.isValid());
}

void tst_SpecValidation::acceptsMinimalSnackbarSpec()
{
    QtMaterial::SnackbarSpec spec;
    spec.containerColor = Qt::black;
    spec.textColor = Qt::white;
    spec.actionColor = Qt::yellow;
    spec.dismissIconColor = Qt::white;

    const auto result = QtMaterial::validateSnackbarSpec(spec);
    QVERIFY(result.isValid());
}

QTEST_MAIN(tst_SpecValidation)
#include "tst_specvalidation.moc"
