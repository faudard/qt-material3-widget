#include <QPixmap>
#include <QtTest/QtTest>

#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationrail.h"

using namespace QtMaterial;

class tst_NavigationRailResolvedSpec
    : public QObject
{
    Q_OBJECT

private slots:
    void followsThemeContext();
    void handlesHoverAndPress();
};

void tst_NavigationRailResolvedSpec::
followsThemeContext()
{
    ThemeBuilder builder;
    Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    ComponentTokenOverride tokens;
    tokens.custom.insert(
        QStringLiteral("railWidth"),
        94);
    theme.componentOverrides().setOverride(
        QStringLiteral("navigationRail"),
        tokens);

    ThemeContext context(theme);
    QtMaterialNavigationRail rail;
    rail.setThemeContext(&context);

    QCOMPARE(rail.resolvedSpec().railWidth, 94);
    QCOMPARE(rail.sizeHint().width(), 94);
}

void tst_NavigationRailResolvedSpec::
handlesHoverAndPress()
{
    QtMaterialNavigationRail rail;

    QPixmap icon(24, 24);
    icon.fill(Qt::white);

    rail.addDestination(
        QStringLiteral("Home"),
        QIcon(icon));
    rail.addDestination(
        QStringLiteral("Settings"));
    rail.resize(rail.sizeHint());
    rail.show();
    QTRY_VERIFY_WITH_TIMEOUT(rail.isVisible(), 1000);

    const NavigationRailSpec spec =
        rail.resolvedSpec();
    const QPoint target(
        spec.railWidth / 2,
        spec.topPadding
            + spec.itemHeight / 2);

    QTest::mouseMove(&rail, target);
    QTest::mouseClick(
        &rail,
        Qt::LeftButton,
        Qt::NoModifier,
        target);

    QCOMPARE(rail.currentIndex(), 0);
    QVERIFY(!rail.grab().isNull());
}

QTEST_MAIN(tst_NavigationRailResolvedSpec)

#include "tst_navigationrail_resolvedspec.moc"
