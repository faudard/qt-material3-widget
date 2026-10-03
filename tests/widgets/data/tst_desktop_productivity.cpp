#include <QtTest/QtTest>

#include <limits>

#include <QAbstractTableModel>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QPixmap>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QToolButton>
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

    void paginationHandlesIntLimitWithoutOverflow()
    {
        QtMaterialPagination pagination;
        const int maximum = std::numeric_limits<int>::max();

        pagination.setTotalCount(maximum);
        pagination.setPageSize(maximum - 1);
        QCOMPARE(pagination.pageCount(), 2);

        pagination.setPage(2);
        QCOMPARE(
            pagination.rangeText(),
            QStringLiteral("2147483647–2147483647 / 2147483647"));
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

    void treeViewKeyboardRtlAndHighDpi()
    {
        QStandardItemModel model;
        auto* root = new QStandardItem(QStringLiteral("Root"));
        root->appendRow(new QStandardItem(QStringLiteral("Child")));
        model.appendRow(root);

        QtMaterialTreeView tree;
        tree.setModel(&model);
        tree.expand(model.index(0, 0));
        tree.setCurrentIndex(model.index(0, 0));
        tree.setLayoutDirection(Qt::RightToLeft);
        tree.resize(360, 240);

        QCOMPARE(tree.accessibleName(), QStringLiteral("Tree"));
        QCOMPARE(tree.layoutDirection(), Qt::RightToLeft);

        tree.show();
        QVERIFY(QTest::qWaitForWindowExposed(&tree));
        tree.setFocus(Qt::OtherFocusReason);
        QTRY_VERIFY(tree.hasFocus());

        QTest::keyClick(&tree, Qt::Key_Down);
        QCOMPARE(tree.currentIndex(), model.index(0, 0, model.index(0, 0)));

        QPixmap pixmap(
            tree.width() * 2,
            tree.height() * 2);
        pixmap.setDevicePixelRatio(2.0);
        pixmap.fill(Qt::transparent);
        tree.render(&pixmap);

        QVERIFY(!pixmap.isNull());
        QCOMPARE(pixmap.devicePixelRatio(), qreal(2.0));
    }

    void paginationAccessibilityKeyboardRtlAndHighDpi()
    {
        QtMaterialPagination pagination;
        pagination.setTotalCount(123);
        pagination.setPageSize(25);
        pagination.setPage(2);

        QCOMPARE(pagination.accessibleName(), QStringLiteral("Pagination"));
        QVERIFY(pagination.accessibleDescription().contains(QStringLiteral("Page 2 of 5")));
        QVERIFY(pagination.accessibleDescription().contains(QStringLiteral("26–50 / 123")));

        QToolButton* firstButton = nullptr;
        QToolButton* previousButton = nullptr;
        QToolButton* nextButton = nullptr;
        QToolButton* lastButton = nullptr;
        const auto buttons = pagination.findChildren<QToolButton*>();
        for (QToolButton* button : buttons) {
            if (button->accessibleName() == QStringLiteral("First page")) {
                firstButton = button;
            } else if (button->accessibleName() == QStringLiteral("Previous page")) {
                previousButton = button;
            } else if (button->accessibleName() == QStringLiteral("Next page")) {
                nextButton = button;
            } else if (button->accessibleName() == QStringLiteral("Last page")) {
                lastButton = button;
            }
        }
        QVERIFY(firstButton);
        QVERIFY(previousButton);
        QVERIFY(nextButton);
        QVERIFY(lastButton);

        pagination.setLayoutDirection(Qt::RightToLeft);
        QCOMPARE(firstButton->text(), QStringLiteral("»"));
        QCOMPARE(previousButton->text(), QStringLiteral("›"));
        QCOMPARE(nextButton->text(), QStringLiteral("‹"));
        QCOMPARE(lastButton->text(), QStringLiteral("«"));

        pagination.resize(560, qMax(48, pagination.sizeHint().height()));
        pagination.show();
        QVERIFY(QTest::qWaitForWindowExposed(&pagination));

        nextButton->setFocus(Qt::OtherFocusReason);
        QTRY_VERIFY(nextButton->hasFocus());
        QTest::keyClick(nextButton, Qt::Key_Space);
        QCOMPARE(pagination.page(), 3);
        QVERIFY(pagination.accessibleDescription().contains(QStringLiteral("Page 3 of 5")));

        QPixmap pixmap(
            pagination.width() * 2,
            pagination.height() * 2);
        pixmap.setDevicePixelRatio(2.0);
        pixmap.fill(Qt::transparent);
        pagination.render(&pixmap);

        QVERIFY(!pixmap.isNull());
        QCOMPARE(pixmap.devicePixelRatio(), qreal(2.0));
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

    void breadcrumbActivationDoesNotRebuildSender()
    {
        QtMaterialBreadcrumb breadcrumb;
        breadcrumb.setItems({
            QStringLiteral("Workspace"),
            QStringLiteral("Requirements"),
            QStringLiteral("REQ-42")
        });

        QToolButton* workspaceButton = nullptr;
        const auto buttons = breadcrumb.findChildren<QToolButton*>();
        for (QToolButton* button : buttons) {
            if (button->text() == QStringLiteral("Workspace")) {
                workspaceButton = button;
                break;
            }
        }
        QVERIFY(workspaceButton);

        QSignalSpy activated(&breadcrumb, &QtMaterialBreadcrumb::activated);
        workspaceButton->click();

        QCOMPARE(breadcrumb.currentIndex(), 0);
        QCOMPARE(activated.count(), 1);
        QCOMPARE(activated.at(0).at(0).toInt(), 0);
        QCOMPARE(activated.at(0).at(1).toString(), QStringLiteral("Workspace"));
        QCOMPARE(
            breadcrumb.findChildren<QToolButton*>().size(),
            buttons.size());
    }

    void breadcrumbAccessibilityRtlAndKeyboard()
    {
        QtMaterialBreadcrumb breadcrumb;
        breadcrumb.setItems({
            QStringLiteral("Workspace"),
            QStringLiteral("Requirements"),
            QStringLiteral("REQ-42")
        });

        QCOMPARE(breadcrumb.accessibleName(), QStringLiteral("Breadcrumb"));
        QVERIFY(
            breadcrumb.accessibleDescription().contains(
                QStringLiteral("Workspace / Requirements / REQ-42")));

        auto buttons = breadcrumb.findChildren<QToolButton*>();
        QCOMPARE(buttons.size(), 3);

        QToolButton* workspaceButton = nullptr;
        QToolButton* requirementsButton = nullptr;
        QToolButton* currentButton = nullptr;
        for (QToolButton* button : buttons) {
            if (button->text() == QStringLiteral("Workspace")) {
                workspaceButton = button;
            } else if (button->text() == QStringLiteral("Requirements")) {
                requirementsButton = button;
            } else if (button->text() == QStringLiteral("REQ-42")) {
                currentButton = button;
            }
        }
        QVERIFY(workspaceButton);
        QVERIFY(requirementsButton);
        QVERIFY(currentButton);
        QVERIFY(workspaceButton->accessibleDescription().contains(QStringLiteral("1 of 3")));
        QVERIFY(requirementsButton->accessibleDescription().contains(QStringLiteral("2 of 3")));
        QVERIFY(currentButton->accessibleDescription().contains(QStringLiteral("current location")));
        QVERIFY(!currentButton->isEnabled());

        breadcrumb.setLayoutDirection(Qt::RightToLeft);
        const auto labels = breadcrumb.findChildren<QLabel*>();
        QCOMPARE(labels.size(), 2);
        for (QLabel* label : labels) {
            QCOMPARE(label->text(), QStringLiteral("‹"));
        }

        breadcrumb.resize(520, breadcrumb.sizeHint().height());
        breadcrumb.show();
        QVERIFY(QTest::qWaitForWindowExposed(&breadcrumb));

        QSignalSpy activated(&breadcrumb, &QtMaterialBreadcrumb::activated);
        workspaceButton->setFocus(Qt::OtherFocusReason);
        QTRY_VERIFY(workspaceButton->hasFocus());
        QTest::keyClick(workspaceButton, Qt::Key_Space);
        QCOMPARE(activated.count(), 1);
        QCOMPARE(activated.at(0).at(0).toInt(), 0);
    }

    void breadcrumbRendersAtHighDpi()
    {
        QtMaterialBreadcrumb breadcrumb;
        breadcrumb.setItems({
            QStringLiteral("Workspace"),
            QStringLiteral("Requirements"),
            QStringLiteral("REQ-42")
        });
        breadcrumb.resize(520, qMax(48, breadcrumb.sizeHint().height()));

        QPixmap pixmap(
            breadcrumb.width() * 2,
            breadcrumb.height() * 2);
        pixmap.setDevicePixelRatio(2.0);
        pixmap.fill(Qt::transparent);
        breadcrumb.render(&pixmap);

        QVERIFY(!pixmap.isNull());
        QCOMPARE(pixmap.devicePixelRatio(), qreal(2.0));
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

        auto* resultView = palette.findChild<QListView*>();
        QVERIFY(resultView);
        QCOMPARE(resultView->model()->rowCount(), 1);
    }

    void commandPaletteKeyboardAccessibilityAndRtl()
    {
        QStringListModel model({
            QStringLiteral("Open file"),
            QStringLiteral("Build project"),
            QStringLiteral("Run tests")
        });

        QtMaterialCommandPalette palette;
        palette.setSourceModel(&model);
        QCOMPARE(palette.accessibleName(), QStringLiteral("Command palette"));
        QVERIFY(palette.accessibleDescription().contains(QStringLiteral("3")));

        auto* searchEdit = palette.findChild<QLineEdit*>();
        auto* resultView = palette.findChild<QListView*>();
        QVERIFY(searchEdit);
        QVERIFY(resultView);
        QCOMPARE(searchEdit->accessibleName(), QStringLiteral("Search commands"));
        QCOMPARE(resultView->accessibleName(), QStringLiteral("Command results"));
        QCOMPARE(resultView->currentIndex().row(), 0);

        palette.setLayoutDirection(Qt::RightToLeft);
        QCOMPARE(searchEdit->layoutDirection(), Qt::RightToLeft);
        QCOMPARE(resultView->layoutDirection(), Qt::RightToLeft);

        palette.show();
        QVERIFY(QTest::qWaitForWindowExposed(&palette));
        searchEdit->setFocus(Qt::OtherFocusReason);
        QTRY_VERIFY(searchEdit->hasFocus());

        QTest::keyClick(searchEdit, Qt::Key_Down);
        QCOMPARE(resultView->currentIndex().row(), 1);
        QTest::keyClick(searchEdit, Qt::Key_End);
        QCOMPARE(resultView->currentIndex().row(), 2);
        QTest::keyClick(searchEdit, Qt::Key_Home);
        QCOMPARE(resultView->currentIndex().row(), 0);
        QTest::keyClick(searchEdit, Qt::Key_Up);
        QCOMPARE(resultView->currentIndex().row(), 2);

        palette.setQuery(QStringLiteral("Build"));
        QCOMPARE(resultView->model()->rowCount(), 1);
        QCOMPARE(resultView->currentIndex().row(), 0);
        QVERIFY(palette.accessibleDescription().contains(QStringLiteral("1")));

        palette.setQuery(QStringLiteral("No match"));
        QCOMPARE(resultView->model()->rowCount(), 0);
        QVERIFY(!resultView->currentIndex().isValid());
        QVERIFY(palette.accessibleDescription().contains(QStringLiteral("0")));

        palette.setQuery(QString());
        QTest::keyClick(searchEdit, Qt::Key_Escape);
        QVERIFY(!palette.isVisible());
    }

    void commandPaletteKeyboardActivationAndHighDpi()
    {
        QStringListModel model({
            QStringLiteral("Open file"),
            QStringLiteral("Build project")
        });

        QtMaterialCommandPalette palette;
        palette.setSourceModel(&model);
        auto* searchEdit = palette.findChild<QLineEdit*>();
        auto* resultView = palette.findChild<QListView*>();
        QVERIFY(searchEdit);
        QVERIFY(resultView);

        palette.resize(560, 400);
        QPixmap pixmap(
            palette.width() * 2,
            palette.height() * 2);
        pixmap.setDevicePixelRatio(2.0);
        pixmap.fill(Qt::transparent);
        palette.render(&pixmap);
        QVERIFY(!pixmap.isNull());
        QCOMPARE(pixmap.devicePixelRatio(), qreal(2.0));

        QSignalSpy activated(&palette, &QtMaterialCommandPalette::commandActivated);
        palette.show();
        QVERIFY(QTest::qWaitForWindowExposed(&palette));
        searchEdit->setFocus(Qt::OtherFocusReason);
        QTRY_VERIFY(searchEdit->hasFocus());

        QTest::keyClick(searchEdit, Qt::Key_Down);
        QCOMPARE(resultView->currentIndex().row(), 1);
        QTest::keyClick(searchEdit, Qt::Key_Return);

        QCOMPARE(activated.count(), 1);
        QCOMPARE(activated.at(0).at(0).value<QModelIndex>().row(), 1);
        QVERIFY(!palette.isVisible());
    }

    void commandPaletteUsesSingleNativeActivationPath()
    {
        QStringListModel model({
            QStringLiteral("Open file"),
            QStringLiteral("Build project")
        });

        QtMaterialCommandPalette palette;
        palette.setSourceModel(&model);

        auto* resultView = palette.findChild<QListView*>();
        QVERIFY(resultView);
        const QModelIndex proxyIndex = resultView->model()->index(0, 0);
        QVERIFY(proxyIndex.isValid());

        QSignalSpy activated(&palette, &QtMaterialCommandPalette::commandActivated);

        palette.show();
        QVERIFY(QTest::qWaitForWindowExposed(&palette));

        QVERIFY(QMetaObject::invokeMethod(
            resultView,
            "activated",
            Qt::DirectConnection,
            Q_ARG(QModelIndex, proxyIndex)));
        QCOMPARE(activated.count(), 1);
        QVERIFY(!palette.isVisible());

        // Double-click is intentionally not a second activation path.
        QVERIFY(QMetaObject::invokeMethod(
            resultView,
            "doubleClicked",
            Qt::DirectConnection,
            Q_ARG(QModelIndex, proxyIndex)));
        QCOMPARE(activated.count(), 1);
    }
};

QTEST_MAIN(tst_DesktopProductivity)
#include "tst_desktop_productivity.moc"
