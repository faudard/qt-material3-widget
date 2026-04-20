#pragma once

#include <QPersistentModelIndex>
#include <QPointer>
#include <QStringList>

class QListView;
class QSortFilterProxyModel;
class QStringListModel;

struct QtMaterialAutocompletePopupPrivate
{
    QStringListModel* sourceModel = nullptr;
    QSortFilterProxyModel* proxyModel = nullptr;
    QListView* view = nullptr;

    QString filterText;
    QPersistentModelIndex currentIndex;
    bool popupVisible = false;
    bool geometryDirty = true;
};
