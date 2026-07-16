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
#include "qtmaterial/specs/qtmaterialbuttonspecresolver.h"
#include "private/qtmaterialbuttonmotionhelper_p.h"
#include "qtmaterial/effects/qtmaterialelevationrenderer.h"

namespace QtMaterial {

QtMaterialFilledButton::QtMaterialFilledButton(QWidget* parent)
 : QtMaterialTextButton(parent)
 , d(std::make_unique<QtMaterialFilledButtonPrivate>(this))
{
 setMaterialComponent(QStringLiteral("button"));
 setMaterialVariant(QStringLiteral("filled"));
 setMaterialRole(QStringLiteral("action"));
 d->elevationTransition->setProgress(0.0);
 QObject::connect(
  d->elevationTransition,
  &QtMaterialTransitionController::progressChanged,
  this,
  [this](qreal) { update(); });
}

QtMaterialFilledButton::QtMaterialFilledButton(const QString& text, QWidget* parent)
    : QtMaterialFilledButton(parent)
{
    setText(text);
}

QtMaterialFilledButton::QtMaterialFilledButton(const QIcon& icon, const QString& text, QWidget* parent)
    : QtMaterialFilledButton(text, parent)
{
    setIcon(icon);
}


QtMaterialFilledButton::~QtMaterialFilledButton() = default;

qreal QtMaterialFilledButtonPrivate::targetElevationProgress(
    const QtMaterialFilledButton& button) const noexcept
{
    button.ensureSpecResolved();
    const ButtonSpec& spec = button.currentButtonSpec();
    const QtMaterialInteractionState state = button.interactionState();

    if (!state.isEnabled()) {
        return spec.disabledElevationProgress;
    }
    if (state.isPressed()) {
        return spec.pressElevationProgress;
    }
    if (state.isFocused()) {
        return spec.focusElevationProgress;
    }
    if (state.isHovered()) {
        return spec.hoverElevationProgress;
    }
    return spec.restingElevationProgress;
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
 const QFont resolvedFont = ButtonRenderHelper::resolvedLabelFont(button.font(), spec);

 layout.visualRect = ButtonRenderHelper::containerRect(button.rect(), spec).adjusted(1, 1, -1, -1).toAlignedRect();
 layout.cornerRadius = ButtonRenderHelper::cornerRadius(spec, layout.visualRect);
 layout.containerPath = ButtonRenderHelper::containerPath(spec, layout.visualRect);

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
 ButtonMotionHelper::configureTransition(currentButtonSpec(), d->elevationTransition);
 d->elevationTransition->startTo(d->targetElevationProgress(*this));
}

void QtMaterialFilledButton::invalidateResolvedSpec()
{
 QtMaterialTextButton::invalidateResolvedSpec();
 d->invalidateLayout(*this);
 ensureSpecResolved();
 ButtonMotionHelper::configureTransition(currentButtonSpec(), d->elevationTransition);
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
 ButtonSpecResolver factory;
 return factory.filledButtonSpec(theme(), density());
}

void QtMaterialFilledButton::stateChangedEvent()
{
 QtMaterialTextButton::stateChangedEvent();
 ensureSpecResolved();
 ButtonMotionHelper::configureTransition(currentButtonSpec(), d->elevationTransition);
 d->elevationTransition->startTo(d->targetElevationProgress(*this));
}

void QtMaterialFilledButton::paintEvent(QPaintEvent*)
{
 ensurePolished();
 ensureSpecResolved();
 d->ensureLayoutResolved(*this);
 const ButtonSpec& spec = currentButtonSpec();

 const QFont resolvedFont = ButtonRenderHelper::resolvedLabelFont(font(), spec);

 QPainter painter(this);
 painter.setRenderHint(QPainter::Antialiasing, true);

    const qreal elevationProgress =
        d->animatedElevationProgress(*this);

    if (
        isEnabled()
        && spec.hasResolvedElevationStyle
        && elevationProgress > 0.0
        && !d->layout.containerPath.isEmpty()) {
        QtMaterialElevationRenderer::
            paintInterpolatedPathElevation(
                &painter,
                d->layout.containerPath,
                spec.shadowColor,
                ElevationStyle{},
                spec.elevationStyle,
                elevationProgress);
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
