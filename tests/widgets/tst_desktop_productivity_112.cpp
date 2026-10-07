#include <QtTest/QtTest>

#include <memory>

#include <QAbstractItemView>
#include <QApplication>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLineEdit>
#include <QListView>
#include <QSignalSpy>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTimer>

#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/data/qtmaterialtreeview.h"
#include "qtmaterial/widgets/layouts/qtmaterialsplitview.h"
#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationsuite.h"

using namespace QtMaterial;

namespace {

std::unique_ptr<QStandardItemModel> makeFlatModel()
{
    auto model =
        std::make_unique<QStandardItemModel>(4, 2);
    model->setHorizontalHeaderLabels(
        {QStringLiteral("Name"), QStringLiteral("Value")});
    const QStringList names = {
        QStringLiteral("Gamma"),
        QStringLiteral("Alpha"),
        QStringLiteral("Delta"),
        QStringLiteral("Beta")
    };
    for (int row = 0; row < names.size(); ++row) {
        model->setData(
            model->index(row, 0),
            names.at(row));
        model->setData(
            model->index(row, 1),
            QStringLiteral("Value %1").arg(row));
    }
    return model;
}

class AsyncBulkProvider final : public QtMaterialCommandProvider
{
public:
    QList<quint64> requests;
    QList<quint64> cancelled;
    QStringList queries;

    void requestCommands(
        const QString& query,
        quint64 requestId) override
    {
        requests.push_back(requestId);
        queries.push_back(query);
    }

    void cancelRequest(quint64 requestId) override
    {
        cancelled.push_back(requestId);
    }

    void activateCommand(const QString&) override
    {
    }

    void enqueue(
        quint64 requestId,
        const QString& prefix,
        int count)
    {
        QList<QtMaterialCommand> commands;
        commands.reserve(count);
        for (int index = 0; index < count; ++index) {
            QtMaterialCommand command;
            command.id =
                QStringLiteral("%1.%2")
                    .arg(prefix)
                    .arg(index, 5, 10, QLatin1Char('0'));
            command.text =
                QStringLiteral("final command %1")
                    .arg(index);
            command.keywords = {
                QStringLiteral("final"),
                QStringLiteral("resource-%1").arg(index)
            };
            commands.push_back(command);
        }

        QTimer::singleShot(
            0,
            this,
            [this, requestId, commands]() {
                Q_EMIT commandsReady(
                    requestId,
                    commands);
            });
    }
};

} // namespace

class tst_DesktopProductivity112 : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void tableKeepsAdvancedKeyboardSelectionNative()
    {
        auto model = makeFlatModel();
        QtMaterialTable table;
        table.setModel(model.get());
        table.setMultiSelectionEnabled(true);
        table.resize(520, 260);
        table.show();
        QCoreApplication::processEvents();

        const QModelIndex first =
            model->index(0, 0);
        table.setCurrentIndex(first);
        table.selectionModel()->select(
            first,
            QItemSelectionModel::ClearAndSelect
                | QItemSelectionModel::Rows);

        QSignalSpy activated(
            &table,
            &QtMaterialTable::rowActivated);

        QTest::keyClick(
            &table,
            Qt::Key_Down,
            Qt::ShiftModifier);
        QCOMPARE(
            table.selectionModel()
                ->selectedRows()
                .size(),
            2);

        QTest::keyClick(
            &table,
            Qt::Key_Space,
            Qt::ControlModifier);
        QCOMPARE(activated.count(), 0);

        QTest::keyClick(
            &table,
            Qt::Key_Space);
        QCOMPARE(activated.count(), 1);
    }

    void treeKeepsAdvancedKeyboardSelectionAndExpansionNative()
    {
        QStandardItemModel model;
        auto* root =
            new QStandardItem(
                QStringLiteral("Root"));
        root->appendRow(
            new QStandardItem(
                QStringLiteral("Child")));
        model.appendRow(root);
        model.appendRow(
            new QStandardItem(
                QStringLiteral("Sibling")));

        QtMaterialTreeView tree;
        tree.setModel(model.get());
        tree.setMultiSelectionEnabled(true);
        tree.resize(420, 260);
        tree.show();
        QCoreApplication::processEvents();

        const QModelIndex rootIndex =
            model.index(0, 0);
        tree.setCurrentIndex(rootIndex);
        QVERIFY(!tree.isExpanded(rootIndex));

        QTest::keyClick(
            &tree,
            Qt::Key_Right);
        QVERIFY(tree.isExpanded(rootIndex));

        tree.selectionModel()->select(
            rootIndex,
            QItemSelectionModel::ClearAndSelect
                | QItemSelectionModel::Rows);
        QTest::keyClick(
            &tree,
            Qt::Key_Down,
            Qt::ShiftModifier);
        QVERIFY(
            tree.selectionModel()
                ->selectedRows()
                .size()
            >= 2);
    }

    void inlineEditingCommitsAndCancelsForTableAndTree()
    {
        QStandardItemModel tableModel(1, 1);
        tableModel.setData(
            tableModel.index(0, 0),
            QStringLiteral("Original"));

        QtMaterialTable table;
        table.setModel(&tableModel);
        table.setInlineEditingEnabled(true);
        QVERIFY(table.inlineEditingEnabled());
        table.resize(320, 160);
        table.show();
        QCoreApplication::processEvents();
        table.setCurrentIndex(
            tableModel.index(0, 0));

        QTest::keyClick(
            &table,
            Qt::Key_F2);
        QTRY_VERIFY(
            table.findChild<QLineEdit*>()
            != nullptr);
        QLineEdit* tableEditor =
            table.findChild<QLineEdit*>();
        QVERIFY(tableEditor);
        tableEditor->setText(
            QStringLiteral("Committed"));
        QTest::keyClick(
            tableEditor,
            Qt::Key_Return);
        QTRY_COMPARE(
            tableModel
                .data(tableModel.index(0, 0))
                .toString(),
            QStringLiteral("Committed"));
        QTRY_VERIFY(
            table.findChild<QLineEdit*>()
            == nullptr);

        QTest::keyClick(
            &table,
            Qt::Key_F2);
        QTRY_VERIFY(
            table.findChild<QLineEdit*>()
            != nullptr);
        tableEditor =
            table.findChild<QLineEdit*>();
        tableEditor->setText(
            QStringLiteral("Discarded"));
        QTest::keyClick(
            tableEditor,
            Qt::Key_Escape);
        QCOMPARE(
            tableModel
                .data(tableModel.index(0, 0))
                .toString(),
            QStringLiteral("Committed"));

        table.setInlineEditingEnabled(false);
        QVERIFY(!table.inlineEditingEnabled());

        QStandardItemModel treeModel;
        treeModel.appendRow(
            new QStandardItem(
                QStringLiteral("Tree Original")));

        QtMaterialTreeView tree;
        tree.setModel(&treeModel);
        tree.setInlineEditingEnabled(true);
        tree.resize(320, 160);
        tree.show();
        QCoreApplication::processEvents();
        tree.setCurrentIndex(
            treeModel.index(0, 0));

        QTest::keyClick(
            &tree,
            Qt::Key_F2);
        QTRY_VERIFY(
            tree.findChild<QLineEdit*>()
            != nullptr);
        QLineEdit* treeEditor =
            tree.findChild<QLineEdit*>();
        QVERIFY(treeEditor);
        treeEditor->setText(
            QStringLiteral("Tree Committed"));
        QTest::keyClick(
            treeEditor,
            Qt::Key_Return);
        QTRY_COMPARE(
            treeModel
                .data(treeModel.index(0, 0))
                .toString(),
            QStringLiteral("Tree Committed"));
        QTRY_VERIFY(
            tree.findChild<QLineEdit*>()
            == nullptr);

        QTest::keyClick(
            &tree,
            Qt::Key_F2);
        QTRY_VERIFY(
            tree.findChild<QLineEdit*>()
            != nullptr);
        treeEditor =
            tree.findChild<QLineEdit*>();
        treeEditor->setText(
            QStringLiteral("Tree Discarded"));
        QTest::keyClick(
            treeEditor,
            Qt::Key_Escape);
        QCOMPARE(
            treeModel
                .data(treeModel.index(0, 0))
                .toString(),
            QStringLiteral("Tree Committed"));
    }

    void sortFilterAndDragDropStayModelViewNative()
    {
        auto source = makeFlatModel();
        QSortFilterProxyModel proxy;
        proxy.setSourceModel(source.get());
        proxy.setFilterKeyColumn(0);

        QtMaterialTable table;
        table.setModel(&proxy);
        QCOMPARE(table.model(), &proxy);

        proxy.setFilterFixedString(
            QStringLiteral("Beta"));
        QCOMPARE(proxy.rowCount(), 1);
        QCOMPARE(
            proxy.index(0, 0)
                .data()
                .toString(),
            QStringLiteral("Beta"));

        proxy.setFilterFixedString(
            QString());
        table.sortByColumn(
            0,
            Qt::AscendingOrder);
        QCOMPARE(
            proxy.index(0, 0)
                .data()
                .toString(),
            QStringLiteral("Alpha"));

        table.setDragDropEnabled(true);
        QVERIFY(table.dragDropEnabled());
        QCOMPARE(
            table.dragDropMode(),
            QAbstractItemView::InternalMove);
        QCOMPARE(
            table.defaultDropAction(),
            Qt::MoveAction);
        QCOMPARE(table.model(), &proxy);

        QtMaterialTreeView tree;
        tree.setModel(&proxy);
        tree.setDragDropEnabled(true);
        QVERIFY(tree.dragDropEnabled());
        QCOMPARE(
            tree.dragDropMode(),
            QAbstractItemView::InternalMove);
        QCOMPARE(tree.model(), &proxy);
    }

    void contextMenuHooksExposeItemAndGlobalPosition()
    {
        auto model = makeFlatModel();

        QtMaterialTable table;
        table.setModel(model.get());
        table.setContextMenuEnabled(true);
        QVERIFY(table.contextMenuEnabled());
        table.resize(480, 240);
        table.show();
        QCoreApplication::processEvents();

        QSignalSpy tableMenu(
            &table,
            &QtMaterialTable::contextMenuRequested);
        const QModelIndex tableIndex =
            model->index(0, 0);
        const QPoint tablePosition =
            table.visualRect(tableIndex).center();
        QVERIFY(
            QMetaObject::invokeMethod(
                table.viewport(),
                "customContextMenuRequested",
                Qt::DirectConnection,
                Q_ARG(QPoint, tablePosition)));
        QCOMPARE(tableMenu.count(), 1);
        const QList<QVariant> tableArgs =
            tableMenu.takeFirst();
        QCOMPARE(
            qvariant_cast<QModelIndex>(
                tableArgs.at(0)),
            tableIndex);
        QCOMPARE(
            tableArgs.at(1).toPoint(),
            table.viewport()
                ->mapToGlobal(tablePosition));

        QtMaterialTreeView tree;
        tree.setModel(&model);
        tree.setContextMenuEnabled(true);
        QVERIFY(tree.contextMenuEnabled());
        tree.resize(480, 240);
        tree.show();
        QCoreApplication::processEvents();

        QSignalSpy treeMenu(
            &tree,
            &QtMaterialTreeView::contextMenuRequested);
        const QModelIndex treeIndex =
            model->index(0, 0);
        const QPoint treePosition =
            tree.visualRect(treeIndex).center();
        QVERIFY(
            QMetaObject::invokeMethod(
                tree.viewport(),
                "customContextMenuRequested",
                Qt::DirectConnection,
                Q_ARG(QPoint, treePosition)));
        QCOMPARE(treeMenu.count(), 1);
        const QList<QVariant> treeArgs =
            treeMenu.takeFirst();
        QCOMPARE(
            qvariant_cast<QModelIndex>(
                treeArgs.at(0)),
            treeIndex);
        QCOMPARE(
            treeArgs.at(1).toPoint(),
            tree.viewport()
                ->mapToGlobal(treePosition));
    }

    void splitViewRestoresExactSizesAcrossInstances()
    {
        QtMaterialSplitView source(
            Qt::Horizontal);
        source.resize(640, 240);
        source.addWidget(new QWidget);
        source.addWidget(new QWidget);
        source.show();
        QCoreApplication::processEvents();

        source.setSizes({190, 450});
        QCoreApplication::processEvents();
        const QList<int> expected =
            source.sizes();
        const QByteArray state =
            source.saveWorkspaceState();

        QtMaterialSplitView target(
            Qt::Horizontal);
        target.resize(640, 240);
        target.addWidget(new QWidget);
        target.addWidget(new QWidget);
        target.show();
        QCoreApplication::processEvents();

        QVERIFY(
            target.restoreWorkspaceState(
                state));
        QCoreApplication::processEvents();
        QCOMPARE(
            target.sizes(),
            expected);
    }

    void commandPaletteLargeAsyncCancellationKeepsLatestSnapshot()
    {
        AsyncBulkProvider provider;
        QtMaterialCommandPalette palette;
        palette.addProvider(&provider);
        palette.openPalette();
        QVERIFY(
            QTest::qWaitForWindowExposed(
                &palette));
        QTRY_VERIFY(
            !provider.requests.isEmpty());

        const quint64 initialRequest =
            provider.requests.last();

        palette.setQuery(
            QStringLiteral("first"));
        palette.refreshProviders();
        const quint64 firstRequest =
            provider.requests.last();
        QVERIFY(
            firstRequest
            != initialRequest);

        palette.setQuery(
            QStringLiteral("final"));
        palette.refreshProviders();
        const quint64 finalRequest =
            provider.requests.last();
        QVERIFY(
            finalRequest
            != firstRequest);

        QVERIFY(
            provider.cancelled.contains(
                initialRequest));
        QVERIFY(
            provider.cancelled.contains(
                firstRequest));

        QListView* results =
            palette.findChild<QListView*>();
        QVERIFY(results);

        provider.enqueue(
            firstRequest,
            QStringLiteral("stale"),
            6000);
        QCoreApplication::processEvents();
        QCOMPARE(
            results->model()->rowCount(),
            0);
        QVERIFY(palette.isLoading());

        provider.enqueue(
            finalRequest,
            QStringLiteral("final"),
            6000);
        QTRY_COMPARE(
            results->model()->rowCount(),
            6000);
        QTRY_VERIFY(!palette.isLoading());

        const QString firstId =
            results->model()
                ->index(0, 0)
                .data(
                    QtMaterialCommandPalette::
                        IdRole)
                .toString();
        QVERIFY(
            firstId.startsWith(
                QStringLiteral("final.")));
    }

    void navigationStateSurvivesRepeatedBarRailTransitions()
    {
        QtMaterialNavigationSuite suite;
        suite.addDestination(
            QStringLiteral("Home"));
        suite.addDestination(
            QStringLiteral("Search"));
        suite.addDestination(
            QStringLiteral("Disabled"));
        suite.addDestination(
            QStringLiteral("Settings"));
        suite.setDestinationEnabled(
            2,
            false);
        suite.setCurrentIndex(3);
        suite.resize(420, 100);
        suite.show();
        QCoreApplication::processEvents();
        suite.setFocus(
            Qt::OtherFocusReason);

        QSignalSpy currentChanged(
            &suite,
            &QtMaterialNavigationSuite::
                currentIndexChanged);

        const QStringList expectedTexts = {
            QStringLiteral("Home"),
            QStringLiteral("Search"),
            QStringLiteral("Disabled"),
            QStringLiteral("Settings")
        };

        for (int iteration = 0;
             iteration < 12;
             ++iteration) {
            suite.setWindowWidthSizeClass(
                WindowWidthSizeClass::Compact);
            QCOMPARE(
                suite.navigationType(),
                NavigationSuiteType::
                    NavigationBar);
            QCOMPARE(
                suite.currentIndex(),
                3);
            QVERIFY(
                !suite.isDestinationEnabled(
                    2));

            suite.setWindowWidthSizeClass(
                WindowWidthSizeClass::Medium);
            QCOMPARE(
                suite.navigationType(),
                NavigationSuiteType::
                    NavigationRail);
            QCOMPARE(
                suite.currentIndex(),
                3);
            QVERIFY(
                !suite.isDestinationEnabled(
                    2));

            suite.setWindowWidthSizeClass(
                WindowWidthSizeClass::Expanded);
            QCOMPARE(
                suite.currentIndex(),
                3);

            for (int index = 0;
                 index < suite.count();
                 ++index) {
                QCOMPARE(
                    suite.destinationText(
                        index),
                    expectedTexts.at(
                        index));
            }
        }

        QCOMPARE(
            currentChanged.count(),
            0);
        QVERIFY(suite.hasFocus());
        QVERIFY(
            suite.accessibilitySummary()
                .contains(
                    QStringLiteral(
                        "selected Settings")));
    }
};

QTEST_MAIN(tst_DesktopProductivity112)
#include "tst_desktop_productivity_112.moc"
