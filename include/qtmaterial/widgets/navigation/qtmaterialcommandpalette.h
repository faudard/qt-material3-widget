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
    Q_PROPERTY(QString emptyStateText READ emptyStateText WRITE setEmptyStateText)

public:
    enum CommandRole {
        ShortcutRole = Qt::UserRole + 1
    };
    Q_ENUM(CommandRole)

    explicit QtMaterialCommandPalette(QWidget* parent = nullptr);
    ~QtMaterialCommandPalette() override;

    void setSourceModel(QAbstractItemModel* model);
    QAbstractItemModel* sourceModel() const;

    QString query() const;
    void setQuery(const QString& query);

    QString emptyStateText() const;
    void setEmptyStateText(const QString& text);

Q_SIGNALS:
    void queryChanged(const QString& query);
    void commandActivated(const QModelIndex& sourceIndex);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void activateProxyIndex(const QModelIndex& proxyIndex);

    std::unique_ptr<QtMaterialCommandPalettePrivate> d_ptr;
};

} // namespace QtMaterial
