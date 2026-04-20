#pragma once

#include <QPointer>
#include <QRect>
#include <QPainterPath>

#include "qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h"

class QKeyEvent;
class QResizeEvent;
class QShowEvent;

namespace QtMaterial {
class BottomSheetSpec;
class ScrimWidget;
class TransitionController;
}

class QtMaterialBottomSheet : public QtMaterialOverlaySurface
{
    Q_OBJECT
public:
    enum class SheetState {
        Closed,
        Opening,
        Open,
        Closing
    };

    explicit QtMaterialBottomSheet(QWidget *parent = nullptr);
    ~QtMaterialBottomSheet() override;

    void open();
    void close();
    bool isOpen() const noexcept;

    void setModal(bool modal);
    bool isModal() const noexcept;

    void setExpandedHeight(int px);
    int expandedHeight() const noexcept;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void themeChangedEvent(const QtMaterial::Theme &theme) override;

private:
    void ensureSpecResolved() const;
    void ensureGeometryResolved() const;
    void invalidateCachedGeometry();
    void syncToHost();
    void syncScrim();
    void focusFirstChild();

    QRect sheetVisualRect() const;
    QRect contentRect() const;
    qreal cornerRadius() const;

    mutable bool m_specDirty = true;
    mutable bool m_geometryDirty = true;
    mutable QtMaterial::BottomSheetSpec *m_specPtr = nullptr;
    mutable QRect m_cachedVisualRect;
    mutable QRect m_cachedContentRect;
    mutable QPainterPath m_cachedContainerPath;
    mutable qreal m_cachedCornerRadius = 0.0;

    QPointer<QtMaterial::ScrimWidget> m_scrim;
    QPointer<QtMaterial::TransitionController> m_transition;
    SheetState m_state = SheetState::Closed;
    bool m_modal = true;
    int m_expandedHeight = 320;
};
