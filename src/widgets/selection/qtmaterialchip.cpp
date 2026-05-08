#include "qtmaterial/widgets/selection/qtmaterialchip.h"
#include <memory>

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

struct QtMaterialChipPrivate
{
    ChipVariant variant = ChipVariant::Assist;
    bool removable = false;
    QIcon trailingIcon;
    mutable bool specDirty = true;
    mutable ChipSpec spec;
};

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
    , d(std::make_unique<QtMaterialChipPrivate>())
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
    return d->variant;
}

void QtMaterialChip::setVariant(ChipVariant variant)
{
    if (d->variant == variant) {
        return;
    }
    d->variant = variant;
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
    return d->removable;
}

void QtMaterialChip::setRemovable(bool removable)
{
    if (d->removable == removable) {
        return;
    }
    d->removable = removable;
    updateGeometry();
    update();
    emit removableChanged(removable);
}

QIcon QtMaterialChip::trailingIcon() const
{
    return d->trailingIcon;
}

void QtMaterialChip::setTrailingIcon(const QIcon& icon)
{
    d->trailingIcon = icon;
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
    d->specDirty = true;
}

void QtMaterialChip::stateChangedEvent()
{
    QtMaterialAbstractButton::stateChangedEvent();
    update();
}

ChipSpec resolveChipSpec(ChipVariant variant, const Theme& theme, Density density) {
    SpecFactory factory;
    switch (variant) {
    case ChipVariant::Filter:
        return factory.filterChipSpec(theme, density);
    case ChipVariant::Input:
        return factory.inputChipSpec(theme, density);
    case ChipVariant::Suggestion:
        return factory.suggestionChipSpec(theme, density);
    case ChipVariant::Assist:
    default:
        return factory.assistChipSpec(theme, density);
    }

}

void ensureChipSpecResolved(QtMaterialChipPrivate& d, const Theme& theme, Density density) {
    if (!d.specDirty) {
        return;
    }
    d.spec = resolveChipSpec(d.variant, theme, density);
    d.specDirty = false;

}

QRectF chipContainerRect(QtMaterialChipPrivate& d, const Theme& theme, Density density, int widgetWidth, int widgetHeight) {
    ensureChipSpecResolved(d, theme, density);
    const int y = (widgetHeight - d.spec.containerHeight) / 2;
    return QRectF(0.5, y + 0.5, widgetWidth - 1.0, d.spec.containerHeight - 1.0);

}

QRect chipTrailingIconRect(QtMaterialChipPrivate& d, const Theme& theme, Density density, const QRectF& visualRect) {
    ensureChipSpecResolved(d, theme, density);
    const int x = static_cast<int>(visualRect.right()) - d.spec.horizontalPadding - d.spec.iconSize;
    const int y = static_cast<int>(visualRect.center().y()) - d.spec.iconSize / 2;
    return QRect(x, y, d.spec.iconSize, d.spec.iconSize);

}

QSize QtMaterialChip::sizeHint() const
{
    ensureChipSpecResolved(*d, theme(), density());
    QFont resolvedFont = font();
    if (theme().typography().contains(d->spec.labelTypeRole)) {
        resolvedFont = theme().typography().style(d->spec.labelTypeRole).font;
    }
    const QFontMetrics fm(resolvedFont);
    int width = d->spec.horizontalPadding * 2 + fm.horizontalAdvance(text());
    if (!icon().isNull()) {
        width += d->spec.iconSize + d->spec.iconSpacing;
    }
    if (d->removable || !d->trailingIcon.isNull()) {
        width += d->spec.iconSize + d->spec.iconSpacing;
    }
    return QSize(qMax(d->spec.minWidth, width), d->spec.touchTarget.height());
}

QSize QtMaterialChip::minimumSizeHint() const
{
    ensureChipSpecResolved(*d, theme(), density());
    return QSize(d->spec.minWidth, d->spec.touchTarget.height());
}

void QtMaterialChip::mouseReleaseEvent(QMouseEvent* event)
{
    ensureChipSpecResolved(*d, theme(), density());
    if ((d->removable || !d->trailingIcon.isNull()) && chipTrailingIconRect(*d, theme(), density(), chipContainerRect(*d, theme(), density(), width(), height())).contains(event->pos())) {
        emit removeRequested();
        event->accept();
        return;
    }
    QtMaterialAbstractButton::mouseReleaseEvent(event);
}

void QtMaterialChip::paintEvent(QPaintEvent*)
{
    ensureChipSpecResolved(*d, theme(), density());
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF visualRect = chipContainerRect(*d, theme(), density(), width(), height());
    const qreal radius = theme().shapes().contains(d->spec.shapeRole)
        ? theme().shapes().radius(d->spec.shapeRole)
        : visualRect.height() / 2.0;

    QPainterPath path;
    path.addRoundedRect(visualRect, radius, radius);

    const bool selected = isChecked();
    const QColor container = selected ? d->spec.selectedContainerColor : d->spec.containerColor;
    const QColor labelColor = !isEnabled()
        ? d->spec.disabledLabelColor
        : (selected ? d->spec.selectedLabelColor : d->spec.labelColor);
    const QColor iconColor = !isEnabled()
        ? d->spec.disabledLabelColor
        : (selected ? d->spec.selectedIconColor : d->spec.iconColor);

    if (container.alpha() > 0) {
        painter.fillPath(path, container);
    }

    const QColor outline = isEnabled() ? d->spec.outlineColor : d->spec.disabledOutlineColor;
    if (outline.isValid() && outline.alpha() > 0) {
        QPen pen(outline, d->spec.outlineWidth);
        painter.setPen(pen);
        painter.drawPath(path);
    }

    const qreal opacity = stateOpacity(theme(), interactionState());
    if (opacity > 0.0) {
        QtMaterialStateLayerPainter::paintPath(&painter, path, d->spec.stateLayerColor, opacity);
    }

    QFont resolvedFont = font();
    if (theme().typography().contains(d->spec.labelTypeRole)) {
        resolvedFont = theme().typography().style(d->spec.labelTypeRole).font;
    }
    painter.setFont(resolvedFont);
    painter.setPen(labelColor);

    int x = static_cast<int>(visualRect.left()) + d->spec.horizontalPadding;
    if (!icon().isNull()) {
        const QRect iconRect(x, static_cast<int>(visualRect.center().y()) - d->spec.iconSize / 2, d->spec.iconSize, d->spec.iconSize);
        icon().paint(&painter, iconRect, Qt::AlignCenter, isEnabled() ? QIcon::Normal : QIcon::Disabled);
        x += d->spec.iconSize + d->spec.iconSpacing;
    }

    int right = static_cast<int>(visualRect.right()) - d->spec.horizontalPadding;
    QRect closeRect;
    if (d->removable || !d->trailingIcon.isNull()) {
        closeRect = chipTrailingIconRect(*d, theme(), density(), visualRect);
        right = closeRect.left() - d->spec.iconSpacing;
    }

    painter.drawText(QRect(x, static_cast<int>(visualRect.top()), right - x, static_cast<int>(visualRect.height())),
                     Qt::AlignVCenter | Qt::AlignLeft, text());

    if (!closeRect.isNull()) {
        if (!d->trailingIcon.isNull()) {
            d->trailingIcon.paint(&painter, closeRect, Qt::AlignCenter, isEnabled() ? QIcon::Normal : QIcon::Disabled);
        } else {
            painter.setPen(QPen(iconColor, 1.8));
            painter.drawLine(closeRect.topLeft() + QPoint(4, 4), closeRect.bottomRight() - QPoint(4, 4));
            painter.drawLine(closeRect.topRight() + QPoint(-4, 4), closeRect.bottomLeft() + QPoint(4, -4));
        }
    }

    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintPathFocusRing(&painter, path, d->spec.focusRingColor, 2.0);
    }
}

} // namespace QtMaterial
