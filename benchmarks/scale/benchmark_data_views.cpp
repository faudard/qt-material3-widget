#include <QtTest/QtTest>

#include <QAbstractItemModel>
#include <QAbstractTableModel>
#include <QApplication>

#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/data/qtmaterialtreeview.h"

namespace {

constexpr int kRowCount = 100000;

class LargeTableModel final : public QAbstractTableModel
{
public:
    explicit LargeTableModel(QObject* parent = nullptr)
        : QAbstractTableModel(parent)
    {
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return parent.isValid() ? 0 : kRowCount;
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return parent.isValid() ? 0 : 8;
    }

    QVariant data(const QModelIndex& index, int role) const override
    {
        if (!index.isValid()) {
            return {};
        }
        if (role == Qt::DisplayRole || role == Qt::AccessibleTextRole) {
            return QStringLiteral("R%1 C%2").arg(index.row()).arg(index.column());
        }
        return {};
    }
};

class LargeFlatTreeModel final : public QAbstractItemModel
{
public:
    explicit LargeFlatTreeModel(QObject* parent = nullptr)
        : QAbstractItemModel(parent)
    {
    }

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override
    {
        if (parent.isValid() || row < 0 || row >= kRowCount || column != 0) {
            return {};
        }
        return createIndex(row, column);
    }

    QModelIndex parent(const QModelIndex&) const override
    {
        return {};
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return parent.isValid() ? 0 : kRowCount;
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return parent.isValid() ? 0 : 1;
    }

    QVariant data(const QModelIndex& index, int role) const override
    {
        if (!index.isValid()) {
            return {};
        }
        if (role == Qt::DisplayRole || role == Qt::AccessibleTextRole) {
            return QStringLiteral("Node %1").arg(index.row());
        }
        return {};
    }
};

} // namespace

class benchmark_DataViewsScale : public QObject
{
    Q_OBJECT

private slots:
    void table100kAttachAndTailNavigation()
    {
        LargeTableModel model;

        QBENCHMARK_ONCE {
            QtMaterial::QtMaterialTable table;
            table.setAttribute(Qt::WA_DontShowOnScreen);
            table.resize(1200, 720);
            table.setModel(&model);
            table.show();
            const QModelIndex tail = model.index(kRowCount - 1, 0);
            table.setCurrentIndex(tail);
            table.scrollTo(tail, QAbstractItemView::PositionAtBottom);
            QApplication::processEvents();
        }
    }

    void tree100kAttachAndTailNavigation()
    {
        LargeFlatTreeModel model;

        QBENCHMARK_ONCE {
            QtMaterial::QtMaterialTreeView tree;
            tree.setAttribute(Qt::WA_DontShowOnScreen);
            tree.resize(1200, 720);
            tree.setModel(&model);
            tree.show();
            const QModelIndex tail = model.index(kRowCount - 1, 0);
            tree.setCurrentIndex(tail);
            tree.scrollTo(tail, QAbstractItemView::PositionAtBottom);
            QApplication::processEvents();
        }
    }
};

QTEST_MAIN(benchmark_DataViewsScale)
#include "benchmark_data_views.moc"
