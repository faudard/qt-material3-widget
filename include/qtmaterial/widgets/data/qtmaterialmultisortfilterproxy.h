#pragma once

#include <QSortFilterProxyModel>
#include <QString>
#include <QStringList>
#include <QVector>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

/**
 * An optional, application-owned proxy for desktop Table/Tree workflows.
 *
 * Install it explicitly between an application model (or another proxy) and
 * a view. This class never owns or reparents the source model.
 *
 * Multi-column sorting is opt-in. With no keys the normal
 * QSortFilterProxyModel::sort()/lessThan() contract remains in effect.
 * When keys are set they determine the order; call setSortKeys() rather than
 * sortByColumn() to change the multi-column order.
 */
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialMultiSortFilterProxy
    : public QSortFilterProxyModel
{
public:
    struct SortKey {
        int column = 0;
        Qt::SortOrder order = Qt::AscendingOrder;
    };

    explicit QtMaterialMultiSortFilterProxy(QObject* parent = nullptr);

    QVector<SortKey> sortKeys() const;
    // Reject negative/duplicate/out-of-range columns without changing state.
    bool setSortKeys(const QVector<SortKey>& keys);
    void clearSortKeys();

    QString searchQuery() const;
    void setSearchQuery(const QString& query);

    // Empty means all columns; tokens may match across different columns.
    QVector<int> searchColumns() const;
    bool setSearchColumns(const QVector<int>& columns);

protected:
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    QVector<SortKey> m_sortKeys;
    QString m_searchQuery;
    QStringList m_tokens;
    QVector<int> m_searchColumns;
};

} // namespace QtMaterial
