#pragma once

#include <memory>

#include <QDialog>
#include <QModelIndex>

#include "qtmaterial/qtmaterialglobal.h"

class QAbstractItemModel;

namespace QtMaterial {

class QtMaterialCommandPalettePrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialCommandPalette : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)

public:
    explicit QtMaterialCommandPalette(QWidget* parent = nullptr);
    ~QtMaterialCommandPalette() override;

    void setSourceModel(QAbstractItemModel* model);
    QAbstractItemModel* sourceModel() const;

    QString query() const;
    void setQuery(const QString& query);

Q_SIGNALS:
    void queryChanged(const QString& query);
    void commandActivated(const QModelIndex& sourceIndex);

private:
    void activateProxyIndex(const QModelIndex& proxyIndex);

    std::unique_ptr<QtMaterialCommandPalettePrivate> d_ptr;
};

} // namespace QtMaterial
