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
#include "qtmaterial/specs/qtmaterialbuttonspecresolver.h"

namespace QtMaterial {
namespace {

} // namespace

QtMaterialTextButton::QtMaterialTextButton(QWidget* parent)
 : QtMaterialAbstractButton(parent)
 , d(std::make_unique<QtMaterialTextButtonPrivate>(this))
{
 setMinimumHeight(40);
 d->stateLayerTransition->setProgress(0.0);
 setMaterialComponent(QStringLiteral("button"));
 setMaterialVariant(QStringLiteral("text"));
 setMaterialRole(QStringLiteral("action"));
 QObject::connect(
  d->stateLayerTransition,
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
 ButtonSpecResolver factory;
 return factory.textButtonSpec(theme(), density());
}

void QtMaterialTextButton::ensureSpecResolved() const
{
 if (!d->specDirty) {
  return;
 }
 d->spec = resolveButtonSpec();
 ButtonMotionHelper::configureMotion(d->spec, d->stateLayerTransition, d->ripple);
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
 ButtonMotionHelper::syncStateLayerTransition(currentButtonSpec(), interactionState(), d->stateLayerTransition);
}

qreal QtMaterialTextButton::animatedStateLayerOpacity() const noexcept
{
 if (!d->stateLayerTransition) {
  return ButtonMotionHelper::targetStateLayerOpacity(currentButtonSpec(), interactionState());
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
 const qreal radius = ButtonRenderHelper::cornerRadius(spec, visualRect);
 const QPainterPath path = ButtonRenderHelper::containerPath(spec, visualRect);

 const qreal layerOpacity = animatedStateLayerOpacity();
 if (layerOpacity > 0.0) {
  QtMaterialStateLayerPainter::paintPath(&painter, path, spec.stateLayerColor, layerOpacity);
 }

 setRippleClipPath(path);
 paintRipple(&painter, spec.stateLayerColor);

 const QFont resolvedFont = ButtonRenderHelper::resolvedLabelFont(font(), spec);

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
