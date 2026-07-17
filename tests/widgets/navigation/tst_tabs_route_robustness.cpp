#include <QSignalSpy>
#include <QUrl>
#include <QtTest/QtTest>

#include "qtmaterial/widgets/navigation/qtmaterialtabs.h"

using namespace QtMaterial;

class tst_TabsRouteRobustness : public QObject
{
    Q_OBJECT

private slots:
    void routeValueNormalizesAtConstruction();
    void routeLookupAcceptsLegacyForms();
    void urlNavigationUsesCanonicalRoute();
    void setRouteIsIdempotent();
    void currentRouteSignalIsDeduplicated();
    void currentRouteSurvivesInsertRemove();
};

void tst_TabsRouteRobustness::
routeValueNormalizesAtConstruction()
{
    QCOMPARE(
        QtMaterialRoute(
            QStringLiteral("settings/profile"))
            .path(),
        QStringLiteral("/settings/profile"));
    QCOMPARE(
        QtMaterialRoute(
            QStringLiteral(
                "///settings//profile///"))
            .path(),
        QStringLiteral("/settings/profile"));
    QCOMPARE(
        QtMaterialRoute(
            QStringLiteral(
                "\\settings\\profile\\"))
            .path(),
        QStringLiteral("/settings/profile"));
    QCOMPARE(
        QtMaterialRoute(
            QStringLiteral("/"))
            .path(),
        QStringLiteral("/"));
    QVERIFY(
        !QtMaterialRoute(QStringLiteral("  "))
             .isValid());
}

void tst_TabsRouteRobustness::
routeLookupAcceptsLegacyForms()
{
    QtMaterialTabs tabs;
    tabs.addTab(
        new QWidget(&tabs),
        QStringLiteral("Home"));
    tabs.addTab(
        new QWidget(&tabs),
        QStringLiteral("Settings"));

    tabs.setRoute(
        0,
        QStringLiteral("home"));
    tabs.setRoute(
        1,
        QStringLiteral("/settings/"));

    QCOMPARE(
        tabs.route(0).path(),
        QStringLiteral("/home"));
    QCOMPARE(
        tabs.route(1).path(),
        QStringLiteral("/settings"));

    QCOMPARE(
        tabs.indexOfRoute(
            QStringLiteral("settings")),
        1);
    QCOMPARE(
        tabs.indexOfRoute(
            QStringLiteral("///settings///")),
        1);
    QCOMPARE(
        tabs.indexOfRoute(
            QStringLiteral("\\settings\\")),
        1);

    QVERIFY(
        tabs.navigateTo(
            QStringLiteral("settings/")));
    QCOMPARE(tabs.currentIndex(), 1);
}

void tst_TabsRouteRobustness::
urlNavigationUsesCanonicalRoute()
{
    QtMaterialTabs tabs;
    tabs.addTab(
        new QWidget(&tabs),
        QStringLiteral("Security"));
    tabs.setRoute(
        0,
        QStringLiteral(
            "/settings/security"));

    QVERIFY(
        tabs.navigateToUrl(
            QUrl(
                QStringLiteral(
                    "app://settings/security"))));
    QCOMPARE(
        tabs.route(
            tabs.currentIndex()).path(),
        QStringLiteral(
            "/settings/security"));

    QVERIFY(
        tabs.navigateToUrl(
            QUrl(
                QStringLiteral(
                    "/settings/security"
                    "?source=test#section"))));
}

void tst_TabsRouteRobustness::
setRouteIsIdempotent()
{
    QtMaterialTabs tabs;
    tabs.addTab(
        new QWidget(&tabs),
        QStringLiteral("Settings"));

    QSignalSpy routeSpy(
        &tabs,
        &QtMaterialTabs::routeChanged);
    QSignalSpy currentSpy(
        &tabs,
        &QtMaterialTabs::currentRouteChanged);

    tabs.setRoute(
        0,
        QStringLiteral("settings"));

    QCOMPARE(routeSpy.count(), 1);
    QCOMPARE(currentSpy.count(), 1);

    tabs.setRoute(
        0,
        QStringLiteral("/settings/"));
    tabs.setRoute(
        0,
        QStringLiteral("///settings"));

    QCOMPARE(routeSpy.count(), 1);
    QCOMPARE(currentSpy.count(), 1);
}

void tst_TabsRouteRobustness::
currentRouteSignalIsDeduplicated()
{
    QtMaterialTabs tabs;
    tabs.addTab(
        new QWidget(&tabs),
        QStringLiteral("Home"));
    tabs.addTab(
        new QWidget(&tabs),
        QStringLiteral("Settings"));

    tabs.setRoute(
        0,
        QStringLiteral("/home"));
    tabs.setRoute(
        1,
        QStringLiteral("/settings"));

    tabs.setCurrentIndex(0);

    QSignalSpy currentSpy(
        &tabs,
        &QtMaterialTabs::currentRouteChanged);

    tabs.setCurrentIndex(1);
    QCOMPARE(currentSpy.count(), 1);
    QCOMPARE(
        qvariant_cast<QtMaterialRoute>(
            currentSpy.at(0).at(0)).path(),
        QStringLiteral("/settings"));

    tabs.setCurrentIndex(1);
    QVERIFY(
        tabs.navigateTo(
            QStringLiteral("settings")));

    QCOMPARE(currentSpy.count(), 1);

    tabs.setCurrentIndex(0);
    QCOMPARE(currentSpy.count(), 2);
}

void tst_TabsRouteRobustness::
currentRouteSurvivesInsertRemove()
{
    QtMaterialTabs tabs;
    tabs.addTab(
        new QWidget(&tabs),
        QStringLiteral("A"));
    tabs.addTab(
        new QWidget(&tabs),
        QStringLiteral("B"));

    tabs.setRoute(
        0,
        QStringLiteral("/a"));
    tabs.setRoute(
        1,
        QStringLiteral("/b"));
    tabs.setCurrentIndex(1);

    QSignalSpy currentSpy(
        &tabs,
        &QtMaterialTabs::currentRouteChanged);

    tabs.insertTab(
        0,
        new QWidget(&tabs),
        QStringLiteral("Inserted"));

    QCOMPARE(
        tabs.route(
            tabs.currentIndex()).path(),
        QStringLiteral("/b"));
    QCOMPARE(currentSpy.count(), 0);

    tabs.removeTab(0);

    QCOMPARE(
        tabs.route(
            tabs.currentIndex()).path(),
        QStringLiteral("/b"));
    QCOMPARE(currentSpy.count(), 0);
}

QTEST_MAIN(tst_TabsRouteRobustness)

#include "tst_tabs_route_robustness.moc"
