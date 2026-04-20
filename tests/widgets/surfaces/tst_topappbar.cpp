#include <QtTest/QtTest>

#include "qtmaterial/widgets/surfaces/qtmaterialtopappbar.h"

class tst_QtMaterialTopAppBar : public QObject
{
    Q_OBJECT

private slots:
    void construct();
    void apiRoundTrip();
    void keyboardNavigationTrigger();
};

void tst_QtMaterialTopAppBar::construct()
{
    QtMaterialTopAppBar bar;
    QVERIFY(bar.sizeHint().height() >= 56);
}

void tst_QtMaterialTopAppBar::apiRoundTrip()
{
    QtMaterialTopAppBar bar;
    bar.setTitle(QStringLiteral("Inbox"));
    QCOMPARE(bar.title(), QStringLiteral("Inbox"));

    bar.setCenteredTitle(true);
    QVERIFY(bar.centeredTitle());

    bar.setElevated(true);
    QVERIFY(bar.elevated());

    const int idx = bar.addActionButton(QIcon(), QStringLiteral("Search"));
    QCOMPARE(idx, 0);
    QCOMPARE(bar.actionCount(), 1);
}

void tst_QtMaterialTopAppBar::keyboardNavigationTrigger()
{
    QtMaterialTopAppBar bar(QStringLiteral("Inbox"));
    QSignalSpy spy(&bar, &QtMaterialTopAppBar::navigationTriggered);
    bar.setNavigationIcon(QIcon());
    // Only verifies non-crash path with no navigation icon.
    QTest::keyClick(&bar, Qt::Key_Return);
    QCOMPARE(spy.count(), 0);
}

QTEST_MAIN(tst_QtMaterialTopAppBar)
#include "tst_topappbar.moc"
