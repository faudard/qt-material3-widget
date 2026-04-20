#include <QtTest/QtTest>

#include <QPushButton>

#include "qtmaterial/widgets/surfaces/qtmaterialbottomappbar.h"

class tst_QtMaterialBottomAppBar : public QObject
{
    Q_OBJECT

private slots:
    void construct();
    void apiRoundTrip();
    void keyboardNavigationTrigger();
};

void tst_QtMaterialBottomAppBar::construct()
{
    QtMaterialBottomAppBar bar;
    QVERIFY(bar.sizeHint().height() >= 64);
}

void tst_QtMaterialBottomAppBar::apiRoundTrip()
{
    QtMaterialBottomAppBar bar;
    bar.setTitle(QStringLiteral("Home"));
    QCOMPARE(bar.title(), QStringLiteral("Home"));

    bar.setElevated(true);
    QVERIFY(bar.elevated());

    bar.setFabAttached(true);
    QVERIFY(bar.fabAttached());

    auto *fab = new QPushButton(QStringLiteral("+"));
    bar.setFabButton(fab);
    QCOMPARE(bar.fabButton(), fab);

    const int idx = bar.addActionButton(QIcon(), QStringLiteral("Search"));
    QCOMPARE(idx, 0);
    QCOMPARE(bar.actionCount(), 1);
}

void tst_QtMaterialBottomAppBar::keyboardNavigationTrigger()
{
    QtMaterialBottomAppBar bar(QStringLiteral("Home"));
    QSignalSpy spy(&bar, &QtMaterialBottomAppBar::navigationTriggered);
    bar.setNavigationIcon(QIcon());
    QTest::keyClick(&bar, Qt::Key_Return);
    QCOMPARE(spy.count(), 0);
}

QTEST_MAIN(tst_QtMaterialBottomAppBar)
#include "tst_bottomappbar.moc"
