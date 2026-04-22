#pragma once

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/core/qtmaterialoverlaysurface.h"

class QKeyEvent;
class QResizeEvent;
class QShowEvent;
class QWidget;
class QMouseEvent;
class QObject;
class QEvent;

namespace QtMaterial {

class QtMaterialScrimWidget;
class QtMaterialTransitionController;
class BottomSheetSpec;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialBottomSheet : public QtMaterialOverlaySurface
{
    Q_OBJECT
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)

public:
    enum class SheetState { Closed, Opening, Open, Closing };

    explicit QtMaterialBottomSheet(QWidget *parent = nullptr);
    ~QtMaterialBottomSheet() override;

    void open();
    void close();

    bool isOpen() const noexcept;
    qreal progress() const noexcept;
    SheetState state() const noexcept;

    void setModal(bool modal);
    bool isModal() const noexcept;

    void setExpandedHeight(int px);
    int expandedHeight() const noexcept;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    QWidget* contentWidget() const noexcept;

signals:
    void progressChanged(qreal value);
    void stateChanged(QtMaterial::QtMaterialBottomSheet::SheetState state);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void themeChangedEvent(const QtMaterial::Theme &theme) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void ensureSpecResolved() const;
    void ensureGeometryResolved() const;
    void invalidateCachedGeometry();
    void syncToHost();
    void syncScrim();
    void syncContainerGeometry();
    void applyContainerClip();
    void focusFirstChild();
    void applySheetMask();

    QRect sheetVisualRect() const;
    QRect contentRect() const;
    qreal cornerRadius() const;

private:
    mutable bool m_specDirty = true;
    mutable bool m_geometryDirty = true;
    mutable QtMaterial::BottomSheetSpec *m_specPtr = nullptr;

    mutable QRect m_cachedVisualRect;
    mutable QRect m_cachedContentRect;
    mutable QPainterPath m_cachedContainerPath;
    mutable qreal m_cachedCornerRadius = 0.0;

    QPointer<QtMaterialScrimWidget> m_scrim;
    QPointer<QtMaterialTransitionController> m_transition;
    QPointer<QWidget> m_container;

    SheetState m_state = SheetState::Closed;
    bool m_modal = true;
    int m_expandedHeight = 320;
};

} // namespace QtMaterial