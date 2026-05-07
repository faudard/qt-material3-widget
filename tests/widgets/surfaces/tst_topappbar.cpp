#include <QSignalSpy>
#include <QTest>
#include <QToolButton>
#include <QPixmap>

#include "qtmaterial/widgets/surfaces/qtmaterialtopappbar.h"

namespace {
QIcon testIcon()
{
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::black);
    return QIcon(pixmap);
}
}

class tst_QtMaterialTopAppBar : public QObject
{
    Q_OBJECT

private slots:
    void construct();
    void apiRoundTrip();
    void accessibilitySummary();
    void navigationButtonAccessibility();
    void keyboardNavigationTrigger();
    void actionAccessibility();
};

void tst_QtMaterialTopAppBar::construct()
{
    QtMaterialTopAppBar bar;
    QVERIFY(bar.sizeHint().height() >= 56);
    QVERIFY(bar.minimumSizeHint().width() > 0);
    QCOMPARE(bar.accessibleName(), QStringLiteral("Top app bar"));
}

void tst_QtMaterialTopAppBar::apiRoundTrip()
{
    QtMaterialTopAppBar bar;
    QSignalSpy titleSpy(&bar, &QtMaterialTopAppBar::titleChanged);
    bar.setTitle(QStringLiteral("Inbox"));
    QCOMPARE(bar.title(), QStringLiteral("Inbox"));
    QCOMPARE(titleSpy.count(), 1);

    bar.setCenteredTitle(true);
    QVERIFY(bar.centeredTitle());

    bar.setElevated(true);
    QVERIFY(bar.elevated());

    bar.setNavigationAccessibleName(QStringLiteral("Open navigation drawer"));
    QCOMPARE(bar.navigationAccessibleName(), QStringLiteral("Open navigation drawer"));

    const int idx = bar.addActionButton(testIcon(), QStringLiteral("Search"));
    QCOMPARE(idx, 0);
    QCOMPARE(bar.actionCount(), 1);
    QCOMPARE(bar.actionAccessibleName(0), QStringLiteral("Search"));

    bar.setActionAccessibleName(0, QStringLiteral("Search mail"));
    QCOMPARE(bar.actionAccessibleName(0), QStringLiteral("Search mail"));

    bar.clearActionButtons();
    QCOMPARE(bar.actionCount(), 0);
}

void tst_QtMaterialTopAppBar::accessibilitySummary()
{
    QtMaterialTopAppBar bar(QStringLiteral("Inbox"));
    QCOMPARE(bar.accessibleName(), QStringLiteral("Inbox"));
    QVERIFY(bar.accessibilitySummary().contains(QStringLiteral("Inbox")));

    QSignalSpy spy(&bar, &QtMaterialTopAppBar::accessibilitySummaryChanged);
    bar.addActionButton(testIcon(), QStringLiteral("Search"));
    QVERIFY(bar.accessibilitySummary().contains(QStringLiteral("1 action")));
    QVERIFY(spy.count() >= 1);
}

void tst_QtMaterialTopAppBar::navigationButtonAccessibility()
{
    QtMaterialTopAppBar bar(QStringLiteral("Inbox"));
    bar.resize(360, 64);
    bar.setNavigationAccessibleName(QStringLiteral("Open menu"));
    bar.setNavigationIcon(testIcon());
    bar.show();
    QVERIFY(QTest::qWaitForWindowExposed(&bar));

    auto* button = bar.findChild<QToolButton*>(QStringLiteral("qtmaterial_topAppBar_navigationButton"));
    QVERIFY(button);
    QVERIFY(button->isVisible());
    QCOMPARE(button->accessibleName(), QStringLiteral("Open menu"));
}

void tst_QtMaterialTopAppBar::keyboardNavigationTrigger()
{
    QtMaterialTopAppBar bar(QStringLiteral("Inbox"));
    bar.setNavigationIcon(testIcon());
    bar.show();
    QVERIFY(QTest::qWaitForWindowExposed(&bar));

    QSignalSpy spy(&bar, &QtMaterialTopAppBar::navigationTriggered);
    QTest::keyClick(&bar, Qt::Key_Return);
    QCOMPARE(spy.count(), 1);
    QTest::keyClick(&bar, Qt::Key_Space);
    QCOMPARE(spy.count(), 2);
}

void tst_QtMaterialTopAppBar::actionAccessibility()
{
    QtMaterialTopAppBar bar(QStringLiteral("Inbox"));
    const int index = bar.addActionButton(testIcon(), QStringLiteral("Search"));
    QCOMPARE(bar.actionAccessibleText(index), QStringLiteral("Search, top app bar action"));

    bar.resize(360, 64);
    bar.show();
    QVERIFY(QTest::qWaitForWindowExposed(&bar));
    auto* button = bar.findChild<QToolButton*>(QStringLiteral("qtmaterial_topAppBar_action_0"));
    QVERIFY(button);
    QCOMPARE(button->accessibleName(), QStringLiteral("Search"));
}

QTEST_MAIN(tst_QtMaterialTopAppBar)
#include "tst_topappbar.moc"
