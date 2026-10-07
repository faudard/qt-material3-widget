#pragma once

#include <memory>

#include <QSplitter>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QtMaterialSplitViewPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSplitView : public QSplitter
{
    Q_OBJECT
    Q_PROPERTY(int keyboardResizeStep READ keyboardResizeStep WRITE setKeyboardResizeStep)
    Q_PROPERTY(bool animatedCollapseEnabled READ animatedCollapseEnabled WRITE setAnimatedCollapseEnabled)
    Q_PROPERTY(int collapseAnimationDuration READ collapseAnimationDuration WRITE setCollapseAnimationDuration)
    Q_PROPERTY(bool rememberPaneSizes READ rememberPaneSizes WRITE setRememberPaneSizes)

public:
    explicit QtMaterialSplitView(QWidget* parent = nullptr);
    explicit QtMaterialSplitView(Qt::Orientation orientation, QWidget* parent = nullptr);
    ~QtMaterialSplitView() override;

    void setPaneCollapsible(int index, bool collapsible);
    bool paneCollapsible(int index) const;

    void setPaneCollapsed(int index, bool collapsed);
    bool paneCollapsed(int index) const;

    void setPaneMinimumExtent(int index, int extent);
    int paneMinimumExtent(int index) const;
    void setPaneMaximumExtent(int index, int extent);
    int paneMaximumExtent(int index) const;

    void resetPaneSizes();
    QList<int> defaultPaneSizes() const;
    void setDefaultPaneSizes(const QList<int>& sizes);

    int keyboardResizeStep() const noexcept;
    void setKeyboardResizeStep(int step);
    bool animatedCollapseEnabled() const noexcept;
    void setAnimatedCollapseEnabled(bool enabled);
    int collapseAnimationDuration() const noexcept;
    void setCollapseAnimationDuration(int duration);

    QByteArray savePaneState() const;
    bool restorePaneState(const QByteArray& state);

    // Uniform 1.12 desktop workspace-state naming. These forward to the
    // established pane-state format and do not introduce a second schema.
    QByteArray saveWorkspaceState() const;
    bool restoreWorkspaceState(const QByteArray& state);
    bool rememberPaneSizes() const noexcept;
    void setRememberPaneSizes(bool remember);

Q_SIGNALS:
    void paneCollapsedChanged(int index, bool collapsed);
    void paneStateChanged(const QByteArray& state);

protected:
    QSplitterHandle* createHandle() override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private:
    void finishCollapseAnimation();
    void persistPaneState();
    void restoreSavedPaneState();
    void trackPane(QWidget* pane);
    std::unique_ptr<QtMaterialSplitViewPrivate> d_ptr;
};

} // namespace QtMaterial
