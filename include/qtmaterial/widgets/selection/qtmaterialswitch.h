#pragma once

#include <QPainterPath>
#include <QRectF>
#include <QFont>

#include "qtmaterial/core/qtmaterialselectioncontrol.h"
#include "qtmaterial/specs/qtmaterialswitchspec.h"
#include "qtmaterial/qtmaterialglobal.h"

class QKeyEvent;
class QMouseEvent;
class QResizeEvent;

namespace QtMaterial {

class QtMaterialRippleController;
class QtMaterialTransitionController;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSwitch : public QtMaterialSelectionControl
{
    Q_OBJECT
public:
    explicit QtMaterialSwitch(QWidget* parent = nullptr);
    explicit QtMaterialSwitch(const QString& text, QWidget* parent = nullptr);
    ~QtMaterialSwitch() override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;
    void stateChangedEvent() override;
    void contentChangedEvent() override;

private:
    void resolveSpecIfNeeded() const;
    void invalidateLayoutCache();
    void resolveLayoutIfNeeded() const;
    void syncTransitionState(bool animated);
    void syncAccessibleState();

    mutable bool m_specDirty = true;
    mutable bool m_layoutDirty = true;
    mutable SwitchSpec m_spec;

    QtMaterialTransitionController* m_transition = nullptr;
    QtMaterialRippleController* m_ripple = nullptr;

    mutable QRectF m_cachedTrackRect;
    mutable QRectF m_cachedStateLayerRect;
    mutable QRectF m_cachedHandleRect;
    mutable QRectF m_cachedFocusRingRect;
    mutable QRect m_cachedLabelRect;
    mutable QPainterPath m_cachedTrackPath;
    mutable QPainterPath m_cachedRippleClipPath;
    mutable QPainterPath m_cachedFocusRingPath;
    mutable QString m_cachedElidedText;
    mutable QFont m_cachedLabelFont;
};

} // namespace QtMaterial
