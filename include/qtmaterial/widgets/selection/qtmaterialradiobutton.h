#pragma once

#include <QPainterPath>
#include <QRectF>

#include "qtmaterial/core/qtmaterialselectioncontrol.h"
#include "qtmaterial/specs/qtmaterialradiobuttonspec.h"
#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {
class QtMaterialRippleController;
class QtMaterialTransitionController;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialRadioButton : public QtMaterialSelectionControl
{
    Q_OBJECT
public:
    explicit QtMaterialRadioButton(QWidget* parent = nullptr);
    explicit QtMaterialRadioButton(const QString& text, QWidget* parent = nullptr);
    ~QtMaterialRadioButton() override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;
    void stateChangedEvent() override;

private:
    void resolveSpecIfNeeded() const;
    void invalidateLayoutCache();
    void resolveLayoutIfNeeded() const;

    mutable bool m_specDirty = true;
    mutable bool m_layoutDirty = true;
    mutable RadioButtonSpec m_spec;

    QtMaterialRippleController* m_ripple = nullptr;
    QtMaterialTransitionController* m_transition = nullptr;

    mutable QRectF m_cachedIndicatorRect;
    mutable QRectF m_cachedDotRect;
    mutable QRectF m_cachedStateLayerRect;
    mutable QRectF m_cachedFocusRingRect;
    mutable QRect m_cachedLabelRect;
    mutable QPainterPath m_cachedIndicatorPath;
    mutable QPainterPath m_cachedRippleClipPath;
    mutable QString m_cachedElidedText;
};

} // namespace QtMaterial
