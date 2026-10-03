#include <QPixmap>
#include <QSignalSpy>
#include <QTest>

#include "qtmaterial/widgets/navigation/qtmaterialmenu.h"

class tst_Menu : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void addsItemsAndSeparators();
    void keyboardNavigationSkipsDisabledItemsAndSeparators();
    void activationTogglesCheckableItemAndEmitsSignal();
    void escapeDismissesMenu();
    void exposesAccessibilitySummary();
    void typeAheadSelectsMatchingItem();
    void rtlLayoutPreservesGeometryAndInteraction();
    void rendersAtDesktopScaleFactors_data();
    void rendersAtDesktopScaleFactors();
};

void tst_Menu::addsItemsAndSeparators()
{
    QtMaterialMenu menu;

    const int first = menu.addItem(QStringLiteral("Copy"));
    const int separator = menu.addSeparator();
    const int second = menu.addItem(QStringLiteral("Paste"));

    QCOMPARE(first, 0);
    QCOMPARE(separator, 1);
    QCOMPARE(second, 2);
    QCOMPARE(menu.count(), 3);
    QCOMPARE(menu.itemText(first), QStringLiteral("Copy"));
    QVERIFY(menu.isSeparator(separator));
    QVERIFY(menu.isItemEnabled(first));
    QVERIFY(menu.isItemEnabled(second));
    QVERIFY(!menu.isItemEnabled(separator));
    QCOMPARE(menu.currentIndex(), first);
}

void tst_Menu::keyboardNavigationSkipsDisabledItemsAndSeparators()
{
    QtMaterialMenu menu;
    menu.addItem(QStringLiteral("One"));
    const int disabled = menu.addItem(QStringLiteral("Two"));
    menu.addSeparator();
    const int three = menu.addItem(QStringLiteral("Three"));
    menu.setItemEnabled(disabled, false);

    menu.resize(menu.sizeHint());
    menu.show();
    QVERIFY(QTest::qWaitForWindowExposed(&menu));
    menu.activateWindow();
    menu.setFocus();
    QTRY_VERIFY(menu.hasFocus());

    QCOMPARE(menu.currentIndex(), 0);
    QTest::keyClick(&menu, Qt::Key_Down);
    QCOMPARE(menu.currentIndex(), three);
    QTest::keyClick(&menu, Qt::Key_Up);
    QCOMPARE(menu.currentIndex(), 0);
    QTest::keyClick(&menu, Qt::Key_End);
    QCOMPARE(menu.currentIndex(), three);
    QTest::keyClick(&menu, Qt::Key_Home);
    QCOMPARE(menu.currentIndex(), 0);
}

void tst_Menu::activationTogglesCheckableItemAndEmitsSignal()
{
    QtMaterialMenu menu;
    const int index = menu.addItem(QStringLiteral("Show grid"));
    menu.setItemCheckable(index, true);

    QSignalSpy activatedSpy(&menu, &QtMaterialMenu::activated);

    menu.resize(menu.sizeHint());
    menu.show();
    QVERIFY(QTest::qWaitForWindowExposed(&menu));
    menu.activateWindow();
    menu.setFocus();
    QTRY_VERIFY(menu.hasFocus());

    QTest::keyClick(&menu, Qt::Key_Return);
    QCOMPARE(activatedSpy.count(), 1);
    QCOMPARE(activatedSpy.takeFirst().at(0).toInt(), index);
    QVERIFY(menu.isItemChecked(index));

    QTest::keyClick(&menu, Qt::Key_Space);
    QCOMPARE(activatedSpy.count(), 1);
    QVERIFY(!menu.isItemChecked(index));
}

void tst_Menu::escapeDismissesMenu()
{
    QtMaterialMenu menu;
    menu.addItem(QStringLiteral("Close"));
    QSignalSpy dismissedSpy(&menu, &QtMaterialMenu::dismissed);

    menu.resize(menu.sizeHint());
    menu.show();
    QVERIFY(QTest::qWaitForWindowExposed(&menu));
    menu.activateWindow();
    menu.setFocus();
    QTRY_VERIFY(menu.hasFocus());

    QTest::keyClick(&menu, Qt::Key_Escape);
    QCOMPARE(dismissedSpy.count(), 1);
    QVERIFY(!menu.isVisible());
}

void tst_Menu::exposesAccessibilitySummary()
{
    QtMaterialMenu menu;
    const int first = menu.addItem(QStringLiteral("Open"));
    const int second = menu.addItem(QStringLiteral("Delete"));
    menu.setItemCheckable(second, true);
    menu.setItemChecked(second, true);

    QCOMPARE(menu.accessibleName(), QStringLiteral("Menu"));
    QVERIFY(menu.accessibilitySummary().contains(QStringLiteral("2")));
    QVERIFY(menu.accessibilitySummary().contains(QStringLiteral("Open")));
    QVERIFY(menu.itemAccessibleText(first).contains(QStringLiteral("Open")));
    QVERIFY(menu.itemAccessibleText(second).contains(QStringLiteral("checked")));
    QCOMPARE(menu.accessibleDescription(), menu.accessibilitySummary());
}

void tst_Menu::typeAheadSelectsMatchingItem()
{
    QtMaterialMenu menu;
    menu.addItem(QStringLiteral("Open"));
    menu.addItem(QStringLiteral("Paste"));
    menu.addItem(QStringLiteral("Preferences"));
    menu.addItem(QStringLiteral("Print"));
    menu.setCurrentIndex(0);

    menu.resize(menu.sizeHint());
    menu.show();
    QVERIFY(QTest::qWaitForWindowExposed(&menu));
    menu.activateWindow();
    menu.setFocus();
    QTRY_VERIFY(menu.hasFocus());

    QTest::keyClicks(&menu, QStringLiteral("p"));
    QCOMPARE(menu.currentIndex(), 1);

    QTest::keyClicks(&menu, QStringLiteral("p"));
    QCOMPARE(menu.currentIndex(), 2);

    QTest::keyClicks(&menu, QStringLiteral("p"));
    QCOMPARE(menu.currentIndex(), 3);
}

void tst_Menu::rtlLayoutPreservesGeometryAndInteraction()
{
    QtMaterialMenu menu;
    const int copy = menu.addItem(QStringLiteral("Copy"));
    menu.setItemShortcutText(copy, QStringLiteral("Ctrl+C"));
    const int showGrid = menu.addItem(QStringLiteral("Show grid"));
    menu.setItemCheckable(showGrid, true);
    menu.setItemChecked(showGrid, true);
    menu.addSeparator();
    const int disabled = menu.addItem(QStringLiteral("Delete"));
    menu.setItemEnabled(disabled, false);

    const QSize ltrSize = menu.sizeHint();
    const QRect ltrFirst = menu.itemRect(copy);

    menu.setLayoutDirection(Qt::RightToLeft);
    QCOMPARE(menu.sizeHint(), ltrSize);
    QCOMPARE(menu.itemRect(copy), ltrFirst);

    menu.resize(menu.sizeHint());
    menu.show();
    QVERIFY(QTest::qWaitForWindowExposed(&menu));
    menu.activateWindow();
    menu.setFocus();
    QTRY_VERIFY(menu.hasFocus());

    QCOMPARE(menu.currentIndex(), copy);
    QTest::keyClick(&menu, Qt::Key_Down);
    QCOMPARE(menu.currentIndex(), showGrid);
    QTest::keyClick(&menu, Qt::Key_Down);
    QCOMPARE(menu.currentIndex(), copy);
    QVERIFY(menu.itemAccessibleText(showGrid).contains(QStringLiteral("checked")));
}

void tst_Menu::rendersAtDesktopScaleFactors_data()
{
    QTest::addColumn<qreal>("dpr");
    QTest::newRow("100-percent") << qreal(1.00);
    QTest::newRow("125-percent") << qreal(1.25);
    QTest::newRow("150-percent") << qreal(1.50);
    QTest::newRow("175-percent") << qreal(1.75);
    QTest::newRow("200-percent") << qreal(2.00);
}

void tst_Menu::rendersAtDesktopScaleFactors()
{
    QFETCH(qreal, dpr);
    QtMaterialMenu menu;
    const int open = menu.addItem(QStringLiteral("Open"));
    menu.setItemShortcutText(open, QStringLiteral("Ctrl+O"));
    const int grid = menu.addItem(QStringLiteral("Show grid"));
    menu.setItemCheckable(grid, true);
    menu.setItemChecked(grid, true);
    menu.addSeparator();
    const int remove = menu.addItem(QStringLiteral("Delete"));
    menu.setItemEnabled(remove, false);
    menu.setLayoutDirection(Qt::RightToLeft);
    menu.resize(menu.sizeHint());

    QPixmap pixmap(
        qMax(1, qRound(menu.width() * dpr)),
        qMax(1, qRound(menu.height() * dpr)));
    pixmap.setDevicePixelRatio(dpr);
    pixmap.fill(Qt::transparent);
    menu.render(&pixmap);

    QVERIFY(!pixmap.isNull());
    QCOMPARE(pixmap.devicePixelRatio(), dpr);
}

QTEST_MAIN(tst_Menu)
#include "tst_menu.moc"
