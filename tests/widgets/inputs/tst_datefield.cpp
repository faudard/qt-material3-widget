
#include <QtTest/QtTest>
#include <qlineedit.h>

#include "qtmaterial/widgets/inputs/qtmaterialdatefield.h"

class tst_DateField : public QObject
{
    Q_OBJECT

private slots:
    void construction();
    void dateRoundTrip();
    void invalidParseSetsError();
};

void tst_DateField::construction()
{
    QtMaterialDateField field(QStringLiteral("Date"));
    QCOMPARE(field.labelText(), QStringLiteral("Date"));
}

void tst_DateField::dateRoundTrip()
{
    QtMaterialDateField field;
    const QDate date(2026, 4, 19);
    field.setDate(date);
    QCOMPARE(field.date(), date);
}

void tst_DateField::invalidParseSetsError()
{
    QtMaterialDateField field;
    QSignalSpy spy(&field, &QtMaterialDateField::parseErrorChanged);
    QVERIFY(spy.isValid());

    if (field.lineEdit()) {
        field.lineEdit()->setText(QStringLiteral("not-a-date"));
    }
    QMetaObject::invokeMethod(&field, "handleEditorEditingFinished", Qt::DirectConnection);
    QVERIFY(spy.count() >= 1);
}

QTEST_MAIN(tst_DateField)
#include "tst_datefield.moc"
