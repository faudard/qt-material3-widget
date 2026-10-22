#include "qtmaterial/widgets/data/qtmaterialmultisortfilterproxy.h"

#include <QAbstractItemModel>
#include <QSet>
#include <QStringList>

namespace QtMaterial {

QtMaterialMultiSortFilterProxy::QtMaterialMultiSortFilterProxy(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
}

QVector<QtMaterialMultiSortFilterProxy::SortKey>
QtMaterialMultiSortFilterProxy::sortKeys() const
{
    return m_sortKeys;
}

bool QtMaterialMultiSortFilterProxy::setSortKeys(const QVector<SortKey>& keys)
{
    QSet<int> seen;
    for (const SortKey& key : keys) {
        if (key.column < 0 || seen.contains(key.column)
            || (sourceModel() && key.column >= sourceModel()->columnCount())
            || (key.order != Qt::AscendingOrder && key.order != Qt::DescendingOrder)) {
            return false;
        }
        seen.insert(key.column);
    }

    m_sortKeys = keys;
    // The primary and secondary directions are handled in lessThan. Keep
    // the proxy's own global sort direction ascending to avoid double reversal.
    if (!m_sortKeys.isEmpty()) {
        // A change to secondary keys must re-sort even if Qt's primary
        // column/order pair has not changed since the previous call.
        invalidate();
        sort(0, Qt::AscendingOrder);
    } else {
        invalidate();
    }
    return true;
}

void QtMaterialMultiSortFilterProxy::clearSortKeys()
{
    if (m_sortKeys.isEmpty()) {
        return;
    }
    m_sortKeys.clear();
    invalidate();
}

QString QtMaterialMultiSortFilterProxy::searchQuery() const
{
    return m_searchQuery;
}

void QtMaterialMultiSortFilterProxy::setSearchQuery(const QString& query)
{
    const QString normalized = query.simplified();
    if (m_searchQuery == normalized) {
        return;
    }
    m_searchQuery = normalized;
    m_tokens = normalized.toCaseFolded().split(QLatin1Char(' '), Qt::SkipEmptyParts);
    invalidateFilter();
}

QVector<int> QtMaterialMultiSortFilterProxy::searchColumns() const
{
    return m_searchColumns;
}

bool QtMaterialMultiSortFilterProxy::setSearchColumns(const QVector<int>& columns)
{
    QSet<int> seen;
    for (int column : columns) {
        if (column < 0 || seen.contains(column)
            || (sourceModel() && column >= sourceModel()->columnCount())) {
            return false;
        }
        seen.insert(column);
    }
    m_searchColumns = columns;
    invalidateFilter();
    return true;
}

bool QtMaterialMultiSortFilterProxy::lessThan(
    const QModelIndex& left, const QModelIndex& right) const
{
    if (m_sortKeys.isEmpty()) {
        return QSortFilterProxyModel::lessThan(left, right);
    }

    const QAbstractItemModel* source = sourceModel();
    for (const SortKey& key : m_sortKeys) {
        if (!source || key.column >= source->columnCount(left.parent())
            || key.column >= source->columnCount(right.parent())) {
            continue;
        }
        const QModelIndex a = left.sibling(left.row(), key.column);
        const QModelIndex b = right.sibling(right.row(), key.column);
        if (QSortFilterProxyModel::lessThan(a, b)) {
            return key.order == Qt::AscendingOrder;
        }
        if (QSortFilterProxyModel::lessThan(b, a)) {
            return key.order == Qt::DescendingOrder;
        }
    }

    // Deterministic ordering of otherwise equal source siblings.
    return left.row() < right.row();
}

bool QtMaterialMultiSortFilterProxy::filterAcceptsRow(
    int sourceRow, const QModelIndex& sourceParent) const
{
    // Respect standard proxy filters if the application has configured them.
    if (!QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent)) {
        return false;
    }
    if (m_tokens.isEmpty()) {
        return true;
    }

    const QAbstractItemModel* source = sourceModel();
    if (!source) {
        return false;
    }

    const int count = source->columnCount(sourceParent);
    QStringList fields;
    if (m_searchColumns.isEmpty()) {
        for (int column = 0; column < count; ++column) {
            fields.append(source->index(sourceRow, column, sourceParent)
                              .data(filterRole()).toString().toCaseFolded());
        }
    } else {
        for (int column : m_searchColumns) {
            if (column < count) {
                fields.append(source->index(sourceRow, column, sourceParent)
                                  .data(filterRole()).toString().toCaseFolded());
            }
        }
    }

    for (const QString& token : m_tokens) {
        bool matched = false;
        for (const QString& field : fields) {
            if (field.contains(token)) {
                matched = true;
                break;
            }
        }
        if (!matched) {
            return false;
        }
    }
    return true;
}

} // namespace QtMaterial
