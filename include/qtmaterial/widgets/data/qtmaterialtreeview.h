#pragma once

#include <QTreeView>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialTreeView : public QTreeView
{
    Q_OBJECT
    Q_PROPERTY(bool dense READ dense WRITE setDense NOTIFY denseChanged)
    Q_PROPERTY(bool multiSelectionEnabled READ multiSelectionEnabled WRITE setMultiSelectionEnabled NOTIFY multiSelectionEnabledChanged)
    Q_PROPERTY(bool dragDropEnabled READ dragDropEnabled WRITE setDragDropEnabled NOTIFY dragDropEnabledChanged)

public:
    explicit QtMaterialTreeView(QWidget* parent = nullptr);

    bool dense() const noexcept;
    void setDense(bool dense);

    bool multiSelectionEnabled() const noexcept;
    void setMultiSelectionEnabled(bool enabled);

    bool dragDropEnabled() const noexcept;
    void setDragDropEnabled(bool enabled);

Q_SIGNALS:
    void denseChanged(bool dense);
    void multiSelectionEnabledChanged(bool enabled);
    void dragDropEnabledChanged(bool enabled);

private:
    bool m_dense = false;
    bool m_dragDropEnabled = false;
};

} // namespace QtMaterial
