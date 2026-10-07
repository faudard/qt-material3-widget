#include <QtTest/QtTest>

#include <QHeaderView>
#include <QStandardItemModel>

#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/data/qtmaterialtreeview.h"
#include "qtmaterial/widgets/layouts/qtmaterialsplitview.h"
#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationsuite.h"

using namespace QtMaterial;

namespace {

QStandardItemModel* makeTableModel(QObject* parent = nullptr)
{
    auto* model = new QStandardItemModel(4, 3, parent);
    model->setHorizontalHeaderLabels(
        {QStringLiteral("Name"),
         QStringLiteral("Role"),
         QStringLiteral("Status")});
    for (int row = 0; row < model->rowCount(); ++row) {
        model->setData(
            model->index(row, 0),
            QStringLiteral("Name %1").arg(row));
        model->setData(
            model->index(row, 1),
            QStringLiteral("Role %1").arg(row));
        model->setData(
            model->index(row, 2),
            QStringLiteral("Status %1").arg(row));
    }
    return model;
}

QStandardItemModel* makeTreeModel(QObject* parent = nullptr)
{
    auto* model = new QStandardItemModel(parent);
    model->setColumnCount(2);
    model->setHorizontalHeaderLabels(
        {QStringLiteral("Item"), QStringLiteral("Details")});

    auto* root = new QStandardItem(QStringLiteral("Workspace"));
    auto* rootDetails = new QStandardItem(QStringLiteral("Root details"));
    QList<QStandardItem*> rootRow;
    rootRow << root << rootDetails;
    model->appendRow(rootRow);

    auto* child = new QStandardItem(QStringLiteral("requirements.xml"));
    auto* childDetails = new QStandardItem(QStringLiteral("Requirements"));
    QList<QStandardItem*> childRow;
    childRow << child << childDetails;
    root->appendRow(childRow);

    auto* grandChild = new QStandardItem(QStringLiteral("REQ-42"));
    auto* grandChildDetails = new QStandardItem(QStringLiteral("Selected requirement"));
    QList<QStandardItem*> grandChildRow;
    grandChildRow << grandChild << grandChildDetails;
    child->appendRow(grandChildRow);

    return model;
}

} // namespace

class tst_DesktopWorkspaceState : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void tableRoundTripsPresentationAndPolicies()
    {
        QtMaterialTable table;
        auto model = std::unique_ptr<QStandardItemModel>(makeTableModel());
        table.setModel(model.get());

        table.setDense(true);
        table.setMultiSelectionEnabled(true);
        table.setColumnReorderingEnabled(true);
        table.setCellSelectionEnabled(true);
        table.setDragDropEnabled(true);
        table.setInlineEditingEnabled(false);
        table.setContextMenuEnabled(true);
        table.setSortingEnabled(true);
        table.sortByColumn(2, Qt::DescendingOrder);

        QHeaderView* header = table.horizontalHeader();
        header->resizeSection(1, 177);
        header->moveSection(header->visualIndex(0), 2);
        header->setSectionHidden(2, true);

        const int savedVisualIndex = header->visualIndex(0);
        const int savedSectionSize = header->sectionSize(1);
        const QByteArray state = table.saveWorkspaceState();
        QVERIFY(!state.isEmpty());

        table.setDense(false);
        table.setMultiSelectionEnabled(false);
        table.setColumnReorderingEnabled(false);
        table.setCellSelectionEnabled(false);
        table.setDragDropEnabled(false);
        table.setInlineEditingEnabled(true);
        table.setContextMenuEnabled(false);
        table.setSortingEnabled(false);
        header->resizeSection(1, 80);
        header->moveSection(header->visualIndex(0), 0);
        header->setSectionHidden(2, false);

        QVERIFY(table.restoreWorkspaceState(state));
        QVERIFY(table.dense());
        QVERIFY(table.multiSelectionEnabled());
        QVERIFY(table.columnReorderingEnabled());
        QVERIFY(table.cellSelectionEnabled());
        QVERIFY(table.dragDropEnabled());
        QVERIFY(!table.inlineEditingEnabled());
        QVERIFY(table.contextMenuEnabled());
        QVERIFY(table.isSortingEnabled());
        QCOMPARE(header->visualIndex(0), savedVisualIndex);
        QCOMPARE(header->sectionSize(1), savedSectionSize);
        QVERIFY(header->isSectionHidden(2));
        QCOMPARE(header->sortIndicatorSection(), 2);
        QCOMPARE(header->sortIndicatorOrder(), Qt::DescendingOrder);

        const QByteArray restored = table.saveWorkspaceState();
        QVERIFY(!table.restoreWorkspaceState(QByteArrayLiteral("broken")));
        QCOMPARE(table.saveWorkspaceState(), restored);
    }

    void tableRejectsWrongColumnTopology()
    {
        QtMaterialTable source;
        auto sourceModel =
            std::unique_ptr<QStandardItemModel>(makeTableModel());
        source.setModel(sourceModel.get());
        const QByteArray state = source.saveWorkspaceState();

        QtMaterialTable target;
        QStandardItemModel targetModel(2, 2);
        target.setModel(&targetModel);
        const QByteArray before = target.saveWorkspaceState();

        QVERIFY(!target.restoreWorkspaceState(state));
        QCOMPARE(target.saveWorkspaceState(), before);
    }

    void treeRoundTripsExpandedHierarchyAndCurrentItem()
    {
        QtMaterialTreeView tree;
        auto model = std::unique_ptr<QStandardItemModel>(makeTreeModel());
        tree.setModel(model.get());

        const QModelIndex root = model->index(0, 0);
        const QModelIndex child = model->index(0, 0, root);
        const QModelIndex grandChild =
            model->index(0, 1, child);

        tree.expand(root);
        tree.expand(child);
        tree.setCurrentIndex(grandChild);
        tree.setDense(true);
        tree.setMultiSelectionEnabled(true);
        tree.setDragDropEnabled(true);
        tree.setInlineEditingEnabled(false);
        tree.setContextMenuEnabled(true);

        QHeaderView* header = tree.header();
        header->resizeSection(0, 211);
        header->moveSection(header->visualIndex(0), 1);
        header->setSectionHidden(1, true);

        const int savedVisualIndex = header->visualIndex(0);
        const int savedSectionSize = header->sectionSize(0);
        const QByteArray state = tree.saveWorkspaceState();
        QVERIFY(!state.isEmpty());

        tree.collapseAll();
        tree.setCurrentIndex(QModelIndex());
        tree.setDense(false);
        tree.setMultiSelectionEnabled(false);
        tree.setDragDropEnabled(false);
        tree.setInlineEditingEnabled(true);
        tree.setContextMenuEnabled(false);
        header->resizeSection(0, 90);
        header->moveSection(header->visualIndex(0), 0);
        header->setSectionHidden(1, false);

        QVERIFY(tree.restoreWorkspaceState(state));
        QVERIFY(tree.isExpanded(root));
        QVERIFY(tree.isExpanded(child));
        QCOMPARE(tree.currentIndex(), grandChild);
        QVERIFY(tree.dense());
        QVERIFY(tree.multiSelectionEnabled());
        QVERIFY(tree.dragDropEnabled());
        QVERIFY(!tree.inlineEditingEnabled());
        QVERIFY(tree.contextMenuEnabled());
        QCOMPARE(header->visualIndex(0), savedVisualIndex);
        QCOMPARE(header->sectionSize(0), savedSectionSize);
        QVERIFY(header->isSectionHidden(1));
        QVERIFY(
            tree.currentItemAccessibleText().contains(
                QStringLiteral("Selected requirement")));

        const QByteArray restored = tree.saveWorkspaceState();
        QVERIFY(!tree.restoreWorkspaceState(QByteArrayLiteral("truncated")));
        QCOMPARE(tree.saveWorkspaceState(), restored);
    }

    void treeRejectsIncompatibleTopology()
    {
        QtMaterialTreeView source;
        auto sourceModel =
            std::unique_ptr<QStandardItemModel>(makeTreeModel());
        source.setModel(sourceModel.get());
        source.expand(sourceModel->index(0, 0));
        source.setCurrentIndex(
            sourceModel->index(
                0,
                0,
                sourceModel->index(0, 0)));
        const QByteArray state = source.saveWorkspaceState();

        QtMaterialTreeView target;
        QStandardItemModel targetModel(1, 2);
        target.setModel(&targetModel);
        const QByteArray before = target.saveWorkspaceState();

        QVERIFY(!target.restoreWorkspaceState(state));
        QCOMPARE(target.saveWorkspaceState(), before);
    }

    void commandPaletteRoundTripsUserPreferences()
    {
        QtMaterialCommandPalette palette;
        palette.setFavoriteCommandIds(
            {QStringLiteral("open"),
             QStringLiteral("build"),
             QStringLiteral("open")});
        palette.setRecentCommandIds(
            {QStringLiteral("build"),
             QStringLiteral("search"),
             QStringLiteral("open")});
        palette.setFuzzyMatchingEnabled(false);

        const QByteArray state = palette.saveWorkspaceState();
        QVERIFY(!state.isEmpty());

        palette.setFavoriteCommandIds({QStringLiteral("other")});
        palette.clearRecentCommands();
        palette.setFuzzyMatchingEnabled(true);

        QVERIFY(palette.restoreWorkspaceState(state));
        QCOMPARE(
            palette.favoriteCommandIds(),
            (QStringList{
                QStringLiteral("open"),
                QStringLiteral("build")}));
        QCOMPARE(
            palette.recentCommandIds(),
            (QStringList{
                QStringLiteral("build"),
                QStringLiteral("search"),
                QStringLiteral("open")}));
        QVERIFY(!palette.fuzzyMatchingEnabled());

        const QByteArray restored = palette.saveWorkspaceState();
        QVERIFY(!palette.restoreWorkspaceState(QByteArrayLiteral("bad")));
        QCOMPARE(palette.saveWorkspaceState(), restored);
    }

    void navigationSuiteRoundTripsAdaptiveState()
    {
        QtMaterialNavigationSuite suite;
        suite.addDestination(QStringLiteral("Home"));
        suite.addDestination(QStringLiteral("Projects"));
        suite.addDestination(QStringLiteral("Settings"));
        suite.setDestinationEnabled(1, false);
        suite.setCurrentIndex(2);
        suite.setWindowWidthSizeClass(WindowWidthSizeClass::Expanded);

        const QByteArray state = suite.saveWorkspaceState();
        QVERIFY(!state.isEmpty());

        suite.setDestinationEnabled(1, true);
        suite.setCurrentIndex(0);
        suite.setWindowWidthSizeClass(WindowWidthSizeClass::Compact);

        QVERIFY(suite.restoreWorkspaceState(state));
        QVERIFY(!suite.isDestinationEnabled(1));
        QCOMPARE(suite.currentIndex(), 2);
        QCOMPARE(
            suite.windowWidthSizeClass(),
            WindowWidthSizeClass::Expanded);
        QCOMPARE(
            suite.navigationType(),
            NavigationSuiteType::NavigationRail);

        const QByteArray restored = suite.saveWorkspaceState();
        QVERIFY(!suite.restoreWorkspaceState(QByteArrayLiteral("broken")));
        QCOMPARE(suite.saveWorkspaceState(), restored);
    }

    void navigationSuiteRejectsDifferentDestinationModel()
    {
        QtMaterialNavigationSuite source;
        source.addDestination(QStringLiteral("Home"));
        source.addDestination(QStringLiteral("Settings"));
        source.setCurrentIndex(1);
        const QByteArray state = source.saveWorkspaceState();

        QtMaterialNavigationSuite target;
        target.addDestination(QStringLiteral("Home"));
        target.addDestination(QStringLiteral("Profile"));
        target.setCurrentIndex(0);
        const QByteArray before = target.saveWorkspaceState();

        QVERIFY(!target.restoreWorkspaceState(state));
        QCOMPARE(target.saveWorkspaceState(), before);
    }

    void splitViewUsesUniformWorkspaceStateNaming()
    {
        QtMaterialSplitView split(Qt::Horizontal);
        split.resize(640, 240);
        split.addWidget(new QWidget);
        split.addWidget(new QWidget);
        split.show();
        QVERIFY(QTest::qWaitForWindowExposed(&split));

        split.setSizes({220, 420});
        QCoreApplication::processEvents();
        split.setPaneCollapsed(0, true);
        QVERIFY(split.paneCollapsed(0));

        const QByteArray state = split.saveWorkspaceState();
        QCOMPARE(state, split.savePaneState());

        split.setPaneCollapsed(0, false);
        split.setSizes({320, 320});
        QCoreApplication::processEvents();
        QVERIFY(!split.paneCollapsed(0));

        QVERIFY(split.restoreWorkspaceState(state));
        QVERIFY(split.paneCollapsed(0));

        const QByteArray restored = split.saveWorkspaceState();
        QVERIFY(!split.restoreWorkspaceState(QByteArrayLiteral("invalid")));
        QCOMPARE(split.saveWorkspaceState(), restored);
    }
};

QTEST_MAIN(tst_DesktopWorkspaceState)
#include "tst_desktop_workspace_state.moc"
