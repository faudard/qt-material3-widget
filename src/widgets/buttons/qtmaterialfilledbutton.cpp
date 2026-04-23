#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

#include <QEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>

#include <QtGlobal>

#include "private/qtmaterialbuttonrenderhelper_p.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

QtMaterialFilledButton::QtMaterialFilledButton(QWidget* parent)
    : QtMaterialTextButton(parent)
    , m_elevationTransition(new QtMaterialTransitionController(this))
{
    m_elevationTransition->setProgress(0.0);
    QObject::connect(m_elevationTransition, &QtMaterialTransitionController::progressChanged,
                     this, [this](qreal) { update(); });
}

QtMaterialFilledButton::~QtMaterialFilledButton() = default;

void QtMaterialFilledButton::themeChangedEvent(const Theme& theme)
{
    QtMaterialTextButton::themeChangedEvent(theme);
    invalidateLayoutCache();
    syncElevationAnimation();
}

void QtMaterialFilledButton::invalidateResolvedSpec()
{
    QtMaterialTextButton::invalidateResolvedSpec();
    invalidateLayoutCache();
    syncElevationAnimation();
}

void QtMaterialFilledButton::contentChangedEvent()
{
    invalidateLayoutCache();
}

void QtMaterialFilledButton::resizeEvent(QResizeEvent* event)
{
    invalidateLayoutCache();
    QtMaterialTextButton::resizeEvent(event);
}

void QtMaterialFilledButton::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::FontChange || event->type() == QEvent::StyleChange) {
        invalidateLayoutCache();
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
    syncElevationAnimation();
}

qreal QtMaterialFilledButton::targetElevationProgress() const noexcept
{
    if (!interactionState().isEnabled()) {
        return 0.0;
    }
    if (interactionState().isPressed()) {
        return 1.0;
    }
    if (interactionState().isHovered() || interactionState().isFocused()) {
        return 0.65;
    }
    return 0.0;
}

qreal QtMaterialFilledButton::animatedElevationProgress() const noexcept
{
    return m_elevationTransition ? m_elevationTransition->progress() : targetElevationProgress();
}

void QtMaterialFilledButton::syncElevationAnimation()
{
    ensureSpecResolved();
    if (!m_elevationTransition) {
        update();
        return;
    }

    m_elevationTransition->applyMotionToken(theme(), m_spec.motionToken);
    m_elevationTransition->startTo(targetElevationProgress());
}

void QtMaterialFilledButton::paintInteractionElevation(QPainter* painter) const
{
    const qreal progress = animatedElevationProgress();
    if (!isEnabled() || progress <= 0.0 || m_cachedContainerPath.isEmpty()
        || m_spec.elevationRole == ElevationRole::Level0) {
        return;
    }

    painter->save();
    painter->setPen(Qt::NoPen);

    QColor ambientShadow(0, 0, 0, qRound(14.0 + 18.0 * progress));
    painter->setBrush(ambientShadow);
    painter->drawPath(m_cachedContainerPath.translated(0.0, 1.0 + progress));

    QColor keyShadow(0, 0, 0, qRound(10.0 + 24.0 * progress));
    painter->setBrush(keyShadow);
    painter->drawPath(m_cachedContainerPath.translated(0.0, 2.0 + 2.0 * progress));

    painter->restore();
}

void QtMaterialFilledButton::invalidateLayoutCache()
{
    m_layoutDirty = true;
    updateGeometry();
    update();
}

void QtMaterialFilledButton::ensureLayoutResolved() const
{
    ensureSpecResolved();
    if (!m_layoutDirty) {
        return;
    }

    QFont resolvedFont = font();
    if (theme().typography().contains(m_spec.labelTypeRole)) {
        resolvedFont = theme().typography().style(m_spec.labelTypeRole).font;
    }

    m_cachedVisualRect = ButtonRenderHelper::containerRect(rect(), m_spec).adjusted(1, 1, -1, -1).toAlignedRect();
    m_cachedCornerRadius = ButtonRenderHelper::cornerRadius(theme(), m_spec, m_cachedVisualRect);
    m_cachedContainerPath = ButtonRenderHelper::containerPath(theme(), m_spec, m_cachedVisualRect);

    const auto layout = ButtonRenderHelper::layoutContent(this, m_spec, m_cachedVisualRect, resolvedFont, text());
    m_cachedIconRect = layout.iconRect;
    m_cachedTextRect = layout.textRect;
    m_cachedElidedText = layout.elidedText;
    m_cachedHasIcon = layout.hasIcon;
    m_layoutDirty = false;
}

void QtMaterialFilledButton::paintEvent(QPaintEvent*)
{
    ensurePolished();
    ensureSpecResolved();
    ensureLayoutResolved();

    QFont resolvedFont = font();
    if (theme().typography().contains(m_spec.labelTypeRole)) {
        resolvedFont = theme().typography().style(m_spec.labelTypeRole).font;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    paintInteractionElevation(&painter);

    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(isEnabled() ? m_spec.containerColor : m_spec.disabledContainerColor);
    painter.drawPath(m_cachedContainerPath);
    painter.restore();

    const qreal layerOpacity = animatedStateLayerOpacity();
    if (layerOpacity > 0.0) {
        QtMaterialStateLayerPainter::paintPath(&painter, m_cachedContainerPath,
                                               m_spec.stateLayerColor, layerOpacity);
    }

    if (m_ripple) {
        m_ripple->setClipPath(m_cachedContainerPath);
        m_ripple->paint(&painter, m_spec.stateLayerColor);
    }

    ButtonRenderHelper::ContentLayout layout;
    layout.iconRect = m_cachedIconRect;
    layout.textRect = m_cachedTextRect;
    layout.elidedText = m_cachedElidedText;
    layout.hasIcon = m_cachedHasIcon;

    const QColor textColor = isEnabled() ? m_spec.labelColor : m_spec.disabledLabelColor;
    const QColor iconColor = isEnabled() ? m_spec.iconColor : m_spec.disabledLabelColor;
    ButtonRenderHelper::paintContentLayout(&painter, this, m_spec, layout, textColor, iconColor,
                                           resolvedFont);

    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintRectFocusRing(&painter, m_cachedVisualRect,
                                                     m_spec.focusRingColor, m_cachedCornerRadius,
                                                     2.0);
    }
}

} // namespace QtMaterial
