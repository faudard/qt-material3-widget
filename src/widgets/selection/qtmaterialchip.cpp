#include "qtmaterial/widgets/selection/qtmaterialchip.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {
namespace {

qreal stateOpacity(const Theme& theme, const QtMaterialInteractionState& state)
{
    if (!state.isEnabled()) {
        return 0.0;
    }
    if (state.isPressed()) {
        return theme.stateLayer().pressOpacity;
    }
    if (state.isFocused()) {
        return theme.stateLayer().focusOpacity;
    }
    if (state.isHovered()) {
        return theme.stateLayer().hoverOpacity;
    }
    return 0.0;
}

} // namespace

QtMaterialChip::QtMaterialChip(QWidget* parent)
    : QtMaterialAbstractButton(parent)
{
    setCheckable(false);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);
}

QtMaterialChip::QtMaterialChip(const QString& text, QWidget* parent)
    : QtMaterialChip(parent)
{
    setText(text);
}

QtMaterialChip::~QtMaterialChip() = default;

ChipVariant QtMaterialChip::variant() const noexcept
{
    return m_variant;
}

void QtMaterialChip::setVariant(ChipVariant variant)
{
    if (m_variant == variant) {
        return;
    }
    m_variant = variant;
    if (variant == ChipVariant::Filter) {
        setCheckable(true);
    }
    invalidateResolvedSpec();
    updateGeometry();
    update();
    emit variantChanged(variant);
}

bool QtMaterialChip::isRemovable() const noexcept
{
    return m_removable;
}

void QtMaterialChip::setRemovable(bool removable)
{
    if (m_removable == removable) {
        return;
    }
    m_removable = removable;
    updateGeometry();
    update();
    emit removableChanged(removable);
}

QIcon QtMaterialChip::trailingIcon() const
{
    return m_trailingIcon;
}

void QtMaterialChip::setTrailingIcon(const QIcon& icon)
{
    m_trailingIcon = icon;
    updateGeometry();
    update();
}

void QtMaterialChip::themeChangedEvent(const Theme& theme)
{
    QtMaterialAbstractButton::themeChangedEvent(theme);
    invalidateResolvedSpec();
}

void QtMaterialChip::invalidateResolvedSpec()
{
    m_specDirty = true;
}

void QtMaterialChip::stateChangedEvent()
{
    QtMaterialAbstractButton::stateChangedEvent();
    update();
}

ChipSpec QtMaterialChip::resolveSpec() const
{
    SpecFactory factory;
    switch (m_variant) {
    case ChipVariant::Filter:
        return factory.filterChipSpec(theme(), density());
    case ChipVariant::Input:
        return factory.inputChipSpec(theme(), density());
    case ChipVariant::Suggestion:
        return factory.suggestionChipSpec(theme(), density());
    case ChipVariant::Assist:
    default:
        return factory.assistChipSpec(theme(), density());
    }
}

void QtMaterialChip::ensureSpecResolved() const
{
    if (!m_specDirty) {
        return;
    }
    m_spec = resolveSpec();
    m_specDirty = false;
}

QRectF QtMaterialChip::containerRect() const
{
    ensureSpecResolved();
    const int y = (height() - m_spec.containerHeight) / 2;
    return QRectF(0.5, y + 0.5, width() - 1.0, m_spec.containerHeight - 1.0);
}

QRect QtMaterialChip::trailingIconRect(const QRectF& visualRect) const
{
    ensureSpecResolved();
    const int x = static_cast<int>(visualRect.right()) - m_spec.horizontalPadding - m_spec.iconSize;
    const int y = static_cast<int>(visualRect.center().y()) - m_spec.iconSize / 2;
    return QRect(x, y, m_spec.iconSize, m_spec.iconSize);
}

QSize QtMaterialChip::sizeHint() const
{
    ensureSpecResolved();
    QFont resolvedFont = font();
    if (theme().typography().contains(m_spec.labelTypeRole)) {
        resolvedFont = theme().typography().style(m_spec.labelTypeRole).font;
    }
    const QFontMetrics fm(resolvedFont);
    int width = m_spec.horizontalPadding * 2 + fm.horizontalAdvance(text());
    if (!icon().isNull()) {
        width += m_spec.iconSize + m_spec.iconSpacing;
    }
    if (m_removable || !m_trailingIcon.isNull()) {
        width += m_spec.iconSize + m_spec.iconSpacing;
    }
    return QSize(qMax(m_spec.minWidth, width), m_spec.touchTarget.height());
}

QSize QtMaterialChip::minimumSizeHint() const
{
    ensureSpecResolved();
    return QSize(m_spec.minWidth, m_spec.touchTarget.height());
}

void QtMaterialChip::mouseReleaseEvent(QMouseEvent* event)
{
    ensureSpecResolved();
    if ((m_removable || !m_trailingIcon.isNull()) && trailingIconRect(containerRect()).contains(event->pos())) {
        emit removeRequested();
        event->accept();
        return;
    }
    QtMaterialAbstractButton::mouseReleaseEvent(event);
}

void QtMaterialChip::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF visualRect = containerRect();
    const qreal radius = theme().shapes().contains(m_spec.shapeRole)
        ? theme().shapes().radius(m_spec.shapeRole)
        : visualRect.height() / 2.0;

    QPainterPath path;
    path.addRoundedRect(visualRect, radius, radius);

    const bool selected = isChecked();
    const QColor container = selected ? m_spec.selectedContainerColor : m_spec.containerColor;
    const QColor labelColor = !isEnabled()
        ? m_spec.disabledLabelColor
        : (selected ? m_spec.selectedLabelColor : m_spec.labelColor);
    const QColor iconColor = !isEnabled()
        ? m_spec.disabledLabelColor
        : (selected ? m_spec.selectedIconColor : m_spec.iconColor);

    if (container.alpha() > 0) {
        painter.fillPath(path, container);
    }

    const QColor outline = isEnabled() ? m_spec.outlineColor : m_spec.disabledOutlineColor;
    if (outline.isValid() && outline.alpha() > 0) {
        QPen pen(outline, m_spec.outlineWidth);
        painter.setPen(pen);
        painter.drawPath(path);
    }

    const qreal opacity = stateOpacity(theme(), interactionState());
    if (opacity > 0.0) {
        QtMaterialStateLayerPainter::paintPath(&painter, path, m_spec.stateLayerColor, opacity);
    }

    QFont resolvedFont = font();
    if (theme().typography().contains(m_spec.labelTypeRole)) {
        resolvedFont = theme().typography().style(m_spec.labelTypeRole).font;
    }
    painter.setFont(resolvedFont);
    painter.setPen(labelColor);

    int x = static_cast<int>(visualRect.left()) + m_spec.horizontalPadding;
    if (!icon().isNull()) {
        const QRect iconRect(x, static_cast<int>(visualRect.center().y()) - m_spec.iconSize / 2, m_spec.iconSize, m_spec.iconSize);
        icon().paint(&painter, iconRect, Qt::AlignCenter, isEnabled() ? QIcon::Normal : QIcon::Disabled);
        x += m_spec.iconSize + m_spec.iconSpacing;
    }

    int right = static_cast<int>(visualRect.right()) - m_spec.horizontalPadding;
    QRect closeRect;
    if (m_removable || !m_trailingIcon.isNull()) {
        closeRect = trailingIconRect(visualRect);
        right = closeRect.left() - m_spec.iconSpacing;
    }

    painter.drawText(QRect(x, static_cast<int>(visualRect.top()), right - x, static_cast<int>(visualRect.height())),
                     Qt::AlignVCenter | Qt::AlignLeft, text());

    if (!closeRect.isNull()) {
        if (!m_trailingIcon.isNull()) {
            m_trailingIcon.paint(&painter, closeRect, Qt::AlignCenter, isEnabled() ? QIcon::Normal : QIcon::Disabled);
        } else {
            painter.setPen(QPen(iconColor, 1.8));
            painter.drawLine(closeRect.topLeft() + QPoint(4, 4), closeRect.bottomRight() - QPoint(4, 4));
            painter.drawLine(closeRect.topRight() + QPoint(-4, 4), closeRect.bottomLeft() + QPoint(4, -4));
        }
    }

    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintPathFocusRing(&painter, path, m_spec.focusRingColor, 2.0);
    }
}

} // namespace QtMaterial
