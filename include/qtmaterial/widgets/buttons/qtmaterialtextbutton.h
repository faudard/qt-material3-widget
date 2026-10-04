#pragma once
#include <QString>
#include <QIcon>


#include <memory>
#include <QColor>
#include <QPointF>

#include "qtmaterial/core/qtmaterialabstractbutton.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/qtmaterialglobal.h"

class QMouseEvent;
class QPaintEvent;
class QPainter;
class QPainterPath;
class QRectF;

namespace QtMaterial {

enum class QtMaterialButtonSize
{
 ExtraSmall,
 Small,
 Medium,
 Large,
 ExtraLarge
};

enum class QtMaterialButtonShape
{
 Round,
 Square
};

class QtMaterialTextButtonPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialTextButton : public QtMaterialAbstractButton {
 Q_OBJECT
public:
 explicit QtMaterialTextButton(QWidget* parent = nullptr);
    explicit QtMaterialTextButton(const QString& text, QWidget* parent = nullptr);
    QtMaterialTextButton(const QIcon& icon, const QString& text, QWidget* parent = nullptr);
 ~QtMaterialTextButton() override;

 QSize sizeHint() const override;
 QSize minimumSizeHint() const override;

 bool expressive() const noexcept;
 void setExpressive(bool enabled);

 QtMaterialButtonSize expressiveSize() const noexcept;
 void setExpressiveSize(QtMaterialButtonSize size);

 QtMaterialButtonShape expressiveShape() const noexcept;
 void setExpressiveShape(QtMaterialButtonShape shape);

protected:
 void paintEvent(QPaintEvent* event) override;
 void themeChangedEvent(const QtMaterial::Theme& theme) override;
 void invalidateResolvedSpec() override;
 void mousePressEvent(QMouseEvent* event) override;
 void stateChangedEvent() override;

 virtual ButtonSpec resolveButtonSpec() const;
 virtual void applyExpressiveSpec(ButtonSpec& spec) const;

protected:
 void ensureSpecResolved() const;
 const ButtonSpec& currentButtonSpec() const noexcept;

 qreal animatedStateLayerOpacity() const noexcept;
 void syncStateLayerAnimation();

 void addRippleAt(const QPointF& position);
 void setRippleClipPath(const QPainterPath& path);
 void paintRipple(QPainter* painter, const QColor& color);

 QPainterPath buttonContainerPath(const QRectF& bounds) const;
 qreal buttonContainerCornerRadius(const QRectF& bounds) const;

signals:
 void expressiveChanged(bool enabled);
 void expressiveSizeChanged(QtMaterial::QtMaterialButtonSize size);
 void expressiveShapeChanged(QtMaterial::QtMaterialButtonShape shape);

private:
 void syncExpressiveShapeAnimation();
 friend class QtMaterialTextButtonPrivate;
 std::unique_ptr<QtMaterialTextButtonPrivate> d;
};

} // namespace QtMaterial
