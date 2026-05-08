#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>

#include "private/qtmaterialbuttonmotionhelper_p.h"
#include "private/qtmaterialbuttonrenderhelper_p.h"
#include "private/qtmaterialtextbutton_p.h"
#include "qtmaterial/core/qtmaterialeventcompat.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {
namespace {

qreal targetStateLayerOpacity(const Theme& theme, const QtMaterialInteractionState& state)
{
 if (!state.isEnabled()) {
  return 0.0;
 }
 const auto& layer = theme.stateLayer();
 if (state.isPressed()) {
  return layer.pressOpacity;
 }
 if (state.isFocused()) {
  return layer.focusOpacity;
 }
 if (state.isHovered()) {
  return layer.hoverOpacity;
 }
 return 0.0;
}

} // namespace

QtMaterialTextButton::QtMaterialTextButton(QWidget* parent)
 : QtMaterialAbstractButton(parent)
 , d(new QtMaterialTextButtonPrivate(this))
{
 setMinimumHeight(40);
 d->stateLayerTransition->setProgress(0.0);
 QObject::connect(
  d->stateLayerTransition,
  &QtMaterialTransitionController::progressChanged,
  this,
  [this](qreal) { update(); });
}

QtMaterialTextButton::~QtMaterialTextButton()
{
 delete d;
}

void QtMaterialTextButton::themeChangedEvent(const Theme& theme)
{
 QtMaterialAbstractButton::themeChangedEvent(theme);
 d->specDirty = true;
 syncStateLayerAnimation();
}

void QtMaterialTextButton::invalidateResolvedSpec()
{
 d->specDirty = true;
}

ButtonSpec QtMaterialTextButton::resolveButtonSpec() const
{
 SpecFactory factory;
 return factory.textButtonSpec(theme(), density());
}

void QtMaterialTextButton::ensureSpecResolved() const
{
 if (!d->specDirty) {
  return;
 }
 d->spec = resolveButtonSpec();
 ButtonMotionHelper::configureMotion(
  theme(), d->spec, d->stateLayerTransition, d->ripple);
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

 QFont resolvedFont = font();
 if (theme().typography().contains(spec.labelTypeRole)) {
  resolvedFont = theme().typography().style(spec.labelTypeRole).font;
 }

 const auto layout = ButtonRenderHelper::layoutContent(
  this,
  spec,
  QRect(0, 0, 4000, spec.touchTarget.height()),
  resolvedFont,
  text());
 const int contentRight = layout.textRect.right();
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
 addRippleAt(QtMaterial::mousePosition(event));
 QtMaterialAbstractButton::mousePressEvent(event);
}

void QtMaterialTextButton::stateChangedEvent()
{
 QtMaterialAbstractButton::stateChangedEvent();
 syncStateLayerAnimation();
}

void QtMaterialTextButton::syncStateLayerAnimation()
{
 ensureSpecResolved();
 ButtonMotionHelper::syncStateLayerTransition(
  theme(), interactionState(), d->stateLayerTransition);
}

qreal QtMaterialTextButton::animatedStateLayerOpacity() const noexcept
{
 if (!d->stateLayerTransition) {
  return targetStateLayerOpacity(theme(), interactionState());
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

void QtMaterialTextButton::paintEvent(QPaintEvent*)
{
 ensureSpecResolved();
 const ButtonSpec& spec = currentButtonSpec();

 QPainter painter(this);
 painter.setRenderHint(QPainter::Antialiasing, true);

 const QRectF visualRect = ButtonRenderHelper::containerRect(rect(), spec).adjusted(1, 1, -1, -1);
 const qreal radius = ButtonRenderHelper::cornerRadius(theme(), spec, visualRect);
 const QPainterPath path = ButtonRenderHelper::containerPath(theme(), spec, visualRect);

 const qreal layerOpacity = animatedStateLayerOpacity();
 if (layerOpacity > 0.0) {
  QtMaterialStateLayerPainter::paintPath(&painter, path, spec.stateLayerColor, layerOpacity);
 }

 setRippleClipPath(path);
 paintRipple(&painter, spec.stateLayerColor);

 QFont resolvedFont = font();
 if (theme().typography().contains(spec.labelTypeRole)) {
  resolvedFont = theme().typography().style(spec.labelTypeRole).font;
 }

 const QColor contentColor = isEnabled() ? spec.labelColor : spec.disabledLabelColor;
 ButtonRenderHelper::paintContent(
  &painter,
  this,
  spec,
  visualRect.toAlignedRect(),
  contentColor,
  contentColor,
  resolvedFont,
  text());

 if (interactionState().isFocused()) {
  QtMaterialFocusIndicator::paintPathFocusRing(&painter, path, spec.focusRingColor, 2.0);
 }
 Q_UNUSED(radius)
}

} // namespace QtMaterial
