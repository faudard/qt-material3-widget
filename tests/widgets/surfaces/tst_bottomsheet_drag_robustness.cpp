#include <QSignalSpy>
#include <QtTest/QtTest>

#include "qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h"

using namespace QtMaterial;

namespace {

void openSheet(QWidget& host, QtMaterialBottomSheet& sheet)
{
    host.resize(800, 600);
    host.show();
    QTRY_VERIFY_WITH_TIMEOUT(host.isVisible(), 1000);
    sheet.open();
    QTRY_COMPARE(
        sheet.state(),
        QtMaterialBottomSheet::SheetState::Open);
}

QPoint pointInsideSheet(
    const QtMaterialBottomSheet& sheet,
    int distanceFromBottom)
{
    return QPoint(
        sheet.width() / 2,
        qMax(0, sheet.height() - distanceFromBottom));
}

void drag(
    QtMaterialBottomSheet& sheet,
    const QPoint& start,
    const QPoint& end,
    int steps = 5)
{
    QTest::mousePress(
        &sheet,
        Qt::LeftButton,
        Qt::NoModifier,
        start);

    for (int step = 1; step <= steps; ++step) {
        const qreal p = qreal(step) / qreal(steps);
        const QPoint position(
            qRound(start.x() + (end.x() - start.x()) * p),
            qRound(start.y() + (end.y() - start.y()) * p));
        QTest::mouseMove(&sheet, position, 12);
    }

    QTest::mouseRelease(
        &sheet,
        Qt::LeftButton,
        Qt::NoModifier,
        end);
}

} // namespace

class tst_BottomSheetDragRobustness : public QObject
{
    Q_OBJECT

private slots:
    void smallDownDragSnapsBack();
    void expandedDownDragCollapsesOnce();
    void collapsedUpDragExpandsOnce();
    void collapsedDownDragDismisses();
    void disabledDragIsIgnored();
    void dragDuringOpeningIsIgnored();
    void closingCancelsActiveDrag();
};

void tst_BottomSheetDragRobustness::smallDownDragSnapsBack()
{
    QWidget host;
    QtMaterialBottomSheet sheet(&host);
    sheet.setExpandedHeight(360);
    sheet.setCollapsedHeight(96);
    openSheet(host, sheet);

    QSignalSpy expandedSpy(
        &sheet,
        &QtMaterialBottomSheet::expandedChanged);
    QSignalSpy dismissedSpy(
        &sheet,
        &QtMaterialBottomSheet::dismissed);

    const QPoint start = pointInsideSheet(sheet, 280);
    drag(sheet, start, start + QPoint(0, 18), 3);

    QVERIFY(sheet.isExpanded());
    QCOMPARE(
        sheet.state(),
        QtMaterialBottomSheet::SheetState::Open);
    QCOMPARE(expandedSpy.count(), 0);
    QCOMPARE(dismissedSpy.count(), 0);
}

void tst_BottomSheetDragRobustness::expandedDownDragCollapsesOnce()
{
    QWidget host;
    QtMaterialBottomSheet sheet(&host);
    sheet.setExpandedHeight(360);
    sheet.setCollapsedHeight(96);
    openSheet(host, sheet);

    QSignalSpy expandedSpy(
        &sheet,
        &QtMaterialBottomSheet::expandedChanged);
    QSignalSpy dismissedSpy(
        &sheet,
        &QtMaterialBottomSheet::dismissed);

    const QPoint start = pointInsideSheet(sheet, 280);
    drag(sheet, start, start + QPoint(0, 120), 6);

    QVERIFY(!sheet.isExpanded());
    QCOMPARE(
        sheet.state(),
        QtMaterialBottomSheet::SheetState::Open);
    QCOMPARE(expandedSpy.count(), 1);
    QCOMPARE(dismissedSpy.count(), 0);
}

void tst_BottomSheetDragRobustness::collapsedUpDragExpandsOnce()
{
    QWidget host;
    QtMaterialBottomSheet sheet(&host);
    sheet.setExpandedHeight(360);
    sheet.setCollapsedHeight(96);
    sheet.setExpanded(false);
    openSheet(host, sheet);

    QSignalSpy expandedSpy(
        &sheet,
        &QtMaterialBottomSheet::expandedChanged);

    const QPoint start = pointInsideSheet(sheet, 48);
    drag(sheet, start, start - QPoint(0, 120), 6);

    QVERIFY(sheet.isExpanded());
    QCOMPARE(expandedSpy.count(), 1);
    QCOMPARE(
        sheet.state(),
        QtMaterialBottomSheet::SheetState::Open);
}

void tst_BottomSheetDragRobustness::collapsedDownDragDismisses()
{
    QWidget host;
    QtMaterialBottomSheet sheet(&host);
    sheet.setExpandedHeight(360);
    sheet.setCollapsedHeight(96);
    sheet.setExpanded(false);
    openSheet(host, sheet);

    QSignalSpy dismissedSpy(
        &sheet,
        &QtMaterialBottomSheet::dismissed);
    QSignalSpy closedSpy(
        &sheet,
        &QtMaterialBottomSheet::closed);

    const QPoint start = pointInsideSheet(sheet, 48);
    drag(sheet, start, start + QPoint(0, 80), 5);

    QTRY_COMPARE(
        sheet.state(),
        QtMaterialBottomSheet::SheetState::Closed);
    QCOMPARE(dismissedSpy.count(), 1);
    QCOMPARE(closedSpy.count(), 1);
}

void tst_BottomSheetDragRobustness::disabledDragIsIgnored()
{
    QWidget host;
    QtMaterialBottomSheet sheet(&host);
    sheet.setExpandedHeight(360);
    sheet.setCollapsedHeight(96);
    sheet.setDragToDismissEnabled(false);
    openSheet(host, sheet);

    QSignalSpy expandedSpy(
        &sheet,
        &QtMaterialBottomSheet::expandedChanged);
    QSignalSpy dismissedSpy(
        &sheet,
        &QtMaterialBottomSheet::dismissed);

    const QPoint start = pointInsideSheet(sheet, 280);
    drag(sheet, start, start + QPoint(0, 180), 6);

    QVERIFY(sheet.isExpanded());
    QCOMPARE(
        sheet.state(),
        QtMaterialBottomSheet::SheetState::Open);
    QCOMPARE(expandedSpy.count(), 0);
    QCOMPARE(dismissedSpy.count(), 0);
}

void tst_BottomSheetDragRobustness::dragDuringOpeningIsIgnored()
{
    QWidget host;
    host.resize(800, 600);
    host.show();
    QTRY_VERIFY_WITH_TIMEOUT(host.isVisible(), 1000);

    QtMaterialBottomSheet sheet(&host);
    sheet.setExpandedHeight(360);
    sheet.setCollapsedHeight(96);
    sheet.open();

    QCOMPARE(
        sheet.state(),
        QtMaterialBottomSheet::SheetState::Opening);

    QSignalSpy expandedSpy(
        &sheet,
        &QtMaterialBottomSheet::expandedChanged);

    const QPoint start = pointInsideSheet(sheet, 280);
    drag(sheet, start, start + QPoint(0, 150), 3);

    QVERIFY(sheet.isExpanded());
    QCOMPARE(expandedSpy.count(), 0);
    QTRY_COMPARE(
        sheet.state(),
        QtMaterialBottomSheet::SheetState::Open);
}

void tst_BottomSheetDragRobustness::closingCancelsActiveDrag()
{
    QWidget host;
    QtMaterialBottomSheet sheet(&host);
    sheet.setExpandedHeight(360);
    sheet.setCollapsedHeight(96);
    openSheet(host, sheet);

    const QPoint start = pointInsideSheet(sheet, 280);
    QTest::mousePress(
        &sheet,
        Qt::LeftButton,
        Qt::NoModifier,
        start);
    QTest::mouseMove(
        &sheet,
        start + QPoint(0, 80),
        12);

    sheet.close();

    QTRY_COMPARE(
        sheet.state(),
        QtMaterialBottomSheet::SheetState::Closed);
    QVERIFY(!sheet.isVisible());
}

QTEST_MAIN(tst_BottomSheetDragRobustness)

#include "tst_bottomsheet_drag_robustness.moc"
