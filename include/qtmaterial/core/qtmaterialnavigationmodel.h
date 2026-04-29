#pragma once

#include <QAbstractListModel>
#include <QIcon>
#include <QString>
#include <QVector>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

struct QTMATERIAL3_CORE_EXPORT QtMaterialNavigationItem {
    QString id;
    QString route;
    QString label;
    QIcon icon;
    QString testId;
    bool enabled = true;
    bool selected = false;
};

class QTMATERIAL3_CORE_EXPORT QtMaterialNavigationModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(QString selectedId READ selectedId NOTIFY selectedIdChanged)
    Q_PROPERTY(QString selectedRoute READ selectedRoute NOTIFY selectedRouteChanged)

public:
    enum Role {
        IdRole = Qt::UserRole + 1,
        RouteRole,
        LabelRole,
        IconRole,
        TestIdRole,
        EnabledRole,
        SelectedRole
    };
    Q_ENUM(Role)

    explicit QtMaterialNavigationModel(QObject* parent = nullptr);
    ~QtMaterialNavigationModel() override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    QVector<QtMaterialNavigationItem> items() const;
    void setItems(const QVector<QtMaterialNavigationItem>& items);
    void addItem(const QtMaterialNavigationItem& item);
    QtMaterialNavigationItem itemAt(int row) const;

    QString selectedId() const;
    QString selectedRoute() const;

public slots:
    bool setSelectedId(const QString& id);
    bool setSelectedRoute(const QString& route);

signals:
    void selectedIdChanged(const QString& id);
    void selectedRouteChanged(const QString& route);

private:
    int findRowById(const QString& id) const;
    int findRowByRoute(const QString& route) const;
    bool setSelectedRow(int row);

    QVector<QtMaterialNavigationItem> m_items;
};

} // namespace QtMaterial
