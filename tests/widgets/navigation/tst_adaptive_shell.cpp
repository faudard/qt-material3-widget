#include <QtTest/QtTest>

#include <QWidget>

#include "qtmaterial/foundation/qtmaterialwindowsizeclass.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/layouts/qtmaterialadaptiveshell.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationsuite.h"

using namespace QtMaterial;

class AdaptiveShellTest : public QObject
{
    Q_OBJECT

private slots:
    void breakpointBoundaries()
    {
        QCOMPARE(
            WindowSizeClass::classifyWidth(599),
            WindowWidthSizeClass::Compact);
        QCOMPARE(
            WindowSizeClass::classifyWidth(600),
            WindowWidthSizeClass::Medium);
        QCOMPARE(
            WindowSizeClass::classifyWidth(839),
            WindowWidthSizeClass::Medium);
        QCOMPARE(
            WindowSizeClass::classifyWidth(840),
            WindowWidthSizeClass::Expanded);
        QCOMPARE(
            WindowSizeClass::classifyWidth(1200),
            WindowWidthSizeClass::Large);
        QCOMPARE(
            WindowSizeClass::classifyWidth(1600),
            WindowWidthSizeClass::ExtraLarge);

        QCOMPARE(
            WindowSizeClass::classifyHeight(479),
            WindowHeightSizeClass::Compact);
        QCOMPARE(
            WindowSizeClass::classifyHeight(480),
            WindowHeightSizeClass::Medium);
        QCOMPARE(
            WindowSizeClass::classifyHeight(900),
            WindowHeightSizeClass::Expanded);
    }

    void navigationSuiteAdaptsAndKeepsSelection()
    {
        QtMaterialNavigationSuite navigation;
        navigation.addDestination(QStringLiteral("Home"));
        navigation.addDestination(QStringLiteral("Search"));
        navigation.addDestination(QStringLiteral("Settings"));
        navigation.setCurrentIndex(0);

        navigation.setWindowWidthSizeClass(WindowWidthSizeClass::Compact);
        QCOMPARE(navigation.navigationType(), NavigationSuiteType::NavigationBar);
        QTest::keyClick(&navigation, Qt::Key_Right);
        QCOMPARE(navigation.currentIndex(), 1);

        navigation.setWindowWidthSizeClass(WindowWidthSizeClass::Medium);
        QCOMPARE(navigation.navigationType(), NavigationSuiteType::NavigationRail);
        QCOMPARE(navigation.currentIndex(), 1);
        QTest::keyClick(&navigation, Qt::Key_Down);
        QCOMPARE(navigation.currentIndex(), 2);
    }

    void shellChangesNavigationDensityAndSecondaryPane()
    {
        QtMaterialAdaptiveShell shell;
        QWidget content;
        QWidget supporting;
        QtMaterialFilledButton childButton(&content);

        shell.setContentWidget(&content);
        shell.setSupportingWidget(&supporting);

        shell.resize(500, 700);
        QCoreApplication::processEvents();
        QCOMPARE(
            shell.windowSizeClass().width,
            WindowWidthSizeClass::Compact);
        QCOMPARE(
            shell.navigationSuite()->navigationType(),
            NavigationSuiteType::NavigationBar);
        QCOMPARE(shell.resolvedDensity(), Density::Default);
        QCOMPARE(childButton.density(), Density::Default);
        QVERIFY(supporting.isHidden());

        shell.resize(700, 700);
        QCoreApplication::processEvents();
        QCOMPARE(
            shell.windowSizeClass().width,
            WindowWidthSizeClass::Medium);
        QCOMPARE(
            shell.navigationSuite()->navigationType(),
            NavigationSuiteType::NavigationRail);
        QCOMPARE(shell.resolvedDensity(), Density::Comfortable);
        QCOMPARE(childButton.density(), Density::Comfortable);
        QVERIFY(supporting.isHidden());

        shell.resize(1000, 700);
        QCoreApplication::processEvents();
        QCOMPARE(
            shell.windowSizeClass().width,
            WindowWidthSizeClass::Expanded);
        QCOMPARE(shell.resolvedDensity(), Density::Compact);
        QCOMPARE(childButton.density(), Density::Compact);
        QVERIFY(!supporting.isHidden());
        QVERIFY(content.width() >= 320);
        QVERIFY(supporting.width() >= 240);
    }

    void shellPlacesRailOnTheTrailingSideInRtl()
    {
        QtMaterialAdaptiveShell shell;
        QWidget content;

        shell.setContentWidget(&content);
        shell.setLayoutDirection(Qt::RightToLeft);
        shell.resize(900, 700);
        QCoreApplication::processEvents();

        QCOMPARE(
            shell.navigationSuite()->navigationType(),
            NavigationSuiteType::NavigationRail);
        QVERIFY(shell.navigationSuite()->geometry().left() > content.geometry().left());
    }
};

QTEST_MAIN(AdaptiveShellTest)
#include "tst_adaptive_shell.moc"
