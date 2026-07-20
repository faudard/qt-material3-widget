#include <QtTest>

#include <QTabBar>
#include <QVariantAnimation>

#include "qtmaterial/specs/qtmaterialtabsspecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/navigation/qtmaterialtabs.h"

using namespace QtMaterial;

class tst_TabsResolvedLifecycle : public QObject
{
    Q_OBJECT

private slots:
    void componentMetricsReachTabBar();
    void indicatorAnimationUsesResolvedMotion();
    void routeNormalizationRemainsStable();
    void lazyFactoryRunsOnce();
};

namespace {

Theme tabsTestTheme()
{
    Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));

    MotionStyle motion;
    motion.durationMs = 240;
    motion.easing =
        QEasingCurve(QEasingCurve::InOutCubic);

    ComponentTokenOverride tokens;
    tokens.motion.insert(MotionToken::Short4, motion);
    tokens.custom.insert(
        QStringLiteral("stateLayerInset"), 7);
    tokens.custom.insert(
        QStringLiteral("stateLayerRadius"), 18.0);
    tokens.custom.insert(
        QStringLiteral("focusInset"), 5);
    tokens.custom.insert(
        QStringLiteral("focusRingWidth"), 3.0);
    tokens.custom.insert(
        QStringLiteral("indicatorHorizontalInset"), 15);
    tokens.custom.insert(
        QStringLiteral("overflowButtonPadding"), 9);
    tokens.custom.insert(
        QStringLiteral("badgeEndInset"), 12);
    theme.componentOverrides().setOverride(
        QStringLiteral("tabs"),
        tokens);

    return theme;
}

} // namespace

void tst_TabsResolvedLifecycle::
componentMetricsReachTabBar()
{
    ThemeContext context(tabsTestTheme());

    QtMaterialTabs tabs;
    tabs.setThemeContext(&context);
    tabs.addTab(new QWidget(), QStringLiteral("Home"));
    tabs.addTab(new QWidget(), QStringLiteral("Settings"));

    const TabsSpec spec = tabs.resolvedSpec();
    QCOMPARE(spec.stateLayerInset, 7);
    QCOMPARE(spec.stateLayerRadius, 18.0);
    QCOMPARE(spec.focusInset, 5);
    QCOMPARE(spec.focusRingWidth, 3.0);
    QCOMPARE(spec.indicatorHorizontalInset, 15);
    QCOMPARE(spec.overflowButtonPadding, 9);
    QCOMPARE(spec.badgeEndInset, 12);

    auto* bar = tabs.findChild<QTabBar*>();
    QVERIFY(bar);
    QVERIFY(
        bar->styleSheet().contains(
            QStringLiteral("padding: 9px")));
}

void tst_TabsResolvedLifecycle::
indicatorAnimationUsesResolvedMotion()
{
    ThemeContext context(tabsTestTheme());

    QtMaterialTabs tabs;
    tabs.setThemeContext(&context);

    auto* animation =
        tabs.findChild<QVariantAnimation*>();
    QVERIFY(animation);

    QCOMPARE(animation->duration(), 240);
    QCOMPARE(
        animation->easingCurve().type(),
        QEasingCurve::InOutCubic);
}

void tst_TabsResolvedLifecycle::
routeNormalizationRemainsStable()
{
    QtMaterialTabs tabs;
    tabs.addTab(new QWidget(), QStringLiteral("Profile"));
    tabs.setRoute(
        0,
        QStringLiteral("///settings\\\\profile///"));

    QCOMPARE(
        tabs.route(0).path(),
        QStringLiteral("/settings/profile"));
    QCOMPARE(
        tabs.indexOfRoute(
            QStringLiteral(" settings/profile/ ")),
        0);
    QVERIFY(
        tabs.navigateTo(
            QStringLiteral("\\settings\\profile")));
}

void tst_TabsResolvedLifecycle::lazyFactoryRunsOnce()
{
    QtMaterialTabs tabs;
    tabs.setLazyLoading(true);
    const int index =
        tabs.addTab(new QWidget(), QStringLiteral("Lazy"));

    int calls = 0;
    tabs.setTabFactory(
        index,
        [&calls]() -> QWidget* {
            ++calls;
            return new QWidget();
        });

    tabs.ensureTabLoaded(index);
    tabs.ensureTabLoaded(index);

    QCOMPARE(calls, 1);
    QVERIFY(tabs.isTabLoaded(index));
}

QTEST_MAIN(tst_TabsResolvedLifecycle)
#include "tst_tabsresolvedlifecycle.moc"
