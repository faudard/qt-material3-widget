#include <QtTest/QtTest>

#include <QAbstractTableModel>
#include <QHeaderView>
#include <QListView>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QWidget>

#include "qtmaterial/widgets/data/qtmaterialpagination.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/data/qtmaterialtreeview.h"
#include "qtmaterial/widgets/layouts/qtmaterialsplitview.h"
#include "qtmaterial/widgets/navigation/qtmaterialbreadcrumb.h"
#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"

using namespace QtMaterial;

namespace {

class LargeModel final : public QAbstractTableModel
{
public:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return parent.isValid() ? 0 : 100000;
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return parent.isValid() ? 0 : 3;
    }

    QVariant data(const QModelIndex& index, int role) const override
    {
        if (!index.isValid() || role != Qt::DisplayRole) {
            return QVariant();
        }
        return QStringLiteral("R%1 C%2").arg(index.row()).arg(index.column());
    }
};

} // namespace

class tst_DesktopProductivity : public QObject
{
    Q_OBJECT

private slots:
    void treeViewKeepsNativeModelViewContracts()
    {
        QStandardItemModel model;
        model.appendRow(new QStandardItem(QStringLiteral("Root")));

        QtMaterialTreeView tree;
        tree.setModel(&model);
        QCOMPARE(tree.model(), &model);
        QVERIFY(tree.uniformRowHeights());

        tree.setDense(true);
        QVERIFY(tree.dense());

        tree.setMultiSelectionEnabled(true);
        QVERIFY(tree.multiSelectionEnabled());

        tree.setDragDropEnabled(true);
        QVERIFY(tree.dragDropEnabled());
        QCOMPARE(tree.dragDropMode(), QAbstractItemView::InternalMove);
        QVERIFY(tree.showDropIndicator());
    }

    void treeViewReflectsNativeDragDropState()
    {
        QtMaterialTreeView tree;

        tree.setDragDropMode(QAbstractItemView::InternalMove);
        QVERIFY(tree.dragDropEnabled());

        tree.setDragDropMode(QAbstractItemView::NoDragDrop);
        QVERIFY(!tree.dragDropEnabled());
    }

    void largeModelDoesNotRequireMaterialization()
    {
        LargeModel model;
        QtMaterialTreeView tree;
        tree.setModel(&model);
        QCOMPARE(tree.model()->rowCount(), 100000);
        QVERIFY(tree.uniformRowHeights());
    }

    void tableDesktopPoliciesRemainNativeQt()
    {
        QStandardItemModel model(4, 3);
        QtMaterialTable table;
        table.setModel(&model);

        table.setColumnReorderingEnabled(true);
        QVERIFY(table.columnReorderingEnabled());
        QVERIFY(table.horizontalHeader()->sectionsMovable());

        table.setCellSelectionEnabled(true);
        QVERIFY(table.cellSelectionEnabled());
        QCOMPARE(table.selectionBehavior(), QAbstractItemView::SelectItems);

        table.setDragDropEnabled(true);
        QVERIFY(table.dragDropEnabled());
        QCOMPARE(table.dragDropMode(), QAbstractItemView::InternalMove);
    }

    void paginationClampsAndDescribesRange()
    {
        QtMaterialPagination pagination;
        pagination.setTotalCount(123);
        pagination.setPageSize(25);
        QCOMPARE(pagination.pageCount(), 5);

        pagination.setPage(3);
        QCOMPARE(pagination.page(), 3);
        QCOMPARE(pagination.rangeText(), QStringLiteral("51–75 / 123"));

        pagination.setPage(99);
        QCOMPARE(pagination.page(), 5);
        QCOMPARE(pagination.rangeText(), QStringLiteral("101–123 / 123"));
    }

    void paginationSignalsOnlyForActualChanges()
    {
        QtMaterialPagination pagination;
        pagination.setTotalCount(100);

        QSignalSpy pageChanged(&pagination, &QtMaterialPagination::pageChanged);
        QSignalSpy pageSizeChanged(&pagination, &QtMaterialPagination::pageSizeChanged);

        pagination.setPageSize(50);
        QCOMPARE(pageSizeChanged.count(), 1);
        QCOMPARE(pageChanged.count(), 0);
        QCOMPARE(pagination.page(), 1);

        pagination.setPage(2);
        QCOMPARE(pageChanged.count(), 1);

        pagination.setPageSize(100);
        QCOMPARE(pageSizeChanged.count(), 2);
        QCOMPARE(pageChanged.count(), 2);
        QCOMPARE(pagination.page(), 1);
    }

    void splitViewUsesQSplitterState()
    {
        QtMaterialSplitView split;
        split.addWidget(new QWidget);
        split.addWidget(new QWidget);
        split.setPaneCollapsible(0, true);
        QVERIFY(split.paneCollapsible(0));
        QCOMPARE(split.count(), 2);
    }

    void splitViewTracksCollapsedPanesIndependently()
    {
        QtMaterialSplitView split;
        split.resize(600, 200);

        split.addWidget(new QWidget);
        split.addWidget(new QWidget);
        split.addWidget(new QWidget);

        split.setPaneCollapsible(0, false);
        split.setPaneCollapsible(1, false);

        split.show();
        QVERIFY(QTest::qWaitForWindowExposed(&split));
        split.setSizes({200, 200, 200});
        QCoreApplication::processEvents();

        split.setPaneCollapsed(0, true);
        split.setPaneCollapsed(1, true);
        QVERIFY(split.paneCollapsed(0));
        QVERIFY(split.paneCollapsed(1));

        split.setPaneCollapsed(0, false);
        QVERIFY(!split.paneCollapsed(0));
        QVERIFY(split.paneCollapsed(1));
        QVERIFY(!split.paneCollapsible(0));

        split.setPaneCollapsed(1, false);
        QVERIFY(!split.paneCollapsed(1));
        QVERIFY(!split.paneCollapsible(1));
    }

    void breadcrumbTracksCurrentSegment()
    {
        QtMaterialBreadcrumb breadcrumb;
        breadcrumb.setItems({
            QStringLiteral("Workspace"),
            QStringLiteral("Requirements"),
            QStringLiteral("REQ-42")
        });
        QCOMPARE(breadcrumb.currentIndex(), 2);

        breadcrumb.setCurrentIndex(1);
        QCOMPARE(breadcrumb.currentIndex(), 1);
        QCOMPARE(breadcrumb.items().at(1), QStringLiteral("Requirements"));
    }

    void commandPaletteFiltersExternalModel()
    {
        QStringListModel model({
            QStringLiteral("Open file"),
            QStringLiteral("Build project"),
            QStringLiteral("Run tests")
        });

        QtMaterialCommandPalette palette;
        palette.setSourceModel(&model);
        QCOMPARE(palette.sourceModel(), &model);

        palette.setQuery(QStringLiteral("Build"));
        QCOMPARE(palette.resultView()->model()->rowCount(), 1);
    }
};

QTEST_MAIN(tst_DesktopProductivity)
#include "tst_desktop_productivity.moc"
