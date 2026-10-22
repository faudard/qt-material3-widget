#include <QtTest/QtTest>

#include <QAbstractTableModel>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>

#include "qtmaterial/widgets/data/qtmaterialmultisortfilterproxy.h"
#include "qtmaterial/widgets/data/qtmaterialselectionstate.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/data/qtmaterialtreeview.h"
#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"

using namespace QtMaterial;

namespace {

constexpr int kIdRole = Qt::UserRole + 42;

void fillTable(QStandardItemModel* model)
{
    model->setColumnCount(3);
    struct Record { const char* group; int priority; const char* status; const char* id; };
    const Record records[] = {
            {"B", 2, "open", "id-b"},
            {"A", 5, "closed", "id-a"},
            {"A", 9, "open", "id-c"},
            {"A", 5, "open", "id-d"}
        };
    model->setRowCount(4);
    for (int row = 0; row < 4; ++row) {
        model->setData(model->index(row, 0), QString::fromLatin1(records[row].group));
        model->setData(model->index(row, 1), records[row].priority);
        model->setData(model->index(row, 2), QString::fromLatin1(records[row].status));
        model->setData(model->index(row, 0),
                       QString::fromLatin1(records[row].id), kIdRole);
    }
}

class LazyTreeModel final : public QStandardItemModel
{
public:
    bool canFetchMore(const QModelIndex& parent) const override
    {
        return !parent.isValid();
    }

    void fetchMore(const QModelIndex&) override { ++fetches; }
    int fetches = 0;
};

class VirtualLargeTable final : public QAbstractTableModel
{
public:
    int rowCount(const QModelIndex& parent = {}) const override
    {
        return parent.isValid() ? 0 : 100000;
    }
    int columnCount(const QModelIndex& parent = {}) const override
    {
        return parent.isValid() ? 0 : 2;
    }
    QVariant data(const QModelIndex& index, int role) const override
    {
        if (!index.isValid()) {
            return {};
        }
        if (role == Qt::DisplayRole) {
            return index.column() == 0
                ? QVariant(index.row())
                : QVariant(index.row() % 16);
        }
        if (role == kIdRole && index.column() == 0) {
            return QStringLiteral("row-%1").arg(index.row());
        }
        return {};
    }
};

} // namespace

class tst_DesktopProductivity129 : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void multiColumnSortFilterKeepsExternalModels()
    {
        QStandardItemModel data;
        fillTable(&data);
        QtMaterialMultiSortFilterProxy proxy;
        proxy.setSourceModel(&data);
        QSortFilterProxyModel externalProxy;
        externalProxy.setSourceModel(&proxy);

        QtMaterialTable table;
        table.setSortingEnabled(false);
        table.setModel(&externalProxy);
        QCOMPARE(table.model(), static_cast<QAbstractItemModel*>(&externalProxy));
        QCOMPARE(proxy.sourceModel(), static_cast<QAbstractItemModel*>(&data));

        QVERIFY(proxy.setSortKeys({{0, Qt::AscendingOrder}, {1, Qt::DescendingOrder}}));
        QCOMPARE(proxy.mapToSource(proxy.index(0, 0)).row(), 2);
        QCOMPARE(proxy.mapToSource(proxy.index(1, 0)).row(), 1);
        QCOMPARE(proxy.mapToSource(proxy.index(2, 0)).row(), 3);
        QCOMPARE(proxy.mapToSource(proxy.index(3, 0)).row(), 0);

        const auto originalKeys = proxy.sortKeys();
        QVERIFY(!proxy.setSortKeys({{0, Qt::AscendingOrder}, {0, Qt::DescendingOrder}}));
        QCOMPARE(proxy.sortKeys().size(), originalKeys.size());
        QVERIFY(!proxy.setSortKeys({{-1, Qt::AscendingOrder}}));
        QVERIFY(!proxy.setSortKeys({{3, Qt::AscendingOrder}}));
        QVERIFY(!proxy.setSearchColumns({2, 2}));

        QVERIFY(proxy.setSearchColumns({0, 2}));
        proxy.setSearchQuery(QStringLiteral("a open"));
        QCOMPARE(proxy.rowCount(), 2);
        QCOMPARE(proxy.mapToSource(proxy.index(0, 0)).row(), 2);
        QCOMPARE(proxy.mapToSource(proxy.index(1, 0)).row(), 3);

        // Additional application-owned proxy filters compose as an intersection.
        externalProxy.setFilterKeyColumn(2);
        externalProxy.setFilterFixedString(QStringLiteral("closed"));
        QCOMPARE(externalProxy.rowCount(), 0);
        externalProxy.setFilterFixedString(QString());
        QCOMPARE(externalProxy.rowCount(), 2);

        data.setData(data.index(3, 2), QStringLiteral("closed"));
        QCOMPARE(proxy.rowCount(), 1);
        QCOMPARE(data.rowCount(), 4);
    }

    void stableSelectionSurvivesProxyResortAndRejectsStaleIDs()
    {
        QStandardItemModel data;
        fillTable(&data);
        QtMaterialMultiSortFilterProxy proxy;
        proxy.setSourceModel(&data);
        QSortFilterProxyModel outer;
        outer.setSourceModel(&proxy);
        QVERIFY(proxy.setSortKeys({{0, Qt::AscendingOrder}, {1, Qt::DescendingOrder}}));

        QtMaterialTable table;
        table.setSortingEnabled(false);
        table.setModel(&outer);
        const QModelIndex selected = outer.mapFromSource(proxy.mapFromSource(data.index(2, 0)));
        QVERIFY(selected.isValid());
        table.selectionModel()->select(
            selected, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        table.selectionModel()->setCurrentIndex(selected, QItemSelectionModel::NoUpdate);

        const QByteArray snapshot = QtMaterialSelectionState::save(&table, kIdRole);
        QVERIFY(!snapshot.isEmpty());

        QVERIFY(proxy.setSortKeys({{1, Qt::AscendingOrder}, {0, Qt::DescendingOrder}}));
        table.selectionModel()->clearSelection();
        table.selectionModel()->setCurrentIndex({}, QItemSelectionModel::NoUpdate);
        QVERIFY(QtMaterialSelectionState::restore(&table, snapshot, kIdRole));
        QCOMPARE(table.currentIndex().data(kIdRole).toString(), QStringLiteral("id-c"));
        QCOMPARE(table.selectionModel()->selectedRows().size(), 1);

        const QModelIndex before = table.currentIndex();
        const int countBefore = table.selectionModel()->selectedIndexes().size();
        proxy.setSearchQuery(QStringLiteral("b"));
        // Current item might be cleared by the filter; preserve the *new* state.
        const QModelIndex filteredCurrent = table.currentIndex();
        const int filteredCount = table.selectionModel()->selectedIndexes().size();
        QVERIFY(!QtMaterialSelectionState::restore(&table, snapshot, kIdRole));
        QCOMPARE(table.currentIndex(), filteredCurrent);
        QCOMPARE(table.selectionModel()->selectedIndexes().size(), filteredCount);
        QVERIFY(!QtMaterialSelectionState::restore(&table, snapshot.left(10), kIdRole));
        QCOMPARE(table.currentIndex(), filteredCurrent);
        Q_UNUSED(before);
        Q_UNUSED(countBefore);

        proxy.setSearchQuery(QString());
        data.setData(data.index(0, 0), QStringLiteral("id-c"), kIdRole);
        QVERIFY(!QtMaterialSelectionState::restore(&table, snapshot, kIdRole));
    }

    void treeSelectionUsesStableIDsWithoutFetching()
    {
        LazyTreeModel model;
        auto* root = new QStandardItem(QStringLiteral("Folder"));
        root->setData(QStringLiteral("folder"), kIdRole);
        auto* child = new QStandardItem(QStringLiteral("Child"));
        child->setData(QStringLiteral("child"), kIdRole);
        root->appendRow(child);
        model.appendRow(root);

        QtMaterialTreeView tree;
        tree.setModel(&model);
        tree.expand(model.index(0, 0));
        const QModelIndex current = model.index(0, 0, model.index(0, 0));
        tree.selectionModel()->select(current, QItemSelectionModel::ClearAndSelect);
        tree.selectionModel()->setCurrentIndex(current, QItemSelectionModel::NoUpdate);
        const QByteArray state = QtMaterialSelectionState::save(&tree, kIdRole);
        QVERIFY(!state.isEmpty());

        tree.selectionModel()->clearSelection();
        QVERIFY(QtMaterialSelectionState::restore(&tree, state, kIdRole));
        QCOMPARE(tree.currentIndex().data(kIdRole).toString(), QStringLiteral("child"));
        QCOMPARE(tree.selectionModel()->selectedIndexes().size(), 1);
        QCOMPARE(model.fetches, 0);
        QVERIFY(!QtMaterialSelectionState::restore(&tree, state, kIdRole, 1));
        QCOMPARE(model.fetches, 0);
    }

    void commandPaletteStateAndSourceOwnership()
    {
        QStandardItemModel source(2, 1);
        source.setData(source.index(0, 0), QStringLiteral("Open project"));
        source.setData(source.index(0, 0), QStringLiteral("open"), QtMaterialCommandPalette::IdRole);
        source.setData(source.index(1, 0), QStringLiteral("Save project"));
        source.setData(source.index(1, 0), QStringLiteral("save"), QtMaterialCommandPalette::IdRole);

        QtMaterialCommandPalette palette;
        palette.setSourceModel(&source);
        palette.setFavoriteCommandIds({QStringLiteral("open")});
        palette.setRecentCommandIds({QStringLiteral("save"), QStringLiteral("open")});
        const QByteArray state = palette.saveWorkspaceState();

        palette.setFavoriteCommandIds({});
        palette.clearRecentCommands();
        QVERIFY(palette.restoreWorkspaceState(state));
        QCOMPARE(palette.sourceModel(), static_cast<QAbstractItemModel*>(&source));
        QCOMPARE(palette.favoriteCommandIds(), QStringList({QStringLiteral("open")}));
        QCOMPARE(palette.recentCommandIds().first(), QStringLiteral("save"));
    }

    void largeProxyChainNavigatesWithoutCopyingSourceData()
    {
        VirtualLargeTable source;
        QtMaterialMultiSortFilterProxy sortProxy;
        sortProxy.setSourceModel(&source);
        QSortFilterProxyModel viewProxy;
        viewProxy.setSourceModel(&sortProxy);

        QtMaterialTable table;
        table.setSortingEnabled(false);
        table.setModel(&viewProxy);
        const QModelIndex last = source.index(source.rowCount() - 1, 0);
        const QModelIndex visible = viewProxy.mapFromSource(sortProxy.mapFromSource(last));
        QVERIFY(visible.isValid());
        table.setCurrentIndex(visible);
        QCOMPARE(table.currentIndex().data(kIdRole).toString(),
                 QStringLiteral("row-99999"));
        QCOMPARE(source.rowCount(), 100000);
    }
};

QTEST_MAIN(tst_DesktopProductivity129)
#include "tst_desktop_productivity_129.moc"
