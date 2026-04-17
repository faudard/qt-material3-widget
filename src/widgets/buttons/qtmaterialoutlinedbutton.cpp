#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"

#include <QEvent>
#include <QFontMetrics>
#include <QIcon>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/core/qtmaterialeventcompat.h"
#include "qtmaterial/theme/qtmaterialtypographyscale.h"

namespace QtMaterial {
namespace {

QRect centeredContentRect(const QRect& outerRect, int visualHeight)
{
    const int height = qMin(visualHeight, outerRect.height());
    const int top = outerRect.y() + (outerRect.height() - height) / 2;
    return QRect(outerRect.x(), top, outerRect.width(), height);
}

QColor withOpacity(const QColor& color, qreal opacity)
{
    QColor result = color;
    result.setAlphaF(qBound<qreal>(0.0, opacity, 1.0));
    return result;
}

} // namespace

QtMaterialOutlinedButton::QtMaterialOutlinedButton(QWidget* parent)
    : QtMaterialAbstractButton(parent)
    , m_ripple(new QtMaterialRippleController(this))
{
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    setMinimumHeight(48);
}

QtMaterialOutlinedButton::QtMaterialOutlinedButton(const QString& text, QWidget* parent)
    : QtMaterialOutlinedButton(parent)
{
    setText(text);
}

QtMaterialOutlinedButton::~QtMaterialOutlinedButton() = default;

QSize QtMaterialOutlinedButton::sizeHint() const
{
    const_cast<QtMaterialOutlinedButton*>(this)->resolveSpecIfNeeded();

    QFont font = this->font();
    const TypographyStyle typography = theme().typography().style(m_spec.labelTypeRole);
    if (!typography.font.family().isEmpty()) {
        font = typography.font;
    }

    const QFontMetrics metrics(font);
    int width = (m_spec.horizontalPadding * 2) + metrics.horizontalAdvance(text());

    if (!icon().isNull()) {
        width += m_spec.iconSize;
        if (!text().isEmpty()) {
            width += m_spec.iconSpacing;
        }
    }

    width = qMax(width, m_spec.touchTarget.width());
    const int height = qMax(m_spec.touchTarget.height(), m_spec.containerHeight);
    return QSize(width, height);
}

QSize QtMaterialOutlinedButton::minimumSizeHint() const
{
    const_cast<QtMaterialOutlinedButton*>(this)->resolveSpecIfNeeded();
    return QSize(m_spec.touchTarget.width(), qMax(m_spec.touchTarget.height(), m_spec.containerHeight));
}

void QtMaterialOutlinedButton::changeEvent(QEvent* event)
{
    if (event && (event->type() == QEvent::FontChange || event->type() == QEvent::StyleChange)) {
        invalidateResolvedSpec();
        updateGeometry();
    }
    QtMaterialAbstractButton::changeEvent(event);
}

void QtMaterialOutlinedButton::themeChangedEvent(const Theme& theme)
{
    QtMaterialAbstractButton::themeChangedEvent(theme);
    Q_UNUSED(theme)
    m_specDirty = true;
}

void QtMaterialOutlinedButton::invalidateResolvedSpec()
{
    m_specDirty = true;
}

void QtMaterialOutlinedButton::mousePressEvent(QMouseEvent* event)
{
    if (event && event->button() == Qt::LeftButton && m_ripple) {
        m_ripple->addRipple(QtMaterial::mousePosition(event));
    }
    QtMaterialAbstractButton::mousePressEvent(event);
}

void QtMaterialOutlinedButton::resolveSpecIfNeeded() const
{
    if (!m_specDirty) {
        return;
    }

    SpecFactory factory;
    m_spec = factory.outlinedButtonSpec(theme(), density());
    m_specDirty = false;
}

QRect QtMaterialOutlinedButton::containerRect() const
{
    resolveSpecIfNeeded();
    return centeredContentRect(rect(), m_spec.containerHeight);
}

QRect QtMaterialOutlinedButton::contentRectForPaint() const
{
    const QRect visualRect = containerRect();
    return visualRect.adjusted(m_spec.horizontalPadding, 0, -m_spec.horizontalPadding, 0);
}

QRect QtMaterialOutlinedButton::iconRectForContent(const QRect& contentRect) const
{
    if (icon().isNull()) {
        return QRect();
    }

    const QSize iconSize(m_spec.iconSize, m_spec.iconSize);
    const QFont font = theme().typography().contains(m_spec.labelTypeRole)
        ? theme().typography().style(m_spec.labelTypeRole).font
        : this->font();
    const QFontMetrics metrics(font);
    const int textWidth = text().isEmpty() ? 0 : metrics.horizontalAdvance(text());
    const int totalWidth = iconSize.width() + (textWidth > 0 ? m_spec.iconSpacing + textWidth : 0);
    const int x = contentRect.x() + (contentRect.width() - totalWidth) / 2;
    const int y = contentRect.y() + (contentRect.height() - iconSize.height()) / 2;
    return QRect(x, y, iconSize.width(), iconSize.height());
}

QRect QtMaterialOutlinedButton::textRectForContent(const QRect& contentRect) const
{
    const QFont font = theme().typography().contains(m_spec.labelTypeRole)
        ? theme().typography().style(m_spec.labelTypeRole).font
        : this->font();
    const QFontMetrics metrics(font);
    const int textWidth = metrics.horizontalAdvance(text());
    const int textHeight = metrics.height();

    if (icon().isNull()) {
        const int x = contentRect.x() + qMax(0, (contentRect.width() - textWidth) / 2);
        const int y = contentRect.y() + (contentRect.height() - textHeight) / 2;
        return QRect(x, y, qMin(contentRect.width(), textWidth), textHeight);
    }

    const QRect iconRect = iconRectForContent(contentRect);
    const int x = iconRect.right() + 1 + m_spec.iconSpacing;
    const int availableWidth = qMax(0, contentRect.right() - x + 1);
    const int y = contentRect.y() + (contentRect.height() - textHeight) / 2;
    return QRect(x, y, availableWidth, textHeight);
}

QPainterPath QtMaterialOutlinedButton::containerPath() const
{
    const QRect bounds = containerRect();
    const qreal radius = containerRadius(bounds);

    QPainterPath path;
    path.addRoundedRect(QRectF(bounds), radius, radius);
    return path;
}

qreal QtMaterialOutlinedButton::containerRadius(const QRect& rect) const
{
    const int radius = theme().shapes().radius(m_spec.shapeRole);
    if (m_spec.shapeRole == ShapeRole::Full) {
        return rect.height() / 2.0;
    }
    return qMin<qreal>(radius, rect.height() / 2.0);
}

qreal QtMaterialOutlinedButton::stateLayerOpacity() const
{
    const StateLayer& stateLayer = theme().stateLayer();
    if (!isEnabled()) {
        return 0.0;
    }
    if (interactionState().isPressed()) {
        return stateLayer.pressOpacity;
    }
    if (interactionState().isFocused()) {
        return stateLayer.focusOpacity;
    }
    if (interactionState().isHovered()) {
        return stateLayer.hoverOpacity;
    }
    return 0.0;
}

void QtMaterialOutlinedButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    resolveSpecIfNeeded();

    const QRect visualRect = containerRect();
    const QRect contentRect = contentRectForPaint();
    const qreal radius = containerRadius(visualRect);
    const QPainterPath clipPath = containerPath();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const qreal overlayOpacity = stateLayerOpacity();
    if (overlayOpacity > 0.0) {
        QtMaterialStateLayerPainter::paintPath(
            &painter,
            clipPath,
            m_spec.stateLayerColor,
            overlayOpacity
        );
    }

    if (m_ripple) {
        m_ripple->setClipPath(clipPath);
        m_ripple->paint(&painter, withOpacity(m_spec.stateLayerColor, theme().stateLayer().pressOpacity));
    }

    painter.save();
    QPen pen(isEnabled() ? m_spec.outlineColor : m_spec.disabledOutlineColor, 1.0);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(clipPath);
    painter.restore();

    QFont labelFont = font();
    if (theme().typography().contains(m_spec.labelTypeRole)) {
        labelFont = theme().typography().style(m_spec.labelTypeRole).font;
    }
    painter.setFont(labelFont);

    const QColor contentColor = isEnabled() ? m_spec.labelColor : m_spec.disabledLabelColor;

    if (!icon().isNull()) {
        const QRect iconRect = iconRectForContent(contentRect);
        const QPixmap iconPixmap = icon().pixmap(QSize(m_spec.iconSize, m_spec.iconSize), isEnabled() ? QIcon::Normal : QIcon::Disabled);
        painter.drawPixmap(iconRect, iconPixmap);
    }

    painter.setPen(contentColor);
    const QRect labelRect = textRectForContent(contentRect);
    const QString elided = painter.fontMetrics().elidedText(text(), Qt::ElideRight, labelRect.width());
    painter.drawText(labelRect, Qt::AlignVCenter | Qt::AlignLeft, elided);

    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintRectFocusRing(
            &painter,
            QRectF(visualRect).adjusted(2.0, 2.0, -2.0, -2.0),
            m_spec.stateLayerColor,
            radius,
            2.0
        );
    }
}

} // namespace QtMaterial
