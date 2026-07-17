#include <QSignalSpy>
#include <QtTest/QtTest>

#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/navigation/qtmaterialtabs.h"

using namespace QtMaterial;

namespace {

Theme makeTheme(const QString& seed, bool dark = false)
{
    ThemeBuilder builder;
    return dark
        ? builder.buildDarkFromSeed(QColor(seed))
        : builder.buildLightFromSeed(QColor(seed));
}

} // namespace

class tst_TabsThemeContext : public QObject
{
    Q_OBJECT

private slots:
    void explicitContextOverridesDefaultContext();
    void contextThemeChangesRefreshResolvedSpec();
    void inheritsMaterialWidgetParentContext();
    void followsParentContextReplacement();
    void explicitContextStopsParentInheritance();
};

void tst_TabsThemeContext::explicitContextOverridesDefaultContext()
{
    ThemeContext context(makeTheme(QStringLiteral("#6750A4")));
    QtMaterialTabs tabs;

    QSignalSpy explicitSpy(
        &tabs,
        &QtMaterialTabs::themeContextChanged);
    QSignalSpy effectiveSpy(
        &tabs,
        &QtMaterialTabs::effectiveThemeContextChanged);

    tabs.setThemeContext(&context);

    QCOMPARE(tabs.themeContext(), &context);
    QCOMPARE(tabs.effectiveThemeContext(), &context);
    QCOMPARE(explicitSpy.count(), 1);
    QCOMPARE(effectiveSpy.count(), 1);
    QCOMPARE(
        tabs.resolvedSpec().activeIndicatorColor,
        context.theme().colorScheme().color(ColorRole::Primary));
}

void tst_TabsThemeContext::contextThemeChangesRefreshResolvedSpec()
{
    const Theme first = makeTheme(QStringLiteral("#6750A4"));
    const Theme second =
        makeTheme(QStringLiteral("#146C2E"), true);

    ThemeContext context(first);
    QtMaterialTabs tabs;
    tabs.setThemeContext(&context);

    const QColor firstIndicator =
        tabs.resolvedSpec().activeIndicatorColor;

    QVERIFY(context.setTheme(second));

    QCOMPARE(
        tabs.resolvedSpec().activeIndicatorColor,
        second.colorScheme().color(ColorRole::Primary));
    QVERIFY(
        tabs.resolvedSpec().activeIndicatorColor
        != firstIndicator);
}

void tst_TabsThemeContext::inheritsMaterialWidgetParentContext()
{
    ThemeContext context(makeTheme(QStringLiteral("#6750A4")));
    QtMaterialWidget parent;
    parent.setThemeContext(&context);

    auto* tabs = new QtMaterialTabs(&parent);

    QCOMPARE(tabs->themeContext(), nullptr);
    QCOMPARE(tabs->effectiveThemeContext(), &context);
    QCOMPARE(
        tabs->resolvedSpec().activeIndicatorColor,
        context.theme().colorScheme().color(ColorRole::Primary));
}

void tst_TabsThemeContext::followsParentContextReplacement()
{
    ThemeContext first(makeTheme(QStringLiteral("#6750A4")));
    ThemeContext second(
        makeTheme(QStringLiteral("#146C2E"), true));

    QtMaterialWidget parent;
    parent.setThemeContext(&first);
    auto* tabs = new QtMaterialTabs(&parent);

    QSignalSpy effectiveSpy(
        tabs,
        &QtMaterialTabs::effectiveThemeContextChanged);

    parent.setThemeContext(&second);

    QCOMPARE(tabs->effectiveThemeContext(), &second);
    QVERIFY(effectiveSpy.count() >= 1);
    QCOMPARE(
        tabs->resolvedSpec().activeIndicatorColor,
        second.theme().colorScheme().color(ColorRole::Primary));
}

void tst_TabsThemeContext::explicitContextStopsParentInheritance()
{
    ThemeContext parentContext(
        makeTheme(QStringLiteral("#6750A4")));
    ThemeContext localContext(
        makeTheme(QStringLiteral("#146C2E"), true));
    ThemeContext replacementParent(
        makeTheme(QStringLiteral("#B3261E")));

    QtMaterialWidget parent;
    parent.setThemeContext(&parentContext);
    auto* tabs = new QtMaterialTabs(&parent);
    tabs->setThemeContext(&localContext);

    parent.setThemeContext(&replacementParent);

    QCOMPARE(tabs->effectiveThemeContext(), &localContext);
    QCOMPARE(
        tabs->resolvedSpec().activeIndicatorColor,
        localContext.theme().colorScheme().color(ColorRole::Primary));

    tabs->setThemeContext(nullptr);

    QCOMPARE(
        tabs->effectiveThemeContext(),
        &replacementParent);
}

QTEST_MAIN(tst_TabsThemeContext)

#include "tst_tabs_themecontext.moc"
