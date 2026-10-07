#pragma once

#include <memory>

#include <QByteArray>
#include <QTreeView>
#include <QString>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QtMaterialTreeViewPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialTreeView : public QTreeView
{
    Q_OBJECT
    Q_PROPERTY(bool dense READ dense WRITE setDense NOTIFY denseChanged)
    Q_PROPERTY(bool multiSelectionEnabled READ multiSelectionEnabled WRITE setMultiSelectionEnabled NOTIFY multiSelectionEnabledChanged)
    Q_PROPERTY(bool dragDropEnabled READ dragDropEnabled WRITE setDragDropEnabled NOTIFY dragDropEnabledChanged)
    Q_PROPERTY(QString accessibilitySummary READ accessibilitySummary NOTIFY accessibilitySummaryChanged)

public:
    explicit QtMaterialTreeView(QWidget* parent = nullptr);
    ~QtMaterialTreeView() override;

    bool dense() const noexcept;
    void setDense(bool dense);

    bool multiSelectionEnabled() const noexcept;
    void setMultiSelectionEnabled(bool enabled);

    bool dragDropEnabled() const noexcept;
    void setDragDropEnabled(bool enabled);

    QString accessibilitySummary() const;
    QString currentItemAccessibleText() const;

    /**
     * Saves header geometry/order, desktop policies, the current item and the
     * visible expanded hierarchy. Model data is never serialized.
     */
    QByteArray saveWorkspaceState() const;

    /**
     * Restores state against the currently installed model/root index.
     *
     * Row paths are validated before mutation. Incompatible model topology,
     * column counts or malformed payloads are rejected.
     */
    bool restoreWorkspaceState(const QByteArray& state);

    void setModel(QAbstractItemModel* model) override;

Q_SIGNALS:
    void denseChanged(bool dense);
    void multiSelectionEnabledChanged(bool enabled);
    void dragDropEnabledChanged(bool enabled);
    void accessibilitySummaryChanged(const QString& summary);

protected:
    void currentChanged(const QModelIndex& current, const QModelIndex& previous) override;

private:
    void syncAccessibility();
    std::unique_ptr<QtMaterialTreeViewPrivate> d_ptr;
};

} // namespace QtMaterial
