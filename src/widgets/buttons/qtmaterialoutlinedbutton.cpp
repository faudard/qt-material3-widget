#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"

#include <QEvent>
#include <QPainter>
#include <QResizeEvent>

#include "private/qtmaterialbuttonrenderhelper_p.h"
#include "private/qtmaterialoutlinedbutton_p.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

QtMaterialOutlinedButton::QtMaterialOutlinedButton(QWidget* parent)
 : QtMaterialTextButton(parent)
 , d(std::make_unique<QtMaterialOutlinedButtonPrivate>())
{
}

QtMaterialOutlinedButton::~QtMaterialOutlinedButton() = default;

void QtMaterialOutlinedButtonPrivate::invalidateLayout(QtMaterialOutlinedButton& button)
{
 layout.dirty = true;
 button.updateGeometry();
 button.update();
}

void QtMaterialOutlinedButtonPrivate::ensureLayoutResolved(const QtMaterialOutlinedButton& button) const
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

void QtMaterialOutlinedButton::themeChangedEvent(const Theme& theme)
{
 QtMaterialTextButton::themeChangedEvent(theme);
 d->invalidateLayout(*this);
}

void QtMaterialOutlinedButton::invalidateResolvedSpec()
{
 QtMaterialTextButton::invalidateResolvedSpec();
 d->invalidateLayout(*this);
}

void QtMaterialOutlinedButton::contentChangedEvent()
{
 d->invalidateLayout(*this);
}

void QtMaterialOutlinedButton::resizeEvent(QResizeEvent* event)
{
 d->invalidateLayout(*this);
 QtMaterialTextButton::resizeEvent(event);
}

void QtMaterialOutlinedButton::changeEvent(QEvent* event)
{
 if (event->type() == QEvent::FontChange || event->type() == QEvent::StyleChange) {
  d->invalidateLayout(*this);
 }
 QtMaterialTextButton::changeEvent(event);
}

ButtonSpec QtMaterialOutlinedButton::resolveButtonSpec() const
{
 SpecFactory factory;
 return factory.outlinedButtonSpec(theme(), density());
}

void QtMaterialOutlinedButton::paintEvent(QPaintEvent*)
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

 painter.save();
 painter.setBrush(Qt::NoBrush);
 painter.setPen(QPen(isEnabled() ? spec.outlineColor : spec.disabledOutlineColor, 1.0));
 painter.drawPath(d->layout.containerPath);
 painter.restore();

 const qreal layerOpacity = ButtonRenderHelper::stateLayerOpacity(theme(), interactionState());
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

 const QColor contentColor = isEnabled() ? spec.labelColor : spec.disabledLabelColor;
 ButtonRenderHelper::paintContentLayout(
  &painter,
  this,
  spec,
  contentLayout,
  contentColor,
  contentColor,
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
