#include <QtTest>

#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationrail.h"

using namespace QtMaterial;

class tst_NavigationRailResolvedWidget : public QObject
{
    Q_OBJECT

private slots:
    void followsThemeContextChanges();
    void componentMetricsReachWidget();
    void mouseActivationOccursOnReleaseOnce();
    void disabledDestinationDoesNotActivate();
};

void tst_NavigationRailResolvedWidget::
followsThemeContextChanges()
{
    Theme first = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    Theme second = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#006874")));
    ThemeContext context(first);

    QtMaterialNavigationRail rail;
    rail.setThemeContext(&context);

    const QColor before =
        rail.resolvedSpec().containerColor;

    QVERIFY(context.setTheme(second));

    const NavigationRailSpec expected =
        NavigationRailSpecResolver()
            .navigationRailSpec(second);
    QCOMPARE(
        rail.resolvedSpec().containerColor,
        expected.containerColor);
    QVERIFY(
        rail.resolvedSpec().containerColor != before);
}

void tst_NavigationRailResolvedWidget::
componentMetricsReachWidget()
{
    Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));

    ComponentTokenOverride tokens;
    tokens.custom.insert(
        QStringLiteral("railWidth"),
        104);
    tokens.custom.insert(
        QStringLiteral("itemHeight"),
        72);
    tokens.custom.insert(
        QStringLiteral("itemSpacing"),
        9);
    tokens.custom.insert(
        QStringLiteral("topPadding"),
        15);
    theme.componentOverrides().setOverride(
        QStringLiteral("navigationRail"),
        tokens);

    ThemeContext context(theme);
    QtMaterialNavigationRail rail;
    rail.setThemeContext(&context);
    rail.addDestination(QStringLiteral("Home"));
    rail.addDestination(QStringLiteral("Search"));

    QCOMPARE(rail.sizeHint().width(), 104);
    QCOMPARE(rail.resolvedSpec().itemHeight, 72);
    QCOMPARE(rail.resolvedSpec().itemSpacing, 9);
    QCOMPARE(rail.resolvedSpec().topPadding, 15);
}

void tst_NavigationRailResolvedWidget::
mouseActivationOccursOnReleaseOnce()
{
    QtMaterialNavigationRail rail;
    rail.addDestination(QStringLiteral("Home"));
    rail.addDestination(QStringLiteral("Settings"));
    rail.resize(rail.sizeHint());

    const NavigationRailSpec& spec =
        rail.resolvedSpec();
    const QPoint firstItemCenter(
        spec.railWidth / 2,
        spec.topPadding + spec.itemHeight / 2);

    QSignalSpy activated(
        &rail,
        &QtMaterialNavigationRail::destinationActivated);

    QTest::mousePress(
        &rail,
        Qt::LeftButton,
        Qt::NoModifier,
        firstItemCenter);
    QCOMPARE(activated.count(), 0);

    QTest::mouseRelease(
        &rail,
        Qt::LeftButton,
        Qt::NoModifier,
        firstItemCenter);

    QCOMPARE(activated.count(), 1);
    QCOMPARE(rail.currentIndex(), 0);
    QCOMPARE(
        activated.takeFirst().at(0).toInt(),
        0);
}

void tst_NavigationRailResolvedWidget::
disabledDestinationDoesNotActivate()
{
    QtMaterialNavigationRail rail;
    rail.addDestination(QStringLiteral("Home"));
    rail.addDestination(QStringLiteral("Settings"));
    rail.setDestinationEnabled(1, false);
    rail.resize(rail.sizeHint());

    const NavigationRailSpec& spec =
        rail.resolvedSpec();
    const QPoint disabledCenter(
        spec.railWidth / 2,
        spec.topPadding
            + spec.itemHeight
            + spec.itemSpacing
            + spec.itemHeight / 2);

    QSignalSpy activated(
        &rail,
        &QtMaterialNavigationRail::destinationActivated);

    QTest::mouseClick(
        &rail,
        Qt::LeftButton,
        Qt::NoModifier,
        disabledCenter);

    QCOMPARE(activated.count(), 0);
    QCOMPARE(rail.currentIndex(), -1);
}

QTEST_MAIN(tst_NavigationRailResolvedWidget)
#include "tst_navigationrailresolvedwidget.moc"
