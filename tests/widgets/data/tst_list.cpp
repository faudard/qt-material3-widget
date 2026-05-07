#include <QtTest/QtTest>

#include "qtmaterial/widgets/data/qtmateriallist.h"
#include "qtmaterial/widgets/data/qtmateriallistitem.h"

using QtMaterial::QtMaterialList;
using QtMaterial::QtMaterialListItem;

class TestQtMaterialList : public QObject
{
    Q_OBJECT

private slots:
    void constructsWithAccessibleSummary();
    void addItemsUpdatesCountAndCurrentIndex();
    void singleSelectionTracksCurrentIndex();
    void keyboardNavigationSkipsDisabledItems();
    void homeAndEndNavigateToEnabledBoundaries();
    void activationEmitsIndex();
    void multiSelectionTogglesActivatedRows();
    void takeItemReparentsWithoutDeleting();
};

void TestQtMaterialList::constructsWithAccessibleSummary()
{
    QtMaterialList list;

    QCOMPARE(list.count(), 0);
    QCOMPARE(list.currentIndex(), -1);
    QCOMPARE(list.accessibilitySummary(), QStringLiteral("Empty list"));
    QCOMPARE(list.accessibleName(), QStringLiteral("List"));
}

void TestQtMaterialList::addItemsUpdatesCountAndCurrentIndex()
{
    QtMaterialList list;
    list.addItem(QStringLiteral("Inbox"));
    list.addItem(QStringLiteral("Archive"));

    QCOMPARE(list.count(), 2);
    QCOMPARE(list.currentIndex(), 0);
    QCOMPARE(list.itemAccessibleText(0), QStringLiteral("Inbox, selected"));
    QVERIFY(list.accessibilitySummary().contains(QStringLiteral("2 items")));
}

void TestQtMaterialList::singleSelectionTracksCurrentIndex()
{
    QtMaterialList list;
    auto* first = list.addItem(QStringLiteral("First"));
    auto* second = list.addItem(QStringLiteral("Second"));

    QSignalSpy spy(&list, &QtMaterialList::selectionChanged);

    list.setCurrentIndex(1);

    QVERIFY(!first->isSelected());
    QVERIFY(second->isSelected());
    QVERIFY(spy.count() >= 1);
}

void TestQtMaterialList::keyboardNavigationSkipsDisabledItems()
{
    QtMaterialList list;
    list.addItem(QStringLiteral("One"));
    list.addItem(QStringLiteral("Two"));
    list.addItem(QStringLiteral("Three"));
    list.setItemEnabled(1, false);
    list.setFocus();

    QTest::keyClick(&list, Qt::Key_Down);

    QCOMPARE(list.currentIndex(), 2);
}

void TestQtMaterialList::homeAndEndNavigateToEnabledBoundaries()
{
    QtMaterialList list;
    list.addItem(QStringLiteral("One"));
    list.addItem(QStringLiteral("Two"));
    list.addItem(QStringLiteral("Three"));
    list.setItemEnabled(0, false);
    list.setCurrentIndex(2);
    list.setFocus();

    QTest::keyClick(&list, Qt::Key_Home);
    QCOMPARE(list.currentIndex(), 1);

    QTest::keyClick(&list, Qt::Key_End);
    QCOMPARE(list.currentIndex(), 2);
}

void TestQtMaterialList::activationEmitsIndex()
{
    QtMaterialList list;
    list.addItem(QStringLiteral("One"));
    list.addItem(QStringLiteral("Two"));
    list.setCurrentIndex(1);
    list.setFocus();

    QSignalSpy spy(&list, &QtMaterialList::itemActivated);

    QTest::keyClick(&list, Qt::Key_Return);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toInt(), 1);
}

void TestQtMaterialList::multiSelectionTogglesActivatedRows()
{
    QtMaterialList list;
    list.addItem(QStringLiteral("One"));
    list.addItem(QStringLiteral("Two"));
    list.setSelectionMode(QtMaterialList::SelectionMode::MultiSelection);
    list.clearSelection();
    list.setCurrentIndex(1);
    list.setFocus();

    QSignalSpy spy(&list, &QtMaterialList::selectionChanged);

    QVERIFY(list.selectedIndexes().isEmpty());

    QTest::keyClick(&list, Qt::Key_Space);

    QCOMPARE(list.selectedIndexes(), QList<int>{1});
    QVERIFY(spy.count() >= 1);
}

void TestQtMaterialList::takeItemReparentsWithoutDeleting()
{
    QtMaterialList list;
    auto* item = list.addItem(QStringLiteral("Detached"));

    auto* taken = list.takeItem(0);

    QCOMPARE(taken, item);
    QCOMPARE(taken->parentWidget(), nullptr);
    QCOMPARE(list.count(), 0);

    delete taken;
}

QTEST_MAIN(TestQtMaterialList)
#include "tst_list.moc"
