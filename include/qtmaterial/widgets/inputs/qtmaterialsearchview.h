#pragma once

#include <QModelIndex>
#include <QWidget>

#include "qtmaterial/qtmaterialglobal.h"

class QAbstractItemModel;
class QListView;
class QSortFilterProxyModel;

namespace QtMaterial {

class QtMaterialSearchBar;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSearchView : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int filterKeyColumn READ filterKeyColumn WRITE setFilterKeyColumn)
    Q_PROPERTY(Qt::CaseSensitivity filterCaseSensitivity READ filterCaseSensitivity WRITE setFilterCaseSensitivity)

public:
    explicit QtMaterialSearchView(QWidget* parent = nullptr);
    ~QtMaterialSearchView() override;

    void setSourceModel(QAbstractItemModel* model);
    QAbstractItemModel* sourceModel() const noexcept;

    QtMaterialSearchBar* searchBar() const noexcept;
    QListView* resultView() const noexcept;

    int filterKeyColumn() const noexcept;
    void setFilterKeyColumn(int column);

    Qt::CaseSensitivity filterCaseSensitivity() const noexcept;
    void setFilterCaseSensitivity(Qt::CaseSensitivity sensitivity);

signals:
    void queryChanged(const QString& query);
    void activated(const QModelIndex& sourceIndex);

private:
    QtMaterialSearchBar* m_searchBar = nullptr;
    QListView* m_view = nullptr;
    QSortFilterProxyModel* m_proxy = nullptr;
    QAbstractItemModel* m_sourceModel = nullptr;
};

} // namespace QtMaterial
