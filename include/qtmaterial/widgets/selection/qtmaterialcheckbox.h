#pragma once

#include <QPainterPath>
#include <QRect>

#include "qtmaterial/core/qtmaterialselectioncontrol.h"
#include "qtmaterial/specs/qtmaterialcheckboxspec.h"
#include "qtmaterial/qtmaterialglobal.h"

class QMouseEvent;
class QPaintEvent;

namespace QtMaterial {

class QtMaterialRippleController;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialCheckbox : public QtMaterialSelectionControl
{
    Q_OBJECT
public:
    explicit QtMaterialCheckbox(QWidget* parent = nullptr);
    explicit QtMaterialCheckbox(const QString& text, QWidget* parent = nullptr);
    ~QtMaterialCheckbox() override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void changeEvent(QEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void resolveSpecIfNeeded() const;
    QRect touchTargetRect() const;
    QRect indicatorRectForPaint() const;
    QRect labelRectForPaint() const;
    QRect stateLayerRect() const;
    QPainterPath indicatorPath() const;
    qreal stateLayerOpacity() const;

    mutable bool m_specDirty = true;
    mutable CheckboxSpec m_spec;
    QtMaterialRippleController* m_ripple = nullptr;
};

} // namespace QtMaterial
