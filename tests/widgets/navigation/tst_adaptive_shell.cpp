#include <QtTest/QtTest>

#include <QAccessible>
#include <QApplication>
#include <QSignalSpy>
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
        shell.show();
        QCoreApplication::processEvents();

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

    void navigationSuiteAccessibilitySurvivesModeSwitch()
    {
#ifndef QT_NO_ACCESSIBILITY
        QtMaterialNavigationSuite navigation;
        navigation.addDestination(QStringLiteral("Home"));
        navigation.addDestination(QStringLiteral("Search"));
        navigation.addDestination(QStringLiteral("Disabled"));
        navigation.addDestination(QStringLiteral("Settings"));
        navigation.setDestinationEnabled(2, false);
        navigation.setCurrentIndex(1);
        navigation.resize(420, 80);
        navigation.show();
        QVERIFY(QTest::qWaitForWindowExposed(&navigation));
        navigation.activateWindow();
        navigation.setFocus(Qt::OtherFocusReason);
        QTRY_VERIFY(navigation.hasFocus());

        auto* accessible = QAccessible::queryAccessibleInterface(&navigation);
        QVERIFY(accessible);
        QCOMPARE(accessible->role(), QAccessible::List);
        QCOMPARE(accessible->childCount(), 4);

        auto* search = accessible->child(1);
        auto* disabled = accessible->child(2);
        QVERIFY(search);
        QVERIFY(disabled);
        QVERIFY(search->state().selected);
        QVERIFY(search->state().focused);
        QVERIFY(disabled->state().disabled);
        QVERIFY(navigation.accessibilitySummary().contains(
            QStringLiteral("bottom navigation")));

        navigation.setWindowWidthSizeClass(WindowWidthSizeClass::Medium);
        navigation.resize(navigation.sizeHint());
        QCoreApplication::processEvents();

        QCOMPARE(navigation.currentIndex(), 1);
        QCOMPARE(navigation.navigationType(), NavigationSuiteType::NavigationRail);
        QVERIFY(navigation.accessibilitySummary().contains(
            QStringLiteral("navigation rail")));
        QVERIFY(accessible->child(1)->state().selected);
        QVERIFY(accessible->child(1)->state().focused);

        QTest::keyClick(&navigation, Qt::Key_Down);
        QCOMPARE(navigation.currentIndex(), 3);
        QVERIFY(accessible->child(3)->state().focused);
#else
        QSKIP("Qt accessibility disabled");
#endif
    }

    void shellCoversAllDesktopWidthClasses()
    {
        QtMaterialAdaptiveShell shell;
        QWidget content;
        QWidget supporting;
        QtMaterialFilledButton childButton(&content);

        shell.setContentWidget(&content);
        shell.setSupportingWidget(&supporting);
        shell.show();
        QVERIFY(QTest::qWaitForWindowExposed(&shell));

        struct Case {
            int width;
            WindowWidthSizeClass sizeClass;
            NavigationSuiteType navigationType;
            Density density;
            bool supportingVisible;
        };

        const Case cases[] = {
            {500, WindowWidthSizeClass::Compact, NavigationSuiteType::NavigationBar, Density::Default, false},
            {700, WindowWidthSizeClass::Medium, NavigationSuiteType::NavigationRail, Density::Comfortable, false},
            {1000, WindowWidthSizeClass::Expanded, NavigationSuiteType::NavigationRail, Density::Compact, true},
            {1300, WindowWidthSizeClass::Large, NavigationSuiteType::NavigationRail, Density::Compact, true},
            {1700, WindowWidthSizeClass::ExtraLarge, NavigationSuiteType::NavigationRail, Density::Compact, true},
        };

        QSignalSpy widthSpy(&shell, &QtMaterialAdaptiveShell::widthSizeClassChanged);
        QSignalSpy paneSpy(&shell, &QtMaterialAdaptiveShell::supportingPaneVisibleChanged);
        QSignalSpy summarySpy(&shell, &QtMaterialAdaptiveShell::accessibilitySummaryChanged);

        for (const Case& entry : cases) {
            shell.resize(entry.width, 700);
            QCoreApplication::processEvents();

            QCOMPARE(shell.windowSizeClass().width, entry.sizeClass);
            QCOMPARE(shell.navigationSuite()->navigationType(), entry.navigationType);
            QCOMPARE(shell.resolvedDensity(), entry.density);
            QCOMPARE(childButton.density(), entry.density);
            QCOMPARE(shell.isSupportingPaneVisible(), entry.supportingVisible);
            QCOMPARE(!supporting.isHidden(), entry.supportingVisible);
            QVERIFY(shell.accessibilitySummary().contains(
                entry.supportingVisible
                    ? QStringLiteral("supporting pane visible")
                    : QStringLiteral("supporting pane hidden")));
            if (entry.supportingVisible) {
                QVERIFY(content.width() >= 320);
                QVERIFY(supporting.width() >= 240);
            }
        }

        QVERIFY(widthSpy.count() >= 4);
        QVERIFY(paneSpy.count() >= 1);
        QVERIFY(summarySpy.count() >= 1);
    }

    void automaticDensityOptOutPreservesApplicationDensity()
    {
        QtMaterialAdaptiveShell shell;
        QWidget content;
        QtMaterialFilledButton childButton(&content);

        shell.setContentWidget(&content);
        shell.setAutomaticDensity(false);
        childButton.setDensity(Density::Default);
        shell.resize(700, 700);
        QCoreApplication::processEvents();

        QCOMPARE(shell.resolvedDensity(), Density::Comfortable);
        QCOMPARE(childButton.density(), Density::Default);

        shell.setAutomaticDensity(true);
        QCOMPARE(childButton.density(), Density::Comfortable);
    }

    void supportingPaneVisibilityTracksSpaceAndRtl()
    {
        QtMaterialAdaptiveShell shell;
        QWidget content;
        QWidget supporting;

        shell.setContentWidget(&content);
        shell.setSupportingWidget(&supporting);
        shell.setSupportingPaneWidth(520);
        shell.resize(900, 700);
        shell.show();
        QVERIFY(QTest::qWaitForWindowExposed(&shell));
        QCoreApplication::processEvents();

        QVERIFY(shell.isSupportingPaneVisible());
        QVERIFY(content.width() >= 320);
        QVERIFY(supporting.width() >= 240);
        QVERIFY(content.geometry().right() < supporting.geometry().left());

        shell.setLayoutDirection(Qt::RightToLeft);
        QCoreApplication::processEvents();

        QVERIFY(shell.isSupportingPaneVisible());
        QVERIFY(supporting.geometry().right() < content.geometry().left());
        QVERIFY(shell.navigationSuite()->geometry().left() > content.geometry().left());

        shell.resize(700, 700);
        QCoreApplication::processEvents();
        QVERIFY(!shell.isSupportingPaneVisible());
        QVERIFY(supporting.isHidden());
    }
};

QTEST_MAIN(AdaptiveShellTest)
#include "tst_adaptive_shell.moc"
