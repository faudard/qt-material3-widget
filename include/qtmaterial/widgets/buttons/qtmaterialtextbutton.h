#pragma once

#include <QColor>
#include <QPointF>

#include "qtmaterial/core/qtmaterialabstractbutton.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/qtmaterialglobal.h"

class QMouseEvent;
class QPaintEvent;
class QPainter;
class QPainterPath;

namespace QtMaterial {

class QtMaterialTextButtonPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialTextButton : public QtMaterialAbstractButton {
 Q_OBJECT
public:
 explicit QtMaterialTextButton(QWidget* parent = nullptr);
 ~QtMaterialTextButton() override;

 QSize sizeHint() const override;
 QSize minimumSizeHint() const override;

protected:
 void paintEvent(QPaintEvent* event) override;
 void themeChangedEvent(const QtMaterial::Theme& theme) override;
 void invalidateResolvedSpec() override;
 void mousePressEvent(QMouseEvent* event) override;
 void stateChangedEvent() override;

 virtual ButtonSpec resolveButtonSpec() const;

protected:
 void ensureSpecResolved() const;
 const ButtonSpec& currentButtonSpec() const noexcept;

 qreal animatedStateLayerOpacity() const noexcept;
 void syncStateLayerAnimation();

 void addRippleAt(const QPointF& position);
 void setRippleClipPath(const QPainterPath& path);
 void paintRipple(QPainter* painter, const QColor& color);

private:
 friend class QtMaterialTextButtonPrivate;
 QtMaterialTextButtonPrivate* d = nullptr;
};

} // namespace QtMaterial
