#pragma once

#include <QDialog>
#include <QModelIndex>

#include "qtmaterial/qtmaterialglobal.h"

class QAbstractItemModel;
class QLineEdit;
class QListView;
class QSortFilterProxyModel;

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialCommandPalette : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)

public:
    explicit QtMaterialCommandPalette(QWidget* parent = nullptr);

    void setSourceModel(QAbstractItemModel* model);
    QAbstractItemModel* sourceModel() const;

    QString query() const;
    void setQuery(const QString& query);

    QLineEdit* searchEdit() const noexcept;
    QListView* resultView() const noexcept;

Q_SIGNALS:
    void queryChanged(const QString& query);
    void commandActivated(const QModelIndex& sourceIndex);

private:
    void activateProxyIndex(const QModelIndex& proxyIndex);

    QLineEdit* m_searchEdit = nullptr;
    QListView* m_resultView = nullptr;
    QSortFilterProxyModel* m_proxyModel = nullptr;
};

} // namespace QtMaterial
