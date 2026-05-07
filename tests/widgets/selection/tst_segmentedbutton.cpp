#include <QtTest/QtTest>

#include "qtmaterial/widgets/selection/qtmaterialsegmentedbutton.h"

using namespace QtMaterial;

class tst_SegmentedButton : public QObject
{
    Q_OBJECT

private slots:
    void constructionAndSelection();
    void disabledSegmentsAreSkippedByKeyboard();
    void homeAndEndSelectFirstAndLastEnabledSegments();
    void multiSelectionTogglesCurrentSegment();
    void accessibilitySummaryReflectsState();
};

void tst_SegmentedButton::constructionAndSelection()
{
    QtMaterialSegmentedButton button;
    button.addSegment(QStringLiteral("Day"));
    button.addSegment(QStringLiteral("Week"));
    button.addSegment(QStringLiteral("Month"));

    QCOMPARE(button.count(), 3);
    QCOMPARE(button.currentIndex(), -1);

    QSignalSpy currentSpy(&button, &QtMaterialSegmentedButton::currentIndexChanged);
    button.setCurrentIndex(1);

    QCOMPARE(button.currentIndex(), 1);
    QVERIFY(button.isSegmentChecked(1));
    QCOMPARE(currentSpy.count(), 1);
}

void tst_SegmentedButton::disabledSegmentsAreSkippedByKeyboard()
{
    QtMaterialSegmentedButton button;
    button.addSegment(QStringLiteral("One"));
    button.addSegment(QStringLiteral("Two"));
    button.addSegment(QStringLiteral("Three"));
    button.setCurrentIndex(0);
    button.setSegmentEnabled(1, false);

    button.show();
    QVERIFY(QTest::qWaitForWindowExposed(&button));
    button.setFocus();

    QTest::keyClick(&button, Qt::Key_Right);
    QCOMPARE(button.currentIndex(), 2);

    QTest::keyClick(&button, Qt::Key_Left);
    QCOMPARE(button.currentIndex(), 0);
}

void tst_SegmentedButton::homeAndEndSelectFirstAndLastEnabledSegments()
{
    QtMaterialSegmentedButton button;
    button.addSegment(QStringLiteral("One"));
    button.addSegment(QStringLiteral("Two"));
    button.addSegment(QStringLiteral("Three"));
    button.setSegmentEnabled(0, false);
    button.setSegmentEnabled(2, false);

    button.show();
    QVERIFY(QTest::qWaitForWindowExposed(&button));
    button.setFocus();

    QTest::keyClick(&button, Qt::Key_Home);
    QCOMPARE(button.currentIndex(), 1);

    QTest::keyClick(&button, Qt::Key_End);
    QCOMPARE(button.currentIndex(), 1);
}

void tst_SegmentedButton::multiSelectionTogglesCurrentSegment()
{
    QtMaterialSegmentedButton button;
    button.addSegment(QStringLiteral("A"));
    button.addSegment(QStringLiteral("B"));
    button.setMultiSelection(true);
    button.setCurrentIndex(0);

    QSignalSpy toggledSpy(&button, &QtMaterialSegmentedButton::segmentToggled);

    button.show();
    QVERIFY(QTest::qWaitForWindowExposed(&button));
    button.setFocus();

    QTest::keyClick(&button, Qt::Key_Space);

    QCOMPARE(toggledSpy.count(), 1);
    QVERIFY(button.isSegmentChecked(0));
}

void tst_SegmentedButton::accessibilitySummaryReflectsState()
{
    QtMaterialSegmentedButton button;
    button.addSegment(QStringLiteral("Grid"));
    button.addSegment(QStringLiteral("List"));
    button.setCurrentIndex(1);
    button.setSegmentEnabled(0, false);

    const QString summary = button.accessibilitySummary();
    QVERIFY(summary.contains(QStringLiteral("2")));
    QVERIFY(summary.contains(QStringLiteral("List")));
    QVERIFY(button.segmentAccessibleText(0).contains(QStringLiteral("disabled")));
    QVERIFY(button.currentSegmentAccessibleText().contains(QStringLiteral("selected")));
}

QTEST_MAIN(tst_SegmentedButton)
#include "tst_segmentedbutton.moc"
