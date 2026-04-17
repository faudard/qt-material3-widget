#pragma once

#include <QPainterPath>
#include <QRect>

#include "qtmaterial/core/qtmaterialabstractbutton.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/qtmaterialglobal.h"

class QMouseEvent;
class QPaintEvent;

namespace QtMaterial {

class QtMaterialRippleController;
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialOutlinedButton : public QtMaterialAbstractButton
{
    Q_OBJECT
public:
    explicit QtMaterialOutlinedButton(QWidget* parent = nullptr);
    explicit QtMaterialOutlinedButton(const QString& text, QWidget* parent = nullptr);
    ~QtMaterialOutlinedButton() override;

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
    QRect containerRect() const;
    QRect contentRectForPaint() const;
    QRect iconRectForContent(const QRect& contentRect) const;
    QRect textRectForContent(const QRect& contentRect) const;
    QPainterPath containerPath() const;
    qreal containerRadius(const QRect& rect) const;
    qreal stateLayerOpacity() const;

    mutable bool m_specDirty = true;
    mutable ButtonSpec m_spec;
    QtMaterialRippleController* m_ripple = nullptr;
};

} // namespace QtMaterial
