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
    menu.setFocus();

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
    menu.setFocus();

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
    menu.setFocus();

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

QTEST_MAIN(tst_Menu)
#include "tst_menu.moc"
