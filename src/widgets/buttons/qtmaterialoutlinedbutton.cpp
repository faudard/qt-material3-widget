#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include <QtGlobal>
#include <QPen>

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
 setMaterialComponent(QStringLiteral("button"));
 setMaterialVariant(QStringLiteral("outlined"));
 setMaterialRole(QStringLiteral("action"));
}

QtMaterialOutlinedButton::QtMaterialOutlinedButton(const QString& text, QWidget* parent)
    : QtMaterialOutlinedButton(parent)
{
    setText(text);
}

QtMaterialOutlinedButton::QtMaterialOutlinedButton(const QIcon& icon, const QString& text, QWidget* parent)
    : QtMaterialOutlinedButton(text, parent)
{
    setIcon(icon);
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

    const qreal strokeInset = qMax<qreal>(resolvedOutlineStrokeWidth(button), 1.0);
    layout.visualRect = ButtonRenderHelper::containerRect(button.rect(), spec)
                            .adjusted(strokeInset, strokeInset, -strokeInset, -strokeInset)
                            .toAlignedRect();
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

qreal QtMaterialOutlinedButtonPrivate::resolvedOutlineStrokeWidth(const QtMaterialOutlinedButton& button) const
{
    const qreal dpr = qMax<qreal>(button.devicePixelRatioF(), 1.0);
    return qMax<qreal>(1.0 / dpr, 0.5);
}

QColor QtMaterialOutlinedButtonPrivate::resolvedOutlineColor(const QtMaterialOutlinedButton& button) const
{
    button.ensureSpecResolved();
    const ButtonSpec& spec = button.currentButtonSpec();
    return button.isEnabled() ? spec.outlineColor : spec.disabledOutlineColor;
}

bool QtMaterialOutlinedButtonPrivate::shouldPaintOutline(const QtMaterialOutlinedButton& button) const
{
    const QColor color = resolvedOutlineColor(button);
    return color.isValid() && color.alpha() > 0 && resolvedOutlineStrokeWidth(button) > 0.0;
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
    switch (event->type()) {
    case QEvent::EnabledChange:
    case QEvent::FontChange:
    case QEvent::LayoutDirectionChange:
    case QEvent::PaletteChange:
    case QEvent::StyleChange:
        d->invalidateLayout(*this);
        break;
    default:
        break;
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

    if (d->shouldPaintOutline(*this)) {
        QPen outlinePen(d->resolvedOutlineColor(*this));
        outlinePen.setWidthF(d->resolvedOutlineStrokeWidth(*this));
        outlinePen.setCapStyle(Qt::RoundCap);
        outlinePen.setJoinStyle(Qt::RoundJoin);
        outlinePen.setCosmetic(false);

        painter.save();
        painter.setBrush(Qt::NoBrush);
        painter.setPen(outlinePen);
        painter.drawPath(d->layout.containerPath);
        painter.restore();
    }

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
