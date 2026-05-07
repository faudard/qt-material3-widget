#include <QSignalSpy>
#include <QTest>

#include "qtmaterial/widgets/inputs/qtmaterialdatefield.h"

class tst_DateField : public QObject
{
    Q_OBJECT

private slots:
    void construction();
    void dateRoundTrip();
    void invalidParseSetsError();
    void displayFormatRoundTrip();
    void clearableRoundTrip();
    void rangeRejectsDateBeforeMinimum();
    void rangeRejectsDateAfterMaximum();
    void rangeAcceptsBoundaryDates();
    void acceptableSignalChangesOnlyOnStateChange();
    void accessibilitySummaryIncludesLabelValueRangeAndError();
};

void tst_DateField::construction()
{
    QtMaterialDateField field(QStringLiteral("Date"));
    QCOMPARE(field.labelText(), QStringLiteral("Date"));
    QVERIFY(field.isDateAcceptable());
}

void tst_DateField::dateRoundTrip()
{
    QtMaterialDateField field;
    const QDate date(2026, 4, 19);
    field.setDate(date);
    QCOMPARE(field.date(), date);
    QVERIFY(field.isDateAcceptable());
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

    QVERIFY(field.hasParseError());
    QVERIFY(!field.isDateAcceptable());
    QVERIFY(spy.count() >= 1);
}

void tst_DateField::displayFormatRoundTrip()
{
    QtMaterialDateField field;
    QSignalSpy spy(&field, &QtMaterialDateField::displayFormatChanged);
    QVERIFY(spy.isValid());

    field.setDisplayFormat(QStringLiteral("dd/MM/yyyy"));
    QCOMPARE(field.displayFormat(), QStringLiteral("dd/MM/yyyy"));
    QCOMPARE(spy.count(), 1);

    field.setDate(QDate(2026, 5, 1));
    QVERIFY(field.lineEdit());
    QCOMPARE(field.lineEdit()->text(), QStringLiteral("01/05/2026"));
}

void tst_DateField::clearableRoundTrip()
{
    QtMaterialDateField field;
    QSignalSpy spy(&field, &QtMaterialDateField::clearableChanged);
    QVERIFY(spy.isValid());

    field.setClearable(true);
    QVERIFY(field.isClearable());
    QCOMPARE(spy.count(), 1);

    field.setClearable(true);
    QCOMPARE(spy.count(), 1);
}

void tst_DateField::rangeRejectsDateBeforeMinimum()
{
    QtMaterialDateField field;
    field.setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    field.setMinimumDate(QDate(2026, 1, 1));
    field.setDate(QDate(2025, 12, 31));

    QVERIFY(!field.isDateAcceptable());
    QVERIFY(field.hasParseError());
    QVERIFY(field.errorText().contains(QStringLiteral("2026-01-01")));
}

void tst_DateField::rangeRejectsDateAfterMaximum()
{
    QtMaterialDateField field;
    field.setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    field.setMaximumDate(QDate(2026, 12, 31));
    field.setDate(QDate(2027, 1, 1));

    QVERIFY(!field.isDateAcceptable());
    QVERIFY(field.hasParseError());
    QVERIFY(field.errorText().contains(QStringLiteral("2026-12-31")));
}

void tst_DateField::rangeAcceptsBoundaryDates()
{
    QtMaterialDateField field;
    field.setMinimumDate(QDate(2026, 1, 1));
    field.setMaximumDate(QDate(2026, 12, 31));

    field.setDate(QDate(2026, 1, 1));
    QVERIFY(field.isDateAcceptable());
    QVERIFY(!field.hasParseError());

    field.setDate(QDate(2026, 12, 31));
    QVERIFY(field.isDateAcceptable());
    QVERIFY(!field.hasParseError());
}

void tst_DateField::acceptableSignalChangesOnlyOnStateChange()
{
    QtMaterialDateField field;
    field.setMinimumDate(QDate(2026, 1, 1));
    QSignalSpy spy(&field, &QtMaterialDateField::dateAcceptableChanged);
    QVERIFY(spy.isValid());

    field.setDate(QDate(2025, 12, 31));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toBool(), false);

    field.setDate(QDate(2025, 11, 30));
    QCOMPARE(spy.count(), 0);

    field.setDate(QDate(2026, 1, 1));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toBool(), true);
}

void tst_DateField::accessibilitySummaryIncludesLabelValueRangeAndError()
{
    QtMaterialDateField field(QStringLiteral("Start date"));
    field.setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    field.setMinimumDate(QDate(2026, 1, 1));
    field.setMaximumDate(QDate(2026, 12, 31));
    field.setDate(QDate(2025, 12, 31));

    const QString summary = field.accessibilitySummary();
    QVERIFY(summary.contains(QStringLiteral("Start date")));
    QVERIFY(summary.contains(QStringLiteral("2025-12-31")));
    QVERIFY(summary.contains(QStringLiteral("2026-01-01")));
    QVERIFY(field.accessibleDescription().contains(QStringLiteral("2026-01-01")));
}

QTEST_MAIN(tst_DateField)
#include "tst_datefield.moc"
