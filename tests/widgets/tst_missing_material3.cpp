#include <QtTest/QtTest>

#include <QAccessible>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QSignalSpy>
#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/widgets/data/qtmaterialbadge.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationbar.h"
#include "qtmaterial/widgets/surfaces/qtmaterialsidesheet.h"
#include "qtmaterial/widgets/surfaces/qtmaterialtooltip.h"

using namespace QtMaterial;

class MissingMaterial3Test : public QObject
{
    Q_OBJECT

private slots:
    void navigationBarDestinationModel();
    void navigationBarKeyboardAndRtl();
    void navigationBarAccessibleDestinations();
    void sideSheetLifecycle();
    void sideSheetModalFocusTrapAndRestore();
    void tooltipTargetAndVisibility();
    void badgeCountAndDotMode();
    void customPaintingSupportsDpr2();
};

void MissingMaterial3Test::navigationBarDestinationModel()
{
    QtMaterialNavigationBar bar;
    QCOMPARE(bar.count(), 0);
    QCOMPARE(bar.currentIndex(), -1);
    QCOMPARE(bar.accessibleName(), QStringLiteral("Navigation bar"));

    QCOMPARE(bar.addDestination(QStringLiteral("Home")), 0);
    QCOMPARE(bar.addDestination(QStringLiteral("Search")), 1);
    QCOMPARE(bar.addDestination(QStringLiteral("Settings")), 2);
    QCOMPARE(bar.count(), 3);

    QSignalSpy currentSpy(&bar, &QtMaterialNavigationBar::currentIndexChanged);
    bar.setCurrentIndex(1);
    QCOMPARE(bar.currentIndex(), 1);
    QCOMPARE(currentSpy.count(), 1);

    bar.setDestinationEnabled(1, false);
    QVERIFY(bar.currentIndex() != 1);
    QVERIFY(bar.accessibilitySummary().contains(QStringLiteral("destinations")));
}

void MissingMaterial3Test::navigationBarKeyboardAndRtl()
{
    QtMaterialNavigationBar bar;
    bar.addDestination(QStringLiteral("Home"));
    bar.addDestination(QStringLiteral("Search"));
    bar.addDestination(QStringLiteral("Settings"));
    bar.setCurrentIndex(1);
    bar.resize(360, 80);
    bar.show();
    QVERIFY(QTest::qWaitForWindowExposed(&bar));

    QTest::keyClick(&bar, Qt::Key_Right);
    QCOMPARE(bar.currentIndex(), 2);

    bar.setLayoutDirection(Qt::RightToLeft);
    QTest::keyClick(&bar, Qt::Key_Right);
    QCOMPARE(bar.currentIndex(), 1);

    QSignalSpy activated(&bar, &QtMaterialNavigationBar::destinationActivated);
    QTest::keyClick(&bar, Qt::Key_Space);
    QCOMPARE(activated.count(), 1);
}

void MissingMaterial3Test::navigationBarAccessibleDestinations()
{
#ifndef QT_NO_ACCESSIBILITY
    QtMaterialNavigationBar bar;
    bar.addDestination(QStringLiteral("Home"));
    bar.addDestination(QStringLiteral("Search"));
    bar.addDestination(QStringLiteral("Disabled"));
    bar.setDestinationEnabled(2, false);
    bar.setCurrentIndex(1);
    bar.resize(360, 80);
    bar.show();
    QVERIFY(QTest::qWaitForWindowExposed(&bar));

    auto* root = QAccessible::queryAccessibleInterface(&bar);
    QVERIFY(root);
    QCOMPARE(root->role(), QAccessible::List);
    QCOMPARE(root->childCount(), 3);

    auto* home = root->child(0);
    auto* search = root->child(1);
    auto* disabled = root->child(2);
    QVERIFY(home);
    QVERIFY(search);
    QVERIFY(disabled);

    QCOMPARE(home->role(), QAccessible::ListItem);
    QCOMPARE(home->text(QAccessible::Name), QStringLiteral("Home"));
    QVERIFY(home->text(QAccessible::Description).contains(QStringLiteral("1 of 3")));

    QVERIFY(search->state().selected);
    QVERIFY(disabled->state().disabled);

    auto* action = static_cast<QAccessibleActionInterface*>(
        home->interface_cast(QAccessible::ActionInterface));
    QVERIFY(action);
    QVERIFY(action->actionNames().contains(
        QAccessibleActionInterface::pressAction()));

    QSignalSpy activated(&bar, &QtMaterialNavigationBar::destinationActivated);
    action->doAction(QAccessibleActionInterface::pressAction());
    QCOMPARE(bar.currentIndex(), 0);
    QCOMPARE(activated.count(), 1);
#else
    QSKIP("Qt accessibility disabled");
#endif
}

void MissingMaterial3Test::sideSheetLifecycle()
{
    QWidget host;
    host.resize(800, 600);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    QtMaterialSideSheet sheet(&host);
    sheet.setTitleText(QStringLiteral("Details"));
    sheet.setModal(true);

    auto* contentLayout = new QVBoxLayout(sheet.contentWidget());
    contentLayout->addWidget(new QLabel(QStringLiteral("Side sheet content"), sheet.contentWidget()));

    QSignalSpy openSpy(&sheet, &QtMaterialSideSheet::openChanged);
    sheet.open();
    QVERIFY(sheet.isOpen());
    QVERIFY(sheet.isVisible());
    QCOMPARE(sheet.height(), host.height());
    QVERIFY(sheet.width() >= sheet.minimumSizeHint().width());
    QVERIFY(openSpy.count() >= 1);

    QTest::keyClick(&sheet, Qt::Key_Escape);
    QVERIFY(!sheet.isOpen());
    QVERIFY(!sheet.isVisible());
}

void MissingMaterial3Test::sideSheetModalFocusTrapAndRestore()
{
    QWidget host;
    auto* hostLayout = new QVBoxLayout(&host);
    auto* invoker = new QPushButton(QStringLiteral("Open details"), &host);
    hostLayout->addWidget(invoker);
    host.resize(800, 600);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    invoker->setFocus(Qt::OtherFocusReason);
    QTRY_VERIFY(invoker->hasFocus());

    QtMaterialSideSheet sheet(&host);
    sheet.setTitleText(QStringLiteral("Details"));
    sheet.setModal(true);

    auto* contentLayout = new QVBoxLayout(sheet.contentWidget());
    auto* first = new QPushButton(QStringLiteral("First action"), sheet.contentWidget());
    auto* second = new QPushButton(QStringLiteral("Second action"), sheet.contentWidget());
    contentLayout->addWidget(first);
    contentLayout->addWidget(second);

    sheet.setInitialFocusWidget(second);
    QVERIFY(sheet.restoreFocusOnClose());
    sheet.open();
    QTRY_VERIFY(second->hasFocus());

    for (int i = 0; i < 5; ++i) {
        QWidget* focused = QApplication::focusWidget();
        QVERIFY(focused);
        QTest::keyClick(focused, Qt::Key_Tab);
        QTRY_VERIFY(QApplication::focusWidget());
        QWidget* next = QApplication::focusWidget();
        QVERIFY(next == &sheet || sheet.isAncestorOf(next));
    }

    sheet.closeSheet();
    QTRY_VERIFY(invoker->hasFocus());
}

void MissingMaterial3Test::tooltipTargetAndVisibility()
{
    QWidget host;
    host.resize(320, 180);
    auto* layout = new QVBoxLayout(&host);
    auto* target = new QLabel(QStringLiteral("Hover target"), &host);
    layout->addWidget(target);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    QtMaterialTooltip tooltip(&host);
    tooltip.setTargetWidget(target);
    tooltip.setText(QStringLiteral("Helpful context"));
    tooltip.setShowDelay(0);
    tooltip.showTooltip();

    QVERIFY(tooltip.isTooltipVisible());
    QVERIFY(tooltip.sizeHint().width() >= tooltip.minimumSizeHint().width());
    QCOMPARE(tooltip.accessibleDescription(), QStringLiteral("Helpful context"));

    tooltip.hideTooltip();
    QVERIFY(!tooltip.isTooltipVisible());
}

void MissingMaterial3Test::badgeCountAndDotMode()
{
    QtMaterialBadge badge;
    badge.setCount(42);
    QCOMPARE(badge.displayText(), QStringLiteral("42"));

    badge.setMaximum(9);
    QCOMPARE(badge.displayText(), QStringLiteral("9+"));

    const QSize textSize = badge.sizeHint();
    badge.setDot(true);
    QCOMPARE(badge.displayText(), QString());
    QVERIFY(badge.sizeHint().width() < textSize.width());
    QVERIFY(badge.accessibleDescription().contains(QStringLiteral("New content")));
}

void MissingMaterial3Test::customPaintingSupportsDpr2()
{
    QtMaterialNavigationBar bar;
    bar.addDestination(QStringLiteral("Home"));
    bar.addDestination(QStringLiteral("Search"));
    bar.setCurrentIndex(0);
    bar.resize(320, 80);

    QPixmap barPixmap(640, 160);
    barPixmap.setDevicePixelRatio(2.0);
    barPixmap.fill(Qt::transparent);
    bar.render(&barPixmap);
    QCOMPARE(barPixmap.devicePixelRatio(), qreal(2.0));

    QtMaterialBadge badge;
    badge.setCount(8);
    badge.resize(badge.sizeHint());

    QPixmap badgePixmap(
        qMax(1, badge.width() * 2),
        qMax(1, badge.height() * 2));
    badgePixmap.setDevicePixelRatio(2.0);
    badgePixmap.fill(Qt::transparent);
    badge.render(&badgePixmap);
    QCOMPARE(badgePixmap.devicePixelRatio(), qreal(2.0));
}

QTEST_MAIN(MissingMaterial3Test)
#include "tst_missing_material3.moc"
