#include <QtTest/QtTest>

#include <QCalendarWidget>
#include <QDialog>
#include <QImage>
#include <QLayout>
#include <QLineEdit>
#include <QListView>
#include <QPainter>
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QToolButton>

#include "qtmaterial/widgets/inputs/qtmaterialautocomplete.h"
#include "qtmaterial/widgets/inputs/qtmaterialcombobox.h"
#include "qtmaterial/widgets/inputs/qtmaterialdatefield.h"
#include "qtmaterial/widgets/inputs/qtmaterialdaterangepicker.h"
#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialsearchbar.h"
#include "qtmaterial/widgets/inputs/qtmaterialsearchview.h"
#include "qtmaterial/widgets/inputs/qtmaterialtimefield.h"
#include "qtmaterial/widgets/inputs/qtmaterialtimepicker.h"
#include "qtmaterial/widgets/qtmaterialdatepicker.h"

using namespace QtMaterial;

namespace {

QImage renderAtDpr2(QWidget& widget, const QSize& minimumLogicalSize)
{
    widget.ensurePolished();

    QSize logicalSize = widget.sizeHint();
    if (!logicalSize.isValid() || logicalSize.isEmpty()) {
        logicalSize = minimumLogicalSize;
    }
    logicalSize = logicalSize.expandedTo(minimumLogicalSize);
    logicalSize = logicalSize.boundedTo(QSize(760, 520));

    widget.resize(logicalSize);
    if (widget.layout()) {
        widget.layout()->activate();
    }

    QImage image(
        logicalSize.width() * 2,
        logicalSize.height() * 2,
        QImage::Format_ARGB32_Premultiplied);
    image.setDevicePixelRatio(2.0);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    widget.render(&painter);
    painter.end();

    return image;
}

void verifyDpr2(QWidget& widget, const QSize& minimumLogicalSize)
{
    const QImage image = renderAtDpr2(widget, minimumLogicalSize);
    QVERIFY(!image.isNull());
    QCOMPARE(image.devicePixelRatio(), qreal(2.0));
}

} // namespace

class tst_InputMaturity : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void accessibilityContracts();
    void keyboardContracts();
    void rtlPropagatesToNativeChildren();
    void dpr2RenderingSmoke();
};

void tst_InputMaturity::accessibilityContracts()
{
    QtMaterialOutlinedTextField outlined;
    outlined.setLabelText(QStringLiteral("Email"));
    QCOMPARE(outlined.accessibleName(), QStringLiteral("Email"));
    QVERIFY(outlined.lineEdit());
    QCOMPARE(outlined.lineEdit()->accessibleName(), QStringLiteral("Email"));

    QtMaterialFilledTextField filled;
    filled.setLabelText(QStringLiteral("Display name"));
    QCOMPARE(filled.accessibleName(), QStringLiteral("Display name"));
    QVERIFY(filled.lineEdit());
    QCOMPARE(
        filled.lineEdit()->accessibleName(),
        QStringLiteral("Display name"));

    QtMaterialComboBox combo;
    combo.setEditable(true);
    combo.setLabelText(QStringLiteral("Country"));
    QCOMPARE(combo.accessibleName(), QStringLiteral("Country"));
    QVERIFY(combo.lineEdit());
    QCOMPARE(combo.lineEdit()->accessibleName(), QStringLiteral("Country"));

    combo.lineEdit()->setAccessibleName(QStringLiteral("Custom country editor"));
    combo.setLabelText(QStringLiteral("Destination"));
    QCOMPARE(
        combo.lineEdit()->accessibleName(),
        QStringLiteral("Custom country editor"));

    QtMaterialAutocomplete autocomplete;
    QCOMPARE(
        autocomplete.accessibleName(),
        QStringLiteral("Autocomplete"));
    autocomplete.setPlaceholderText(QStringLiteral("Project"));
    QCOMPARE(autocomplete.accessibleName(), QStringLiteral("Project"));
    QCOMPARE(
        autocomplete.lineEdit()->accessibleName(),
        QStringLiteral("Project"));

    autocomplete.setAccessibleName(QStringLiteral("Custom project search"));
    autocomplete.setPlaceholderText(QStringLiteral("Another placeholder"));
    QCOMPARE(
        autocomplete.accessibleName(),
        QStringLiteral("Custom project search"));
    QCOMPARE(
        autocomplete.lineEdit()->accessibleName(),
        QStringLiteral("Custom project search"));

    QtMaterialSearchBar searchBar;
    QCOMPARE(searchBar.accessibleName(), QStringLiteral("Search"));
    QCOMPARE(
        searchBar.lineEdit()->accessibleName(),
        QStringLiteral("Search query"));
    auto* clearButton =
        searchBar.findChild<QToolButton*>();
    QVERIFY(clearButton);
    QCOMPARE(
        clearButton->accessibleName(),
        QStringLiteral("Clear search"));

    QtMaterialSearchView searchView;
    QCOMPARE(
        searchView.accessibleName(),
        QStringLiteral("Search results"));
    QCOMPARE(
        searchView.resultView()->accessibleName(),
        QStringLiteral("Search results list"));

    QtMaterialDateField dateField;
    dateField.setLabelText(QStringLiteral("Due date"));
    dateField.setDate(QDate(2026, 10, 2));
    QCOMPARE(
        dateField.accessibleName(),
        QStringLiteral("Due date"));
    QVERIFY(
        dateField.accessibilitySummary().contains(
            QStringLiteral("Due date")));

    QtMaterialDatePicker datePicker;
    QVERIFY(
        datePicker.accessibleName().contains(
            QStringLiteral("Date picker")));
    auto* calendar =
        datePicker.findChild<QCalendarWidget*>(
            QStringLiteral("QtMaterialDatePickerCalendar"));
    QVERIFY(calendar);
    QCOMPARE(
        calendar->accessibleName(),
        QStringLiteral("Calendar"));

    QtMaterialDateRangePicker dateRange;
    QCOMPARE(
        dateRange.accessibleName(),
        QStringLiteral("Date range"));
    QCOMPARE(
        dateRange.startPicker()->accessibleDescription(),
        QStringLiteral("Start date"));
    QCOMPARE(
        dateRange.endPicker()->accessibleDescription(),
        QStringLiteral("End date"));

    QtMaterialTimeField timeField;
    QCOMPARE(timeField.accessibleName(), QStringLiteral("Time"));
    QCOMPARE(timeField.focusPolicy(), Qt::StrongFocus);

    QtMaterialTimePicker timePicker;
    QCOMPARE(
        timePicker.accessibleName(),
        QStringLiteral("Time picker"));
    QVERIFY(timePicker.timeField());
    QCOMPARE(
        timePicker.timeField()->accessibleName(),
        QStringLiteral("Time"));
}

void tst_InputMaturity::keyboardContracts()
{
    QtMaterialOutlinedTextField outlined;
    outlined.setLabelText(QStringLiteral("Name"));
    QVERIFY(outlined.lineEdit());
    QTest::keyClicks(outlined.lineEdit(), QStringLiteral("Ada"));
    QCOMPARE(outlined.text(), QStringLiteral("Ada"));

    QtMaterialComboBox combo;
    combo.addItems({
        QStringLiteral("One"),
        QStringLiteral("Two"),
        QStringLiteral("Three")
    });
    combo.setCurrentIndex(0);
    QTest::keyClick(&combo, Qt::Key_Down);
    QCOMPARE(combo.currentIndex(), 1);

    QtMaterialSearchBar searchBar;
    QSignalSpy searchRequested(
        &searchBar,
        &QtMaterialSearchBar::searchRequested);
    searchBar.setText(QStringLiteral("material"));
    QTest::keyClick(searchBar.lineEdit(), Qt::Key_Return);
    QCOMPARE(searchRequested.count(), 1);

    QStandardItemModel sourceModel(3, 1);
    sourceModel.setData(
        sourceModel.index(0, 0),
        QStringLiteral("Alpha"));
    sourceModel.setData(
        sourceModel.index(1, 0),
        QStringLiteral("Beta"));
    sourceModel.setData(
        sourceModel.index(2, 0),
        QStringLiteral("Gamma"));

    QtMaterialSearchView searchView;
    searchView.setSourceModel(&sourceModel);
    QSignalSpy querySpy(
        &searchView,
        &QtMaterialSearchView::queryChanged);
    QTest::keyClicks(
        searchView.searchBar()->lineEdit(),
        QStringLiteral("Beta"));
    QVERIFY(querySpy.count() >= 1);
    QCOMPARE(searchView.resultView()->model()->rowCount(), 1);

    QtMaterialAutocomplete autocomplete;
    autocomplete.setSuggestions({
        QStringLiteral("Alpha"),
        QStringLiteral("Beta")
    });
    autocomplete.setText(QStringLiteral("A"));
    QVERIFY(
        autocomplete.accessibilitySummary().contains(
            QStringLiteral("A")));

    QtMaterialDateField dateField;
    dateField.setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    dateField.lineEdit()->setText(QStringLiteral("2026-10-02"));
    QVERIFY(QMetaObject::invokeMethod(
        dateField.lineEdit(),
        "editingFinished",
        Qt::DirectConnection));
    QCOMPARE(dateField.date(), QDate(2026, 10, 2));

    QtMaterialDatePicker datePicker;
    QSignalSpy activated(
        &datePicker,
        &QtMaterialDatePicker::activated);
    QTest::keyClick(&datePicker, Qt::Key_Return);
    QCOMPARE(activated.count(), 1);

    QtMaterialDateRangePicker dateRange;
    QSignalSpy startActivated(
        dateRange.startPicker(),
        &QtMaterialDatePicker::activated);
    QTest::keyClick(
        dateRange.startPicker(),
        Qt::Key_Return);
    QCOMPARE(startActivated.count(), 1);

    QtMaterialTimeField timeField;
    timeField.setTime(QTime(10, 15));
    timeField.setCurrentSection(
        QDateTimeEdit::MinuteSection);
    QTest::keyClick(&timeField, Qt::Key_Up);
    QCOMPARE(timeField.time(), QTime(10, 16));

    QtMaterialTimePicker timePicker;
    timePicker.setSelectedTime(QTime(8, 30));
    timePicker.timeField()->setCurrentSection(
        QDateTimeEdit::MinuteSection);
    QTest::keyClick(
        timePicker.timeField(),
        Qt::Key_Up);
    QCOMPARE(
        timePicker.selectedTime(),
        QTime(8, 31));
}

void tst_InputMaturity::rtlPropagatesToNativeChildren()
{
    QtMaterialOutlinedTextField outlined;
    outlined.setLayoutDirection(Qt::RightToLeft);
    QCOMPARE(
        outlined.lineEdit()->layoutDirection(),
        Qt::RightToLeft);

    QtMaterialFilledTextField filled;
    filled.setLayoutDirection(Qt::RightToLeft);
    QCOMPARE(
        filled.lineEdit()->layoutDirection(),
        Qt::RightToLeft);

    QtMaterialComboBox combo;
    combo.setEditable(true);
    combo.setLayoutDirection(Qt::RightToLeft);
    QCOMPARE(combo.layoutDirection(), Qt::RightToLeft);
    QCOMPARE(
        combo.lineEdit()->layoutDirection(),
        Qt::RightToLeft);

    QtMaterialAutocomplete autocomplete;
    autocomplete.setLayoutDirection(Qt::RightToLeft);
    QCOMPARE(
        autocomplete.lineEdit()->layoutDirection(),
        Qt::RightToLeft);

    QtMaterialSearchBar searchBar;
    searchBar.setLayoutDirection(Qt::RightToLeft);
    QCOMPARE(
        searchBar.lineEdit()->layoutDirection(),
        Qt::RightToLeft);

    QtMaterialSearchView searchView;
    searchView.setLayoutDirection(Qt::RightToLeft);
    QCOMPARE(
        searchView.searchBar()->layoutDirection(),
        Qt::RightToLeft);
    QCOMPARE(
        searchView.resultView()->layoutDirection(),
        Qt::RightToLeft);

    QtMaterialDateField dateField;
    dateField.setLayoutDirection(Qt::RightToLeft);
    QCOMPARE(
        dateField.lineEdit()->layoutDirection(),
        Qt::RightToLeft);

    QtMaterialDatePicker datePicker;
    datePicker.setLayoutDirection(Qt::RightToLeft);
    auto* calendar =
        datePicker.findChild<QCalendarWidget*>(
            QStringLiteral("QtMaterialDatePickerCalendar"));
    QVERIFY(calendar);
    QCOMPARE(
        calendar->layoutDirection(),
        Qt::RightToLeft);

    QtMaterialDateRangePicker dateRange;
    dateRange.setLayoutDirection(Qt::RightToLeft);
    QCOMPARE(
        dateRange.startPicker()->layoutDirection(),
        Qt::RightToLeft);
    QCOMPARE(
        dateRange.endPicker()->layoutDirection(),
        Qt::RightToLeft);

    QtMaterialTimeField timeField;
    timeField.setLayoutDirection(Qt::RightToLeft);
    QCOMPARE(
        timeField.layoutDirection(),
        Qt::RightToLeft);

    QtMaterialTimePicker timePicker;
    timePicker.setLayoutDirection(Qt::RightToLeft);
    QCOMPARE(
        timePicker.timeField()->layoutDirection(),
        Qt::RightToLeft);
}

void tst_InputMaturity::dpr2RenderingSmoke()
{
    QtMaterialOutlinedTextField outlined;
    outlined.setLabelText(QStringLiteral("Email"));
    outlined.setText(QStringLiteral("dev@example.com"));
    verifyDpr2(outlined, QSize(320, 88));

    QtMaterialFilledTextField filled;
    filled.setLabelText(QStringLiteral("Name"));
    filled.setText(QStringLiteral("Ada"));
    verifyDpr2(filled, QSize(320, 88));

    QtMaterialComboBox combo;
    combo.setLabelText(QStringLiteral("Country"));
    combo.addItems({
        QStringLiteral("France"),
        QStringLiteral("Germany")
    });
    verifyDpr2(combo, QSize(240, 48));

    QtMaterialAutocomplete autocomplete;
    autocomplete.setPlaceholderText(QStringLiteral("Project"));
    autocomplete.setText(QStringLiteral("Material"));
    verifyDpr2(autocomplete, QSize(280, 56));

    QtMaterialSearchBar searchBar;
    searchBar.setText(QStringLiteral("Material"));
    verifyDpr2(searchBar, QSize(280, 48));

    QtMaterialSearchView searchView;
    QStandardItemModel sourceModel(3, 1);
    sourceModel.setData(
        sourceModel.index(0, 0),
        QStringLiteral("Alpha"));
    searchView.setSourceModel(&sourceModel);
    verifyDpr2(searchView, QSize(360, 240));

    QtMaterialDateField dateField;
    dateField.setLabelText(QStringLiteral("Due date"));
    dateField.setDate(QDate(2026, 10, 2));
    verifyDpr2(dateField, QSize(320, 88));

    QtMaterialDatePicker datePicker;
    datePicker.setSelectedDate(QDate(2026, 10, 2));
    verifyDpr2(datePicker, QSize(360, 360));

    QtMaterialDateRangePicker dateRange;
    dateRange.setDateRange(
        QDate(2026, 10, 2),
        QDate(2026, 10, 9));
    verifyDpr2(dateRange, QSize(720, 420));

    QtMaterialTimeField timeField;
    timeField.setTime(QTime(10, 30));
    verifyDpr2(timeField, QSize(180, 48));

    QtMaterialTimePicker timePicker;
    timePicker.setSelectedTime(QTime(10, 30));
    verifyDpr2(timePicker, QSize(280, 160));
}

QTEST_MAIN(tst_InputMaturity)
#include "tst_input_maturity.moc"
