#include "qtmaterial/widgets/selection/qtmaterialchip.h"

#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QStyle>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/specs/qtmaterialcompactspecresolver.h"

namespace QtMaterial {
namespace {


qreal stateOpacity(
    const ChipSpec& spec,
    const QtMaterialInteractionState& state)
{
    if (!state.isEnabled()) {
        return 0.0;
    }

    if (state.isPressed()) {
        return spec.pressStateLayerOpacity;
    }

    if (state.isFocused()) {
        return spec.focusStateLayerOpacity;
    }

    if (state.isHovered()) {
        return spec.hoverStateLayerOpacity;
    }

    return 0.0;
}

QString variantName(ChipVariant variant)
{
    switch (variant) {
    case ChipVariant::Filter:
        return QStringLiteral("filter");
    case ChipVariant::Input:
        return QStringLiteral("input");
    case ChipVariant::Suggestion:
        return QStringLiteral("suggestion");
    case ChipVariant::Assist:
    default:
        return QStringLiteral("assist");
    }
}

void paintTintedIcon(
    QPainter* painter,
    const QIcon& icon,
    const QRect& target,
    const QColor& color,
    bool enabled)
{
    if (
        !painter
        || icon.isNull()
        || target.isEmpty()) {
        return;
    }

    QPixmap pixmap =
        icon.pixmap(
            target.size(),
            enabled
                ? QIcon::Normal
                : QIcon::Disabled,
            QIcon::Off);

    if (pixmap.isNull()) {
        return;
    }

    if (color.isValid()) {
        QPainter tintPainter(&pixmap);
        tintPainter.setCompositionMode(
            QPainter::CompositionMode_SourceIn);
        tintPainter.fillRect(
            pixmap.rect(),
            color);
    }

    painter->drawPixmap(
        target,
        pixmap);
}

} // namespace

QtMaterialChip::QtMaterialChip(
    QWidget* parent)
    : QtMaterialAbstractButton(parent)
{
    setCheckable(false);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);
    setMaterialComponent(
        QStringLiteral("chip"));
    setMaterialVariant(
        QStringLiteral("assist"));
}

QtMaterialChip::QtMaterialChip(
    const QString& text,
    QWidget* parent)
    : QtMaterialChip(parent)
{
    setText(text);
}

QtMaterialChip::~QtMaterialChip() = default;

ChipVariant QtMaterialChip::variant() const noexcept
{
    return m_variant;
}

void QtMaterialChip::setVariant(
    ChipVariant variant)
{
    if (m_variant == variant) {
        return;
    }

    m_variant = variant;

    const bool filter =
        variant == ChipVariant::Filter;

    if (!filter && isChecked()) {
        setChecked(false);
    }

    setCheckable(filter);
    setMaterialVariant(
        variantName(variant));

    invalidateResolvedSpec();
    updateGeometry();
    update();

    emit variantChanged(variant);
}

bool QtMaterialChip::isRemovable() const noexcept
{
    return m_removable;
}

void QtMaterialChip::setRemovable(
    bool removable)
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

void QtMaterialChip::setTrailingIcon(
    const QIcon& icon)
{
    if (m_trailingIcon.cacheKey() == icon.cacheKey()) {
        return;
    }

    m_trailingIcon = icon;
    updateGeometry();
    update();
}

const ChipSpec& QtMaterialChip::resolvedSpec() const
{
    ensureSpecResolved();
    return m_spec;
}

void QtMaterialChip::themeChangedEvent(
    const Theme& theme)
{
    QtMaterialAbstractButton::
        themeChangedEvent(theme);
    invalidateResolvedSpec();
}

void QtMaterialChip::invalidateResolvedSpec()
{
    m_specDirty = true;
}

void QtMaterialChip::stateChangedEvent()
{
    QtMaterialAbstractButton::
        stateChangedEvent();
    update();
}

void QtMaterialChip::contentChangedEvent()
{
    QtMaterialAbstractButton::
        contentChangedEvent();
    updateGeometry();
    update();
}

ChipSpec QtMaterialChip::resolveSpec() const
{
    const CompactSpecResolver resolver;
    return resolver.resolve(
        m_variant,
        theme(),
        density());
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
    const ChipSpec& spec =
        resolvedSpec();

    const int y =
        (height() - spec.containerHeight) / 2;

    return QRectF(
        0.5,
        y + 0.5,
        qMax(0.0, width() - 1.0),
        qMax(0.0, spec.containerHeight - 1.0));
}

QRect QtMaterialChip::trailingIconRect(
    const QRectF& visualRect) const
{
    const ChipSpec& spec =
        resolvedSpec();

    const QRect outer =
        visualRect.toAlignedRect();

    const QRect logical(
        outer.right()
            - spec.horizontalPadding
            - spec.iconSize
            + 1,
        outer.center().y()
            - spec.iconSize / 2,
        spec.iconSize,
        spec.iconSize);

    return style()->visualRect(
        layoutDirection(),
        outer,
        logical);
}

QSize QtMaterialChip::sizeHint() const
{
    const ChipSpec& spec =
        resolvedSpec();

    const QFont resolvedFont =
        spec.hasResolvedLabelFont
            ? spec.labelFont
            : font();

    const QFontMetrics metrics(
        resolvedFont);

    int width =
        spec.horizontalPadding * 2
        + metrics.horizontalAdvance(text());

    if (!icon().isNull()) {
        width +=
            spec.iconSize
            + spec.iconSpacing;
    }

    if (
        m_removable
        || !m_trailingIcon.isNull()) {
        width +=
            spec.iconSize
            + spec.iconSpacing;
    }

    return QSize(
        qMax(spec.minWidth, width),
        spec.touchTarget.height());
}

QSize QtMaterialChip::minimumSizeHint() const
{
    const ChipSpec& spec =
        resolvedSpec();

    return QSize(
        spec.minWidth,
        spec.touchTarget.height());
}

void QtMaterialChip::mouseReleaseEvent(
    QMouseEvent* event)
{
    if (
        event
        && isEnabled()
        && event->button() == Qt::LeftButton
        && (
            m_removable
            || !m_trailingIcon.isNull())
        && trailingIconRect(
               containerRect())
               .contains(event->pos())) {
        setDown(false);
        emit removeRequested();
        event->accept();
        update();
        return;
    }

    QtMaterialAbstractButton::
        mouseReleaseEvent(event);
}

void QtMaterialChip::paintEvent(
    QPaintEvent*)
{
    const ChipSpec& spec =
        resolvedSpec();

    QPainter painter(this);
    painter.setRenderHint(
        QPainter::Antialiasing,
        true);

    const QRectF visualRect =
        containerRect();

    if (visualRect.isEmpty()) {
        return;
    }

    const qreal radius =
        qMin(
            spec.cornerRadius,
            visualRect.height() / 2.0);

    QPainterPath path;
    path.addRoundedRect(
        visualRect,
        radius,
        radius);

    const bool selected =
        isChecked();

    QColor containerColor;
    if (!isEnabled()) {
        containerColor =
            spec.disabledContainerColor;
    } else if (selected) {
        containerColor =
            spec.selectedContainerColor;
    } else {
        containerColor =
            spec.containerColor;
    }

    const QColor labelColor =
        !isEnabled()
            ? spec.disabledLabelColor
            : (
                selected
                    ? spec.selectedLabelColor
                    : spec.labelColor);

    const QColor iconColor =
        !isEnabled()
            ? spec.disabledLabelColor
            : (
                selected
                    ? spec.selectedIconColor
                    : spec.iconColor);

    if (
        containerColor.isValid()
        && containerColor.alpha() > 0) {
        painter.fillPath(
            path,
            containerColor);
    }

    const QColor outlineColor =
        isEnabled()
            ? spec.outlineColor
            : spec.disabledOutlineColor;

    if (
        outlineColor.isValid()
        && outlineColor.alpha() > 0
        && spec.outlineWidth > 0) {
        painter.setPen(
            QPen(
                outlineColor,
                spec.outlineWidth));
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(path);
    }

    const qreal opacity =
        stateOpacity(
            spec,
            interactionState());

    if (opacity > 0.0) {
        QtMaterialStateLayerPainter::
            paintPath(
                &painter,
                path,
                spec.stateLayerColor,
                opacity);
    }

    painter.setFont(
        spec.hasResolvedLabelFont
            ? spec.labelFont
            : font());

    const QRect outer =
        visualRect.toAlignedRect();

    QRect logicalContent =
        outer.adjusted(
            spec.horizontalPadding,
            0,
            -spec.horizontalPadding,
            0);

    int logicalLeft =
        logicalContent.left();
    int logicalRight =
        logicalContent.right();

    if (!icon().isNull()) {
        const QRect logicalIcon(
            logicalLeft,
            outer.center().y()
                - spec.iconSize / 2,
            spec.iconSize,
            spec.iconSize);

        const QRect visualIcon =
            style()->visualRect(
                layoutDirection(),
                outer,
                logicalIcon);

        paintTintedIcon(
            &painter,
            icon(),
            visualIcon,
            iconColor,
            isEnabled());

        logicalLeft +=
            spec.iconSize
            + spec.iconSpacing;
    }

    QRect closeRect;
    if (
        m_removable
        || !m_trailingIcon.isNull()) {
        const QRect logicalClose(
            logicalRight
                - spec.iconSize
                + 1,
            outer.center().y()
                - spec.iconSize / 2,
            spec.iconSize,
            spec.iconSize);

        closeRect =
            style()->visualRect(
                layoutDirection(),
                outer,
                logicalClose);

        logicalRight =
            logicalClose.left()
            - spec.iconSpacing;
    }

    const QRect logicalText(
        logicalLeft,
        outer.top(),
        qMax(
            0,
            logicalRight
                - logicalLeft
                + 1),
        outer.height());

    const QRect visualText =
        style()->visualRect(
            layoutDirection(),
            outer,
            logicalText);

    painter.setPen(labelColor);
    painter.drawText(
        visualText,
        Qt::AlignVCenter
            | (
                layoutDirection()
                    == Qt::RightToLeft
                    ? Qt::AlignRight
                    : Qt::AlignLeft),
        text());

    if (!closeRect.isNull()) {
        if (!m_trailingIcon.isNull()) {
            paintTintedIcon(
                &painter,
                m_trailingIcon,
                closeRect,
                iconColor,
                isEnabled());
        } else {
            const int inset =
                qMax(
                    3,
                    spec.iconSize / 4);

            painter.setPen(
                QPen(
                    iconColor,
                    1.8,
                    Qt::SolidLine,
                    Qt::RoundCap));

            painter.drawLine(
                closeRect.topLeft()
                    + QPoint(inset, inset),
                closeRect.bottomRight()
                    - QPoint(inset, inset));

            painter.drawLine(
                closeRect.topRight()
                    + QPoint(-inset, inset),
                closeRect.bottomLeft()
                    + QPoint(inset, -inset));
        }
    }

    if (
        interactionState().isFocused()
        && spec.focusRingWidth > 0.0) {
        QtMaterialFocusIndicator::
            paintPathFocusRing(
                &painter,
                path,
                spec.focusRingColor,
                spec.focusRingWidth);
    }
}

} // namespace QtMaterial
