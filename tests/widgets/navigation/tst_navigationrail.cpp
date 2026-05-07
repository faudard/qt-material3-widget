#include <QtTest/QtTest>

#include "qtmaterial/widgets/navigation/qtmaterialnavigationrail.h"

using namespace QtMaterial;

class NavigationRailTest : public QObject {
    Q_OBJECT

private slots:
    void constructionAndDestinationModel();
    void currentIndexRejectsDisabledDestination();
    void keyboardNavigationSkipsDisabledDestinations();
    void activationUsesCurrentDestination();
    void homeAndEndSelectFirstAndLastEnabledDestination();
    void labelsVisibleEmitsOnlyOnChange();
    void accessibilitySummaryTracksSelectionAndEnabledState();
};

void NavigationRailTest::constructionAndDestinationModel()
{
    QtMaterialNavigationRail rail;

    QCOMPARE(rail.count(), 0);
    QCOMPARE(rail.currentIndex(), -1);
    QVERIFY(rail.labelsVisible());
    QCOMPARE(rail.accessibleName(), QStringLiteral("Navigation rail"));

    const int home = rail.addDestination(QStringLiteral("Home"));
    const int search = rail.addDestination(QStringLiteral("Search"));

    QCOMPARE(home, 0);
    QCOMPARE(search, 1);
    QCOMPARE(rail.count(), 2);
    QCOMPARE(rail.destinationText(0), QStringLiteral("Home"));
    QCOMPARE(rail.destinationText(1), QStringLiteral("Search"));
    QVERIFY(rail.isDestinationEnabled(0));
    QVERIFY(rail.isDestinationEnabled(1));
    QVERIFY(rail.sizeHint().width() >= rail.minimumSizeHint().width());
}

void NavigationRailTest::currentIndexRejectsDisabledDestination()
{
    QtMaterialNavigationRail rail;
    rail.addDestination(QStringLiteral("Home"));
    rail.addDestination(QStringLiteral("Settings"));

    QSignalSpy currentSpy(&rail, &QtMaterialNavigationRail::currentIndexChanged);
    QSignalSpy enabledSpy(&rail, &QtMaterialNavigationRail::destinationEnabledChanged);

    rail.setDestinationEnabled(1, false);
    QCOMPARE(enabledSpy.count(), 1);
    QCOMPARE(enabledSpy.takeFirst().at(0).toInt(), 1);

    rail.setCurrentIndex(1);
    QCOMPARE(rail.currentIndex(), -1);
    QCOMPARE(currentSpy.count(), 0);

    rail.setCurrentIndex(0);
    QCOMPARE(rail.currentIndex(), 0);
    QCOMPARE(currentSpy.count(), 1);
}

void NavigationRailTest::keyboardNavigationSkipsDisabledDestinations()
{
    QtMaterialNavigationRail rail;
    rail.addDestination(QStringLiteral("Home"));
    rail.addDestination(QStringLiteral("Search"));
    rail.addDestination(QStringLiteral("Settings"));
    rail.setDestinationEnabled(1, false);
    rail.setCurrentIndex(0);
    rail.resize(rail.sizeHint());
    rail.show();
    QVERIFY(QTest::qWaitForWindowExposed(&rail));

    QTest::keyClick(&rail, Qt::Key_Down);
    QCOMPARE(rail.currentIndex(), 2);

    QTest::keyClick(&rail, Qt::Key_Up);
    QCOMPARE(rail.currentIndex(), 0);

    QTest::keyClick(&rail, Qt::Key_Right);
    QCOMPARE(rail.currentIndex(), 2);

    QTest::keyClick(&rail, Qt::Key_Left);
    QCOMPARE(rail.currentIndex(), 0);
}

void NavigationRailTest::activationUsesCurrentDestination()
{
    QtMaterialNavigationRail rail;
    rail.addDestination(QStringLiteral("Home"));
    rail.addDestination(QStringLiteral("Settings"));
    rail.setCurrentIndex(1);
    rail.resize(rail.sizeHint());
    rail.show();
    QVERIFY(QTest::qWaitForWindowExposed(&rail));

    QSignalSpy activatedSpy(&rail, &QtMaterialNavigationRail::destinationActivated);

    QTest::keyClick(&rail, Qt::Key_Space);
    QCOMPARE(activatedSpy.count(), 1);
    QCOMPARE(activatedSpy.takeFirst().at(0).toInt(), 1);

    QTest::keyClick(&rail, Qt::Key_Return);
    QCOMPARE(activatedSpy.count(), 1);
    QCOMPARE(activatedSpy.takeFirst().at(0).toInt(), 1);
}

void NavigationRailTest::homeAndEndSelectFirstAndLastEnabledDestination()
{
    QtMaterialNavigationRail rail;
    rail.addDestination(QStringLiteral("Home"));
    rail.addDestination(QStringLiteral("Search"));
    rail.addDestination(QStringLiteral("Settings"));
    rail.setDestinationEnabled(0, false);
    rail.setCurrentIndex(1);
    rail.resize(rail.sizeHint());
    rail.show();
    QVERIFY(QTest::qWaitForWindowExposed(&rail));

    QTest::keyClick(&rail, Qt::Key_End);
    QCOMPARE(rail.currentIndex(), 2);

    QTest::keyClick(&rail, Qt::Key_Home);
    QCOMPARE(rail.currentIndex(), 1);
}

void NavigationRailTest::labelsVisibleEmitsOnlyOnChange()
{
    QtMaterialNavigationRail rail;
    QSignalSpy spy(&rail, &QtMaterialNavigationRail::labelsVisibleChanged);

    rail.setLabelsVisible(false);
    QCOMPARE(rail.labelsVisible(), false);
    QCOMPARE(spy.count(), 1);

    rail.setLabelsVisible(false);
    QCOMPARE(spy.count(), 1);

    rail.setLabelsVisible(true);
    QCOMPARE(rail.labelsVisible(), true);
    QCOMPARE(spy.count(), 2);
}

void NavigationRailTest::accessibilitySummaryTracksSelectionAndEnabledState()
{
    QtMaterialNavigationRail rail;
    rail.addDestination(QStringLiteral("Home"));
    rail.addDestination(QStringLiteral("Settings"));

    QSignalSpy summarySpy(&rail, &QtMaterialNavigationRail::accessibilitySummaryChanged);

    rail.setCurrentIndex(1);
    QVERIFY(rail.accessibilitySummary().contains(QStringLiteral("2")));
    QVERIFY(rail.accessibilitySummary().contains(QStringLiteral("Settings")));
    QCOMPARE(rail.accessibleDescription(), rail.accessibilitySummary());

    QVERIFY(rail.destinationAccessibleText(1).contains(QStringLiteral("selected")));

    rail.setDestinationEnabled(1, false);
    QCOMPARE(rail.currentIndex(), 0);
    QVERIFY(rail.destinationAccessibleText(1).contains(QStringLiteral("disabled")));
    QVERIFY(summarySpy.count() >= 1);
}

QTEST_MAIN(NavigationRailTest)
#include "tst_navigationrail.moc"
