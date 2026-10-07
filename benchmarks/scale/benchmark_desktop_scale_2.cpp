#include <QtTest/QtTest>

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QAbstractTableModel>
#include <QApplication>
#include <QColor>
#include <QPixmap>
#include <QPixmapCache>
#include <QSortFilterProxyModel>
#include <QWidget>

#include "qtmaterial/effects/private/qtmaterialshadowcache_p.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/data/qtmaterialtreeview.h"
#include "qtmaterial/widgets/layouts/qtmaterialadaptiveshell.h"
#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"
#include "qtmaterial/widgets/surfaces/qtmaterialsidesheet.h"

namespace {

constexpr int kRows = 1000000;

class MillionTableModel final : public QAbstractTableModel
{
public:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return parent.isValid() ? 0 : kRows;
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
            return QStringLiteral("row-%1 bucket-%2")
                .arg(index.row())
                .arg(index.row() % 1024, 4, 10, QLatin1Char('0'));
        }
        return {};
    }
};

class MillionTreeModel final : public QAbstractItemModel
{
public:
    QModelIndex index(
        int row,
        int column,
        const QModelIndex& parent = QModelIndex()) const override
    {
        if (parent.isValid() || row < 0 || row >= kRows || column != 0) {
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
        return parent.isValid() ? 0 : kRows;
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
            return QStringLiteral("node-%1").arg(index.row());
        }
        return {};
    }
};

class LazyMillionModel final : public QAbstractTableModel
{
public:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return parent.isValid() ? 0 : loaded_;
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return parent.isValid() ? 0 : 4;
    }

    QVariant data(const QModelIndex& index, int role) const override
    {
        if (!index.isValid()) {
            return {};
        }
        return role == Qt::DisplayRole
            ? QVariant(QStringLiteral("lazy-%1").arg(index.row()))
            : QVariant();
    }

    bool canFetchMore(const QModelIndex& parent) const override
    {
        return !parent.isValid() && loaded_ < kRows;
    }

    void fetchMore(const QModelIndex& parent) override
    {
        if (parent.isValid() || loaded_ >= kRows) {
            return;
        }
        const int count = qMin(50000, kRows - loaded_);
        beginInsertRows(QModelIndex(), loaded_, loaded_ + count - 1);
        loaded_ += count;
        endInsertRows();
    }

private:
    int loaded_ = 0;
};

void populateButtons(QWidget* host, int count)
{
    for (int i = 0; i < count; ++i) {
        new QtMaterial::QtMaterialFilledButton(
            QStringLiteral("Action %1").arg(i),
            host);
    }
}

} // namespace

class benchmark_DesktopScale2 : public QObject
{
    Q_OBJECT

private slots:
    void tableTreeMillionTailNavigation()
    {
        MillionTableModel tableModel;
        MillionTreeModel treeModel;

        QBENCHMARK_ONCE {
            QtMaterial::QtMaterialTable table;
            table.setAttribute(Qt::WA_DontShowOnScreen);
            table.resize(1280, 720);
            table.setModel(&tableModel);
            table.show();

            QtMaterial::QtMaterialTreeView tree;
            tree.setAttribute(Qt::WA_DontShowOnScreen);
            tree.resize(1280, 720);
            tree.setModel(&treeModel);
            tree.show();

            const QModelIndex tableTail = tableModel.index(kRows - 1, 0);
            const QModelIndex treeTail = treeModel.index(kRows - 1, 0);
            table.setCurrentIndex(tableTail);
            table.scrollTo(tableTail, QAbstractItemView::PositionAtBottom);
            tree.setCurrentIndex(treeTail);
            tree.scrollTo(treeTail, QAbstractItemView::PositionAtBottom);
            QApplication::processEvents();
        }
    }

    void lazyFetchMoreMillion()
    {
        QBENCHMARK_ONCE {
            LazyMillionModel model;
            QtMaterial::QtMaterialTable table;
            table.setAttribute(Qt::WA_DontShowOnScreen);
            table.setModel(&model);
            while (model.canFetchMore(QModelIndex())) {
                model.fetchMore(QModelIndex());
                QApplication::processEvents();
            }
            QCOMPARE(model.rowCount(), kRows);
        }
    }

    void proxyFilterMillion()
    {
        MillionTableModel source;
        QSortFilterProxyModel proxy;
        proxy.setSourceModel(&source);
        proxy.setFilterKeyColumn(0);
        proxy.setDynamicSortFilter(false);

        QBENCHMARK_ONCE {
            proxy.setFilterFixedString(QStringLiteral("bucket-0512"));
            const int matches = proxy.rowCount();
            QVERIFY(matches > 0);
        }
    }

    void themeStorm2000()
    {
        QWidget host;
        host.setAttribute(Qt::WA_DontShowOnScreen);
        populateButtons(&host, 64);
        host.show();
        QApplication::processEvents();

        auto& manager = QtMaterial::ThemeManager::instance();
        const QtMaterial::Theme original = manager.theme();

        QBENCHMARK_ONCE {
            for (int i = 0; i < 2000; ++i) {
                manager.applySeedColor(
                    QColor::fromHsv((i * 37) % 360, 160, 210),
                    (i % 2 == 0)
                        ? QtMaterial::ThemeMode::Light
                        : QtMaterial::ThemeMode::Dark);
                QApplication::processEvents();
            }
        }

        manager.setTheme(original);
    }

    void adaptiveResizeStorm5000()
    {
        QtMaterial::QtMaterialAdaptiveShell shell;
        shell.setAttribute(Qt::WA_DontShowOnScreen);
        shell.setContentWidget(new QWidget);
        shell.setSupportingWidget(new QWidget);
        shell.show();

        const int widths[] = {500, 599, 600, 700, 839, 840, 1000, 1200, 1600, 1700};

        QBENCHMARK_ONCE {
            for (int i = 0; i < 5000; ++i) {
                shell.resize(widths[i % 10], 720 + (i % 3));
                QApplication::processEvents();
            }
        }
    }

    void transientSurfaceLifecycle1000()
    {
        QWidget host;
        host.setAttribute(Qt::WA_DontShowOnScreen);
        host.resize(1024, 768);
        host.show();

        QBENCHMARK_ONCE {
            for (int i = 0; i < 500; ++i) {
                QtMaterial::QtMaterialDialog dialog(&host);
                dialog.setTitleText(QStringLiteral("Scale dialog"));
                dialog.open();
                dialog.close();

                QtMaterial::QtMaterialSideSheet sheet(&host);
                sheet.setTitleText(QStringLiteral("Scale side sheet"));
                sheet.open();
                sheet.closeSheet();
                QApplication::processEvents();
            }
        }
    }

    void shadowPixmapCache5000Hits()
    {
        QPixmapCache::clear();
        QtMaterial::QtMaterialShadowCache cache;
        QPixmap pixmap(64, 64);
        pixmap.setDevicePixelRatio(2.0);
        pixmap.fill(Qt::transparent);

        QStringList keys;
        for (int i = 0; i < 256; ++i) {
            const QString key = cache.keyFor(
                64 + (i % 8),
                64 + (i % 5),
                8.0 + (i % 4),
                12 + (i % 6),
                i % 4,
                QColor(0, 0, 0, 40 + (i % 80)),
                2.0);
            keys.push_back(key);
            cache.insert(key, pixmap);
        }

        QBENCHMARK_ONCE {
            for (int i = 0; i < 5000; ++i) {
                QPixmap out;
                QVERIFY(cache.find(keys.at(i % keys.size()), &out));
            }
        }
    }
};

QTEST_MAIN(benchmark_DesktopScale2)
#include "benchmark_desktop_scale_2.moc"
