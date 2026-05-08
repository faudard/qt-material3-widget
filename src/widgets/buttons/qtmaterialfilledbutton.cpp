#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

#include <QEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QtGlobal>

#include "private/qtmaterialbuttonrenderhelper_p.h"
#include "private/qtmaterialfilledbutton_p.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

QtMaterialFilledButton::QtMaterialFilledButton(QWidget* parent)
 : QtMaterialTextButton(parent)
 , d(std::make_unique<QtMaterialFilledButtonPrivate>(this))
{
 d->elevationTransition->setProgress(0.0);
 QObject::connect(
  d->elevationTransition,
  &QtMaterialTransitionController::progressChanged,
  this,
  [this](qreal) { update(); });
}

QtMaterialFilledButton::~QtMaterialFilledButton() = default;

qreal QtMaterialFilledButtonPrivate::targetElevationProgress(const QtMaterialFilledButton& button) const noexcept
{
 if (!button.interactionState().isEnabled()) {
  return 0.0;
 }
 if (button.interactionState().isPressed()) {
  return 1.0;
 }
 if (button.interactionState().isHovered() || button.interactionState().isFocused()) {
  return 0.65;
 }
 return 0.0;
}

qreal QtMaterialFilledButtonPrivate::animatedElevationProgress(const QtMaterialFilledButton& button) const noexcept
{
 return elevationTransition ? elevationTransition->progress() : targetElevationProgress(button);
}

void QtMaterialFilledButtonPrivate::invalidateLayout(QtMaterialFilledButton& button)
{
 layout.dirty = true;
 button.updateGeometry();
 button.update();
}

void QtMaterialFilledButtonPrivate::ensureLayoutResolved(const QtMaterialFilledButton& button) const
{
 button.ensureSpecResolved();
 if (!layout.dirty) {
  return;
 }

 const ButtonSpec& spec = button.currentButtonSpec();
 QFont resolvedFont = button.font();
 if (button.theme().typography().contains(spec.labelTypeRole)) {
  resolvedFont = button.theme().typography().style(spec.labelTypeRole).font;
 }

 layout.visualRect = ButtonRenderHelper::containerRect(button.rect(), spec).adjusted(1, 1, -1, -1).toAlignedRect();
 layout.cornerRadius = ButtonRenderHelper::cornerRadius(button.theme(), spec, layout.visualRect);
 layout.containerPath = ButtonRenderHelper::containerPath(button.theme(), spec, layout.visualRect);

 const auto contentLayout = ButtonRenderHelper::layoutContent(
  &button,
  spec,
  layout.visualRect,
  resolvedFont,
  button.text());
 layout.iconRect = contentLayout.iconRect;
 layout.textRect = contentLayout.textRect;
 layout.elidedText = contentLayout.elidedText;
 layout.hasIcon = contentLayout.hasIcon;
 layout.dirty = false;
}

void QtMaterialFilledButton::themeChangedEvent(const Theme& theme)
{
 QtMaterialTextButton::themeChangedEvent(theme);
 d->invalidateLayout(*this);
 ensureSpecResolved();
 d->elevationTransition->applyMotionToken(theme, currentButtonSpec().motionToken);
 d->elevationTransition->startTo(d->targetElevationProgress(*this));
}

void QtMaterialFilledButton::invalidateResolvedSpec()
{
 QtMaterialTextButton::invalidateResolvedSpec();
 d->invalidateLayout(*this);
 ensureSpecResolved();
 d->elevationTransition->applyMotionToken(theme(), currentButtonSpec().motionToken);
 d->elevationTransition->startTo(d->targetElevationProgress(*this));
}

void QtMaterialFilledButton::contentChangedEvent()
{
 d->invalidateLayout(*this);
}

void QtMaterialFilledButton::resizeEvent(QResizeEvent* event)
{
 d->invalidateLayout(*this);
 QtMaterialTextButton::resizeEvent(event);
}

void QtMaterialFilledButton::changeEvent(QEvent* event)
{
 if (event->type() == QEvent::FontChange || event->type() == QEvent::StyleChange) {
  d->invalidateLayout(*this);
 }
 QtMaterialTextButton::changeEvent(event);
}

ButtonSpec QtMaterialFilledButton::resolveButtonSpec() const
{
 SpecFactory factory;
 return factory.filledButtonSpec(theme(), density());
}

void QtMaterialFilledButton::stateChangedEvent()
{
 QtMaterialTextButton::stateChangedEvent();
 ensureSpecResolved();
 d->elevationTransition->applyMotionToken(theme(), currentButtonSpec().motionToken);
 d->elevationTransition->startTo(d->targetElevationProgress(*this));
}

void QtMaterialFilledButton::paintEvent(QPaintEvent*)
{
 ensurePolished();
 ensureSpecResolved();
 d->ensureLayoutResolved(*this);
 const ButtonSpec& spec = currentButtonSpec();

 QFont resolvedFont = font();
 if (theme().typography().contains(spec.labelTypeRole)) {
  resolvedFont = theme().typography().style(spec.labelTypeRole).font;
 }

 QPainter painter(this);
 painter.setRenderHint(QPainter::Antialiasing, true);

 const qreal elevationProgress = d->animatedElevationProgress(*this);
 if (isEnabled() && elevationProgress > 0.0 && !d->layout.containerPath.isEmpty()
  && spec.elevationRole != ElevationRole::Level0) {
  painter.save();
  painter.setPen(Qt::NoPen);

  QColor ambientShadow(0, 0, 0, qRound(14.0 + 18.0 * elevationProgress));
  painter.setBrush(ambientShadow);
  painter.drawPath(d->layout.containerPath.translated(0.0, 1.0 + elevationProgress));

  QColor keyShadow(0, 0, 0, qRound(10.0 + 24.0 * elevationProgress));
  painter.setBrush(keyShadow);
  painter.drawPath(d->layout.containerPath.translated(0.0, 2.0 + 2.0 * elevationProgress));
  painter.restore();
 }

 painter.save();
 painter.setPen(Qt::NoPen);
 painter.setBrush(isEnabled() ? spec.containerColor : spec.disabledContainerColor);
 painter.drawPath(d->layout.containerPath);
 painter.restore();

 const qreal layerOpacity = animatedStateLayerOpacity();
 if (layerOpacity > 0.0) {
  QtMaterialStateLayerPainter::paintPath(
   &painter,
   d->layout.containerPath,
   spec.stateLayerColor,
   layerOpacity);
 }

 setRippleClipPath(d->layout.containerPath);
 paintRipple(&painter, spec.stateLayerColor);

 ButtonRenderHelper::ContentLayout contentLayout;
 contentLayout.iconRect = d->layout.iconRect;
 contentLayout.textRect = d->layout.textRect;
 contentLayout.elidedText = d->layout.elidedText;
 contentLayout.hasIcon = d->layout.hasIcon;

 const QColor textColor = isEnabled() ? spec.labelColor : spec.disabledLabelColor;
 const QColor iconColor = isEnabled() ? spec.iconColor : spec.disabledLabelColor;
 ButtonRenderHelper::paintContentLayout(
  &painter,
  this,
  spec,
  contentLayout,
  textColor,
  iconColor,
  resolvedFont);

 if (interactionState().isFocused()) {
  QtMaterialFocusIndicator::paintRectFocusRing(
   &painter,
   d->layout.visualRect,
   spec.focusRingColor,
   d->layout.cornerRadius,
   2.0);
 }
}

} // namespace QtMaterial
