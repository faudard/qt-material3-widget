#include "qtmaterial/core/qtmaterialnavigationmodel.h"

namespace QtMaterial {

QtMaterialNavigationModel::QtMaterialNavigationModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

QtMaterialNavigationModel::~QtMaterialNavigationModel() = default;

int QtMaterialNavigationModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return m_items.size();
}

QVariant QtMaterialNavigationModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size()) return QVariant();
    const QtMaterialNavigationItem& item = m_items.at(index.row());
    switch (role) {
    case Qt::DisplayRole:
    case LabelRole: return item.label;
    case IdRole: return item.id;
    case RouteRole: return item.route;
    case IconRole: return item.icon;
    case TestIdRole: return item.testId;
    case EnabledRole: return item.enabled;
    case SelectedRole: return item.selected;
    default: return QVariant();
    }
}

QHash<int, QByteArray> QtMaterialNavigationModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[IdRole] = "id";
    roles[RouteRole] = "route";
    roles[LabelRole] = "label";
    roles[IconRole] = "icon";
    roles[TestIdRole] = "testId";
    roles[EnabledRole] = "enabled";
    roles[SelectedRole] = "selected";
    return roles;
}

QVector<QtMaterialNavigationItem> QtMaterialNavigationModel::items() const { return m_items; }

void QtMaterialNavigationModel::setItems(const QVector<QtMaterialNavigationItem>& items)
{
    beginResetModel();
    m_items = items;
    bool foundSelected = false;
    for (QtMaterialNavigationItem& item : m_items) {
        if (item.selected && !foundSelected) {
            foundSelected = true;
        } else {
            item.selected = false;
        }
    }
    endResetModel();
    emit selectedIdChanged(selectedId());
    emit selectedRouteChanged(selectedRoute());
}

void QtMaterialNavigationModel::addItem(const QtMaterialNavigationItem& item)
{
    const int row = m_items.size();
    beginInsertRows(QModelIndex(), row, row);
    QtMaterialNavigationItem copy = item;
    if (copy.selected) {
        for (QtMaterialNavigationItem& existing : m_items) existing.selected = false;
    }
    m_items.append(copy);
    endInsertRows();
    if (copy.selected) {
        emit selectedIdChanged(copy.id);
        emit selectedRouteChanged(copy.route);
    }
}

QtMaterialNavigationItem QtMaterialNavigationModel::itemAt(int row) const
{
    if (row < 0 || row >= m_items.size()) return QtMaterialNavigationItem();
    return m_items.at(row);
}

QString QtMaterialNavigationModel::selectedId() const
{
    for (const QtMaterialNavigationItem& item : m_items) {
        if (item.selected) return item.id;
    }
    return QString();
}

QString QtMaterialNavigationModel::selectedRoute() const
{
    for (const QtMaterialNavigationItem& item : m_items) {
        if (item.selected) return item.route;
    }
    return QString();
}

bool QtMaterialNavigationModel::setSelectedId(const QString& id)
{
    return setSelectedRow(findRowById(id));
}

bool QtMaterialNavigationModel::setSelectedRoute(const QString& route)
{
    return setSelectedRow(findRowByRoute(route));
}

int QtMaterialNavigationModel::findRowById(const QString& id) const
{
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items.at(i).id == id) return i;
    }
    return -1;
}

int QtMaterialNavigationModel::findRowByRoute(const QString& route) const
{
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items.at(i).route == route) return i;
    }
    return -1;
}

bool QtMaterialNavigationModel::setSelectedRow(int row)
{
    if (row < 0 || row >= m_items.size()) return false;
    if (!m_items.at(row).enabled) return false;

    const QString oldId = selectedId();
    const QString oldRoute = selectedRoute();
    QVector<int> changedRows;
    for (int i = 0; i < m_items.size(); ++i) {
        const bool selected = i == row;
        if (m_items[i].selected != selected) {
            m_items[i].selected = selected;
            changedRows.append(i);
        }
    }

    for (int changedRow : changedRows) {
        const QModelIndex idx = index(changedRow, 0);
        emit dataChanged(idx, idx, QVector<int>{SelectedRole});
    }

    if (oldId != selectedId()) emit selectedIdChanged(selectedId());
    if (oldRoute != selectedRoute()) emit selectedRouteChanged(selectedRoute());
    return true;
}

} // namespace QtMaterial
