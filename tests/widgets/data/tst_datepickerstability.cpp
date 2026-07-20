#include <QtTest>

#include <QCalendarWidget>
#include <QLabel>
#include <QLayout>
#include <qtextformat.h>

#include "qtmaterial/widgets/qtmaterialdatepicker.h"

using namespace QtMaterial;

class tst_DatePickerStability : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void programmaticSelectionEmitsOnce();
    void selectedDateFormattingMovesWithSelection();
    void resolvedMetricsReachChildWidgets();
};

void tst_DatePickerStability::programmaticSelectionEmitsOnce()
{
    QtMaterialDatePicker picker;
    const QDate next = picker.selectedDate().addDays(1);

    QSignalSpy spy(
        &picker,
        &QtMaterialDatePicker::selectedDateChanged);
    picker.setSelectedDate(next);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(picker.selectedDate(), next);
}

void tst_DatePickerStability::selectedDateFormattingMovesWithSelection()
{
    QtMaterialDatePicker picker;
    auto* calendar = picker.findChild<QCalendarWidget*>();
    QVERIFY(calendar);

    DatePickerSpec spec = picker.spec();
    spec.selectedDateColor = QColor(QStringLiteral("#123456"));
    picker.setSpec(spec);

    const QDate previous = picker.selectedDate();
    const QDate next = previous.addDays(1);
    picker.setSelectedDate(next);

    QCOMPARE(
        calendar->dateTextFormat(next).background().color(),
        QColor(QStringLiteral("#123456")));
    QVERIFY(
        calendar->dateTextFormat(previous).background().color()
        != QColor(QStringLiteral("#123456")));
}

void tst_DatePickerStability::resolvedMetricsReachChildWidgets()
{
    QtMaterialDatePicker picker;

    DatePickerSpec spec = picker.spec();
    spec.cellSize = 48;
    spec.headerHeight = 72;
    spec.contentSpacing = 14;
    picker.setSpec(spec);

    auto* calendar = picker.findChild<QCalendarWidget*>();
    QVERIFY(calendar);
    QVERIFY(calendar->minimumWidth() >= 48 * 7);
    QVERIFY(calendar->minimumHeight() >= 48 * 7);

    auto* header = picker.findChild<QLabel*>(
        QStringLiteral("QtMaterialDatePickerHeader"));
    QVERIFY(header);
    QVERIFY(header->minimumHeight() >= 72);
    QCOMPARE(picker.layout()->spacing(), 14);
}

QTEST_MAIN(tst_DatePickerStability)
#include "tst_datepickerstability.moc"
