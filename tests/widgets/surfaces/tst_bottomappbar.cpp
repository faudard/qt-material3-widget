#include <QPushButton>
#include <QSignalSpy>
#include <QTest>
#include <QToolButton>
#include <QPixmap>

#include "qtmaterial/widgets/surfaces/qtmaterialbottomappbar.h"

namespace {
QIcon testIcon()
{
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::black);
    return QIcon(pixmap);
}
}

class tst_QtMaterialBottomAppBar : public QObject
{
    Q_OBJECT

private slots:
    void construct();
    void apiRoundTrip();
    void accessibilitySummary();
    void navigationButtonAccessibility();
    void keyboardNavigationTrigger();
    void fabAccessibility();
    void actionAccessibility();
};

void tst_QtMaterialBottomAppBar::construct()
{
    QtMaterialBottomAppBar bar;
    QVERIFY(bar.sizeHint().height() >= 64);
    QVERIFY(bar.minimumSizeHint().width() > 0);
    QCOMPARE(bar.accessibleName(), QStringLiteral("Bottom app bar"));
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

    auto* fab = new QPushButton(QStringLiteral("+"));
    bar.setFabButton(fab);
    QCOMPARE(bar.fabButton(), fab);

    bar.setNavigationAccessibleName(QStringLiteral("Open navigation"));
    QCOMPARE(bar.navigationAccessibleName(), QStringLiteral("Open navigation"));

    const int idx = bar.addActionButton(testIcon(), QStringLiteral("Search"));
    QCOMPARE(idx, 0);
    QCOMPARE(bar.actionCount(), 1);
    QCOMPARE(bar.actionAccessibleName(0), QStringLiteral("Search"));
}

void tst_QtMaterialBottomAppBar::accessibilitySummary()
{
    QtMaterialBottomAppBar bar(QStringLiteral("Home"));
    QCOMPARE(bar.accessibleName(), QStringLiteral("Home"));
    QVERIFY(bar.accessibilitySummary().contains(QStringLiteral("Home")));

    QSignalSpy spy(&bar, &QtMaterialBottomAppBar::accessibilitySummaryChanged);
    bar.addActionButton(testIcon(), QStringLiteral("Search"));
    QVERIFY(bar.accessibilitySummary().contains(QStringLiteral("1 action")));
    QVERIFY(spy.count() >= 1);
}

void tst_QtMaterialBottomAppBar::navigationButtonAccessibility()
{
    QtMaterialBottomAppBar bar(QStringLiteral("Home"));
    bar.resize(360, 80);
    bar.setNavigationAccessibleName(QStringLiteral("Open menu"));
    bar.setNavigationIcon(testIcon());
    bar.show();
    QVERIFY(QTest::qWaitForWindowExposed(&bar));

    auto* button = bar.findChild<QToolButton*>(QStringLiteral("qtmaterial_bottomAppBar_navigationButton"));
    QVERIFY(button);
    QVERIFY(button->isVisible());
    QCOMPARE(button->accessibleName(), QStringLiteral("Open menu"));
}

void tst_QtMaterialBottomAppBar::keyboardNavigationTrigger()
{
    QtMaterialBottomAppBar bar(QStringLiteral("Home"));
    bar.setNavigationIcon(testIcon());
    bar.show();
    QVERIFY(QTest::qWaitForWindowExposed(&bar));

    QSignalSpy spy(&bar, &QtMaterialBottomAppBar::navigationTriggered);
    QTest::keyClick(&bar, Qt::Key_Return);
    QCOMPARE(spy.count(), 1);
    QTest::keyClick(&bar, Qt::Key_Space);
    QCOMPARE(spy.count(), 2);
}

void tst_QtMaterialBottomAppBar::fabAccessibility()
{
    QtMaterialBottomAppBar bar(QStringLiteral("Home"));
    auto* fab = new QPushButton(QStringLiteral("+"));
    bar.setFabButton(fab);
    QCOMPARE(bar.fabAccessibleText(), QStringLiteral("Primary action"));
    QCOMPARE(fab->accessibleDescription(), QStringLiteral("Bottom app bar floating action button"));
}

void tst_QtMaterialBottomAppBar::actionAccessibility()
{
    QtMaterialBottomAppBar bar(QStringLiteral("Home"));
    const int index = bar.addActionButton(testIcon(), QStringLiteral("Search"));
    QCOMPARE(bar.actionAccessibleText(index), QStringLiteral("Search, bottom app bar action"));

    bar.resize(360, 80);
    bar.show();
    QVERIFY(QTest::qWaitForWindowExposed(&bar));
    auto* button = bar.findChild<QToolButton*>(QStringLiteral("qtmaterial_bottomAppBar_action_0"));
    QVERIFY(button);
    QCOMPARE(button->accessibleName(), QStringLiteral("Search"));
}

QTEST_MAIN(tst_QtMaterialBottomAppBar)
#include "tst_bottomappbar.moc"
