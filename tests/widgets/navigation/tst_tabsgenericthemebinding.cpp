#include <QtTest>

#include "qtmaterial/core/qtmaterialthemecontextbinding.h"
#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontexthost.h"
#include "qtmaterial/widgets/navigation/qtmaterialroute.h"
#include "qtmaterial/widgets/navigation/qtmaterialtabs.h"

using namespace QtMaterial;

class tst_TabsGenericThemeBinding : public QObject
{
    Q_OBJECT

private slots:
    void routeHeaderIsSelfContained();
    void tabsImplementsThemeContextHost();
    void followsParentContextReplacement();
    void explicitContextWins();
};

void tst_TabsGenericThemeBinding::routeHeaderIsSelfContained()
{
    QCOMPARE(
        QtMaterialRoute::normalizedPath(
            QStringLiteral("  settings\\\\profile///  ")),
        QStringLiteral("/settings/profile"));
    QCOMPARE(
        QtMaterialRoute(QStringLiteral("///")).path(),
        QStringLiteral("/"));
    QVERIFY(!QtMaterialRoute(QStringLiteral("   ")).isValid());
}

void tst_TabsGenericThemeBinding::tabsImplementsThemeContextHost()
{
    QtMaterialTabs tabs;

    auto* host =
        qobject_cast<ThemeContextHost*>(&tabs);
    QVERIFY(host);
    QCOMPARE(
        host->effectiveThemeContext(),
        tabs.effectiveThemeContext());
}

void tst_TabsGenericThemeBinding::
followsParentContextReplacement()
{
    ThemeContext first(
        ThemeBuilder().buildLightFromSeed(
            QColor(QStringLiteral("#6750A4"))));
    ThemeContext second(
        ThemeBuilder().buildDarkFromSeed(
            QColor(QStringLiteral("#006874"))));

    QtMaterialWidget parent;
    parent.setThemeContext(&first);

    QtMaterialTabs tabs(&parent);
    QCOMPARE(tabs.effectiveThemeContext(), &first);

    QSignalSpy effectiveSpy(
        &tabs,
        &QtMaterialTabs::effectiveThemeContextChanged);

    parent.setThemeContext(&second);

    QCOMPARE(tabs.effectiveThemeContext(), &second);
    QCOMPARE(effectiveSpy.count(), 1);
    QCOMPARE(
        tabs.resolvedSpec().containerColor,
        TabsSpecResolver().resolve(second.theme())
            .containerColor);
}

void tst_TabsGenericThemeBinding::explicitContextWins()
{
    ThemeContext inherited(
        ThemeBuilder().buildLightFromSeed(
            QColor(QStringLiteral("#6750A4"))));
    ThemeContext explicitContext(
        ThemeBuilder().buildDarkFromSeed(
            QColor(QStringLiteral("#006874"))));

    QtMaterialWidget parent;
    parent.setThemeContext(&inherited);

    QtMaterialTabs tabs(&parent);
    QSignalSpy explicitSpy(
        &tabs,
        &QtMaterialTabs::themeContextChanged);

    tabs.setThemeContext(&explicitContext);

    QCOMPARE(tabs.themeContext(), &explicitContext);
    QCOMPARE(
        tabs.effectiveThemeContext(),
        &explicitContext);
    QCOMPARE(explicitSpy.count(), 1);
}

QTEST_MAIN(tst_TabsGenericThemeBinding)
#include "tst_tabsgenericthemebinding.moc"
