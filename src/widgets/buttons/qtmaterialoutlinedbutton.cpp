#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"

#include <QEvent>
#include <QPainter>
#include <QResizeEvent>

#include "private/qtmaterialbuttonrenderhelper_p.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

QtMaterialOutlinedButton::QtMaterialOutlinedButton(QWidget* parent)
    : QtMaterialTextButton(parent)
{
}

QtMaterialOutlinedButton::~QtMaterialOutlinedButton() = default;

void QtMaterialOutlinedButton::themeChangedEvent(const Theme& theme)
{
    QtMaterialTextButton::themeChangedEvent(theme);
    invalidateLayoutCache();
}

void QtMaterialOutlinedButton::invalidateResolvedSpec()
{
    QtMaterialTextButton::invalidateResolvedSpec();
    invalidateLayoutCache();
}

void QtMaterialOutlinedButton::contentChangedEvent()
{
    invalidateLayoutCache();
}

void QtMaterialOutlinedButton::resizeEvent(QResizeEvent* event)
{
    invalidateLayoutCache();
    QtMaterialTextButton::resizeEvent(event);
}

void QtMaterialOutlinedButton::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::FontChange || event->type() == QEvent::StyleChange) {
        invalidateLayoutCache();
    }
    QtMaterialTextButton::changeEvent(event);
}

ButtonSpec QtMaterialOutlinedButton::resolveButtonSpec() const
{
    SpecFactory factory;
    return factory.outlinedButtonSpec(theme(), density());
}

void QtMaterialOutlinedButton::invalidateLayoutCache()
{
    m_layoutDirty = true;
}

void QtMaterialOutlinedButton::ensureLayoutResolved() const
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

void QtMaterialOutlinedButton::paintEvent(QPaintEvent*)
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

    painter.save();
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(isEnabled() ? m_spec.outlineColor : m_spec.disabledOutlineColor, 1.0));
    painter.drawPath(m_cachedContainerPath);
    painter.restore();

    const qreal layerOpacity = animatedStateLayerOpacity();
    if (layerOpacity > 0.0) {
        QtMaterialStateLayerPainter::paintPath(&painter, m_cachedContainerPath, m_spec.stateLayerColor, layerOpacity);
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

    const QColor contentColor = isEnabled() ? m_spec.labelColor : m_spec.disabledLabelColor;
    ButtonRenderHelper::paintContentLayout(&painter, this, m_spec, layout, contentColor, contentColor, resolvedFont);

    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintRectFocusRing(&painter, m_cachedVisualRect, m_spec.focusRingColor, m_cachedCornerRadius, 2.0);
    }
}

} // namespace QtMaterial
