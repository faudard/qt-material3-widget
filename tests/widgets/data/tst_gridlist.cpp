#include <QtTest/QtTest>

#include "qtmaterial/widgets/data/qtmaterialgridlist.h"

using QtMaterial::QtMaterialGridList;

class tst_GridList : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void constructionDefaults();
    void addItemsUpdatesTextAndAccessibility();
    void currentIndexSkipsDisabledItems();
    void selectionModes();
    void keyboardNavigation();
    void keyboardActivation();
    void columnsAndCellExtent();
};

void tst_GridList::constructionDefaults()
{
    QtMaterialGridList grid;
    QCOMPARE(grid.objectName(), QStringLiteral("QtMaterialGridList"));
    QCOMPARE(grid.viewMode(), QListView::IconMode);
    QCOMPARE(grid.gridSelectionMode(), QtMaterialGridList::SelectionMode::SingleSelection);
    QCOMPARE(grid.columns(), 3);
    QVERIFY(grid.accessibilitySummary().contains(QStringLiteral("0")));
}

void tst_GridList::addItemsUpdatesTextAndAccessibility()
{
    QtMaterialGridList grid;
    const int first = grid.addGridItem(QStringLiteral("Photos"), QStringLiteral("12 items"));
    const int second = grid.addGridItem(QStringLiteral("Videos"));

    QCOMPARE(first, 0);
    QCOMPARE(second, 1);
    QCOMPARE(grid.count(), 2);
    QCOMPARE(grid.itemTitle(0), QStringLiteral("Photos"));
    QCOMPARE(grid.itemSupportingText(0), QStringLiteral("12 items"));
    QVERIFY(grid.itemAccessibleText(0).contains(QStringLiteral("Photos")));
    QVERIFY(grid.itemAccessibleText(0).contains(QStringLiteral("12 items")));
    QVERIFY(grid.accessibilitySummary().contains(QStringLiteral("2")));
}

void tst_GridList::currentIndexSkipsDisabledItems()
{
    QtMaterialGridList grid;
    grid.addGridItem(QStringLiteral("One"));
    grid.addGridItem(QStringLiteral("Two"));
    grid.addGridItem(QStringLiteral("Three"));

    grid.setItemEnabled(1, false);
    grid.setCurrentIndex(1);
    QVERIFY(grid.currentIndex() != 1);

    grid.setCurrentIndex(0);
    QTest::keyClick(&grid, Qt::Key_Right);
    QCOMPARE(grid.currentIndex(), 2);
    QVERIFY(grid.itemAccessibleText(1).contains(QStringLiteral("Disabled")));
}

void tst_GridList::selectionModes()
{
    QtMaterialGridList grid;
    grid.addGridItem(QStringLiteral("One"));
    grid.addGridItem(QStringLiteral("Two"));
    grid.addGridItem(QStringLiteral("Three"));

    grid.setGridSelectionMode(QtMaterialGridList::SelectionMode::MultiSelection);
    grid.setItemSelected(0, true);
    grid.setItemSelected(2, true);

    const QList<int> selected = grid.selectedIndexesList();
    QCOMPARE(selected.size(), 2);
    QVERIFY(selected.contains(0));
    QVERIFY(selected.contains(2));

    grid.setGridSelectionMode(QtMaterialGridList::SelectionMode::NoSelection);
    QVERIFY(grid.selectedIndexesList().isEmpty());
}

void tst_GridList::keyboardNavigation()
{
    QtMaterialGridList grid;
    grid.setColumns(2);
    grid.addGridItem(QStringLiteral("One"));
    grid.addGridItem(QStringLiteral("Two"));
    grid.addGridItem(QStringLiteral("Three"));
    grid.addGridItem(QStringLiteral("Four"));

    grid.setCurrentIndex(0);
    QTest::keyClick(&grid, Qt::Key_Right);
    QCOMPARE(grid.currentIndex(), 1);

    QTest::keyClick(&grid, Qt::Key_Down);
    QCOMPARE(grid.currentIndex(), 3);

    QTest::keyClick(&grid, Qt::Key_Home);
    QCOMPARE(grid.currentIndex(), 0);

    QTest::keyClick(&grid, Qt::Key_End);
    QCOMPARE(grid.currentIndex(), 3);
}

void tst_GridList::keyboardActivation()
{
    QtMaterialGridList grid;
    grid.addGridItem(QStringLiteral("One"));
    grid.setCurrentIndex(0);

    QSignalSpy activatedSpy(&grid, &QtMaterialGridList::gridItemActivated);
    QTest::keyClick(&grid, Qt::Key_Return);

    QCOMPARE(activatedSpy.size(), 1);
    QCOMPARE(activatedSpy.takeFirst().at(0).toInt(), 0);
}

void tst_GridList::columnsAndCellExtent()
{
    QtMaterialGridList grid;

    QSignalSpy columnsSpy(&grid, &QtMaterialGridList::columnsChanged);
    grid.setColumns(4);
    QCOMPARE(grid.columns(), 4);
    QCOMPARE(columnsSpy.size(), 1);

    QSignalSpy cellSpy(&grid, &QtMaterialGridList::cellExtentChanged);
    grid.setCellExtent(QSize(180, 128));
    QCOMPARE(grid.cellExtent(), QSize(180, 128));
    QCOMPARE(cellSpy.size(), 1);
}

QTEST_MAIN(tst_GridList)
#include "tst_gridlist.moc"
