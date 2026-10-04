#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPointF>
#include <QRectF>
#include <QtMath>

#include "private/qtmaterialbuttonmotionhelper_p.h"
#include "private/qtmaterialbuttonrenderhelper_p.h"
#include "private/qtmaterialtextbutton_p.h"
#include "qtmaterial/core/qtmaterialeventcompat.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/specs/qtmaterialbuttonspecresolver.h"

namespace QtMaterial {

namespace {

struct ExpressiveButtonMetrics
{
 int height;
 int touchTarget;
 int horizontalPadding;
 int iconSize;
 int iconSpacing;
 qreal squareRadius;
 TypeRole typeRole;
};

ExpressiveButtonMetrics expressiveMetrics(QtMaterialButtonSize size)
{
 switch (size) {
 case QtMaterialButtonSize::ExtraSmall:
  return {32, 48, 12, 20, 4, 8.0, TypeRole::LabelLarge};
 case QtMaterialButtonSize::Medium:
  return {56, 56, 24, 24, 8, 12.0, TypeRole::TitleMedium};
 case QtMaterialButtonSize::Large:
  return {96, 96, 48, 32, 12, 16.0, TypeRole::HeadlineSmall};
 case QtMaterialButtonSize::ExtraLarge:
  return {136, 136, 64, 40, 16, 16.0, TypeRole::HeadlineLarge};
 case QtMaterialButtonSize::Small:
 default:
  return {40, 48, 16, 20, 8, 8.0, TypeRole::LabelLarge};
 }
}

qreal resolvedRadius(qreal radius, const QRectF& bounds)
{
 return radius < 0.0
  ? bounds.height() / 2.0
  : qMin(radius, bounds.height() / 2.0);
}

} // namespace

QtMaterialTextButton::QtMaterialTextButton(QWidget* parent)
 : QtMaterialAbstractButton(parent)
 , d(std::make_unique<QtMaterialTextButtonPrivate>(this))
{
 setMinimumHeight(40);
 d->ripple->setObjectName(
  QStringLiteral("_qtm3_button_ripple_controller"));
 d->stateLayerTransition->setObjectName(
  QStringLiteral("_qtm3_button_state_layer_transition"));
 d->stateLayerTransition->setProgress(0.0);
 setMaterialComponent(QStringLiteral("button"));
 setMaterialVariant(QStringLiteral("text"));
 setMaterialRole(QStringLiteral("action"));
 QObject::connect(
  d->stateLayerTransition,
  &QtMaterialTransitionController::progressChanged,
  this,
  [this](qreal) { update(); });
 d->shapeTransition->setObjectName(
  QStringLiteral("_qtm3_button_shape_transition"));
 d->shapeTransition->setProgress(0.0);
 QObject::connect(
  d->shapeTransition,
  &QtMaterialTransitionController::progressChanged,
  this,
  [this](qreal) { update(); });
}

QtMaterialTextButton::QtMaterialTextButton(const QString& text, QWidget* parent)
    : QtMaterialTextButton(parent)
{
    setText(text);
}

QtMaterialTextButton::QtMaterialTextButton(const QIcon& icon, const QString& text, QWidget* parent)
    : QtMaterialTextButton(text, parent)
{
    setIcon(icon);
}


QtMaterialTextButton::~QtMaterialTextButton() = default;

bool QtMaterialTextButton::expressive() const noexcept
{
 return d->expressive;
}

void QtMaterialTextButton::setExpressive(bool enabled)
{
 if (d->expressive == enabled) {
  return;
 }
 d->expressive = enabled;
 d->specDirty = true;
 d->shapeMorph.clear();
 contentChangedEvent();
 updateGeometry();
 syncExpressiveShapeAnimation();
 update();
 emit expressiveChanged(enabled);
}

QtMaterialButtonSize QtMaterialTextButton::expressiveSize() const noexcept
{
 return d->expressiveSize;
}

void QtMaterialTextButton::setExpressiveSize(QtMaterialButtonSize size)
{
 if (d->expressiveSize == size) {
  return;
 }
 d->expressiveSize = size;
 d->specDirty = true;
 d->shapeMorph.clear();
 contentChangedEvent();
 updateGeometry();
 update();
 emit expressiveSizeChanged(size);
}

QtMaterialButtonShape QtMaterialTextButton::expressiveShape() const noexcept
{
 return d->expressiveShape;
}

void QtMaterialTextButton::setExpressiveShape(QtMaterialButtonShape shape)
{
 if (d->expressiveShape == shape) {
  return;
 }
 d->expressiveShape = shape;
 d->specDirty = true;
 d->shapeMorph.clear();
 contentChangedEvent();
 syncExpressiveShapeAnimation();
 update();
 emit expressiveShapeChanged(shape);
}

void QtMaterialTextButton::themeChangedEvent(const Theme& theme)
{
 QtMaterialAbstractButton::themeChangedEvent(theme);
 d->specDirty = true;
 syncStateLayerAnimation();
 syncExpressiveShapeAnimation();
}

void QtMaterialTextButton::invalidateResolvedSpec()
{
 d->specDirty = true;
 d->shapeMorph.clear();
}

ButtonSpec QtMaterialTextButton::resolveButtonSpec() const
{
 ButtonSpecResolver factory;
 return factory.textButtonSpec(theme(), density());
}

void QtMaterialTextButton::applyExpressiveSpec(ButtonSpec& spec) const
{
 const ExpressiveButtonMetrics metrics = expressiveMetrics(d->expressiveSize);
 spec.containerHeight = metrics.height;
 spec.touchTarget = QSize(metrics.touchTarget, metrics.touchTarget);
 spec.horizontalPadding = metrics.horizontalPadding;
 spec.iconSize = metrics.iconSize;
 spec.iconSpacing = metrics.iconSpacing;
 spec.labelTypeRole = metrics.typeRole;
 spec.motionToken = MotionToken::SpatialFast;

 const bool round = d->expressiveShape == QtMaterialButtonShape::Round;
 spec.cornerRadius = round ? -1.0 : metrics.squareRadius;
 spec.pressedCornerRadius = round ? metrics.squareRadius : -1.0;
 spec.selectedCornerRadius = spec.pressedCornerRadius;
 spec.hasStateShapeMorph = true;

 ButtonSpecResolver().resolveRuntimeValues(theme(), &spec);
}

void QtMaterialTextButton::ensureSpecResolved() const
{
 if (!d->specDirty) {
  return;
 }
 d->spec = resolveButtonSpec();
 if (d->expressive) {
  applyExpressiveSpec(d->spec);
 }
 ButtonMotionHelper::configureMotion(
  d->spec,
  d->stateLayerTransition,
  d->ripple,
  theme().accessibility().reducedMotion);
 d->specDirty = false;
}

const ButtonSpec& QtMaterialTextButton::currentButtonSpec() const noexcept
{
 return d->spec;
}

QSize QtMaterialTextButton::sizeHint() const
{
 ensureSpecResolved();
 const ButtonSpec& spec = currentButtonSpec();

 const QFont resolvedFont = ButtonRenderHelper::resolvedLabelFont(font(), spec);

 const auto layout = ButtonRenderHelper::layoutContent(
  this,
  spec,
  QRect(0, 0, 4000, spec.touchTarget.height()),
  resolvedFont,
  text());
 const int contentRight = layout.hasIcon ? qMax(layout.iconRect.right(), layout.textRect.right())
                                    : layout.textRect.right();
 const int contentLeft = layout.hasIcon ? qMin(layout.iconRect.left(), layout.textRect.left())
                                        : layout.textRect.left();
 const int contentWidth = qMax(0, contentRight - contentLeft + 1);
 const int width = qMax(88, contentWidth + 2 * spec.horizontalPadding);
 return QSize(width, spec.touchTarget.height());
}

QSize QtMaterialTextButton::minimumSizeHint() const
{
 ensureSpecResolved();
 return QSize(64, currentButtonSpec().touchTarget.height());
}

void QtMaterialTextButton::mousePressEvent(QMouseEvent* event)
{
 if (isEnabled()) {
  addRippleAt(QtMaterial::mousePosition(event));
 }
 QtMaterialAbstractButton::mousePressEvent(event);
}

void QtMaterialTextButton::stateChangedEvent()
{
 QtMaterialAbstractButton::stateChangedEvent();
 ensureSpecResolved();
 if (
  isEnabled()
  && interactionState().isPressed()
  && d->ripple
  && !d->ripple->isActive()) {
  d->ripple->addRipple(QPointF(rect().center()));
 }
 syncStateLayerAnimation();
 syncExpressiveShapeAnimation();
}

void QtMaterialTextButton::syncStateLayerAnimation()
{
 ensureSpecResolved();
 ButtonMotionHelper::syncStateLayerTransition(
  currentButtonSpec(),
  interactionState(),
  d->stateLayerTransition,
  theme().accessibility().reducedMotion);
}

qreal QtMaterialTextButton::animatedStateLayerOpacity() const noexcept
{
 if (!d->stateLayerTransition) {
  return ButtonMotionHelper::targetStateLayerOpacity(
   currentButtonSpec(),
   interactionState(),
   theme().accessibility().reducedMotion);
 }
 return d->stateLayerTransition->progress();
}

void QtMaterialTextButton::addRippleAt(const QPointF& position)
{
 if (d->ripple) {
  d->ripple->addRipple(position);
 }
}

void QtMaterialTextButton::setRippleClipPath(const QPainterPath& path)
{
 if (d->ripple) {
  d->ripple->setClipPath(path);
 }
}

void QtMaterialTextButton::paintRipple(QPainter* painter, const QColor& color)
{
 if (d->ripple) {
  d->ripple->paint(painter, color);
 }
}

void QtMaterialTextButton::syncExpressiveShapeAnimation()
{
 if (!d->shapeTransition) {
  return;
 }

 d->shapeTransition->applyMotionToken(theme(), MotionToken::SpatialFast);
 const bool active =
  d->expressive
  && isEnabled()
  && (interactionState().isPressed()
      || (interactionState().isCheckable() && interactionState().isChecked()));
 d->shapeTransition->startTo(active ? 1.0 : 0.0);
}

QPainterPath QtMaterialTextButton::buttonContainerPath(const QRectF& bounds) const
{
 ensureSpecResolved();
 const ButtonSpec& spec = currentButtonSpec();
 if (!d->expressive || !spec.hasStateShapeMorph || !d->shapeTransition) {
  return ButtonRenderHelper::containerPath(spec, bounds);
 }

 const qreal sourceRadius = resolvedRadius(spec.cornerRadius, bounds);
 qreal targetToken = interactionState().isCheckable() && interactionState().isChecked()
  ? spec.selectedCornerRadius
  : spec.pressedCornerRadius;
 const qreal targetRadius = resolvedRadius(targetToken, bounds);

 const bool cacheMiss =
  d->morphBounds != bounds
  || !qFuzzyCompare(d->morphSourceRadius + 1.0, sourceRadius + 1.0)
  || !qFuzzyCompare(d->morphTargetRadius + 1.0, targetRadius + 1.0);
 if (cacheMiss) {
  d->morphBounds = bounds;
  d->morphSourceRadius = sourceRadius;
  d->morphTargetRadius = targetRadius;
  d->shapeMorph.setShapes(
   QtMaterialShapeMorph::roundedRectangle(bounds, sourceRadius),
   QtMaterialShapeMorph::roundedRectangle(bounds, targetRadius));
 }

 if (!d->shapeMorph.isValid()) {
  return ButtonRenderHelper::containerPath(spec, bounds);
 }
 return d->shapeMorph.pathAt(d->shapeTransition->progress());
}

qreal QtMaterialTextButton::buttonContainerCornerRadius(const QRectF& bounds) const
{
 ensureSpecResolved();
 const ButtonSpec& spec = currentButtonSpec();
 const qreal sourceRadius = resolvedRadius(spec.cornerRadius, bounds);
 if (!d->expressive || !spec.hasStateShapeMorph || !d->shapeTransition) {
  return sourceRadius;
 }

 const qreal targetToken = interactionState().isCheckable() && interactionState().isChecked()
  ? spec.selectedCornerRadius
  : spec.pressedCornerRadius;
 const qreal targetRadius = resolvedRadius(targetToken, bounds);
 const qreal progress = d->shapeTransition->progress();
 return sourceRadius + (targetRadius - sourceRadius) * progress;
}

void QtMaterialTextButton::paintEvent(QPaintEvent*)
{
 ensureSpecResolved();
 const ButtonSpec& spec = currentButtonSpec();

 QPainter painter(this);
 painter.setRenderHint(QPainter::Antialiasing, true);

 const QRectF visualRect = ButtonRenderHelper::containerRect(rect(), spec);
 const qreal radius = buttonContainerCornerRadius(visualRect);
 const QPainterPath path = buttonContainerPath(visualRect);

 painter.save();
 painter.setPen(Qt::NoPen);
 painter.setBrush(
  isEnabled() ? spec.containerColor : spec.disabledContainerColor);
 painter.drawPath(path);
 painter.restore();

 const qreal layerOpacity = animatedStateLayerOpacity();
 if (isEnabled() && layerOpacity > 0.0) {
  QtMaterialStateLayerPainter::paintPath(&painter, path, spec.stateLayerColor, layerOpacity);
 }

 if (isEnabled()) {
  setRippleClipPath(path);
  paintRipple(&painter, spec.stateLayerColor);
 }

 const QFont resolvedFont = ButtonRenderHelper::resolvedLabelFont(font(), spec);

 const QColor labelColor =
  isEnabled() ? spec.labelColor : spec.disabledLabelColor;
 const QColor iconColor =
  isEnabled() ? spec.iconColor : spec.disabledLabelColor;
 ButtonRenderHelper::paintContent(
  &painter,
  this,
  spec,
  visualRect.toAlignedRect(),
  labelColor,
  iconColor,
  resolvedFont,
  text());

 if (
  spec.focusRingWidth > 0.0
  && QtMaterialFocusIndicator::shouldShow(
   interactionState(),
   focusReason(),
   theme().interactions())) {
  QtMaterialFocusIndicator::paintRectFocusRing(
   &painter,
   visualRect,
   spec.focusRingColor,
   radius,
   spec.focusRingWidth);
 }
}

} // namespace QtMaterial
