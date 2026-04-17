#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"

#include <QEvent>
#include <QFontMetrics>
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

QColor withOpacity(const QColor& color, qreal opacity)
{
    QColor result = color;
    result.setAlphaF(qBound<qreal>(0.0, opacity, 1.0));
    return result;
}

QRect centeredSquare(const QRect& bounds, int size)
{
    const int x = bounds.x() + (bounds.width() - size) / 2;
    const int y = bounds.y() + (bounds.height() - size) / 2;
    return QRect(x, y, size, size);
}

} // namespace

QtMaterialCheckbox::QtMaterialCheckbox(QWidget* parent)
    : QtMaterialSelectionControl(parent)
    , m_ripple(new QtMaterialRippleController(this))
{
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    setMinimumHeight(48);
}

QtMaterialCheckbox::QtMaterialCheckbox(const QString& text, QWidget* parent)
    : QtMaterialCheckbox(parent)
{
    setText(text);
}

QtMaterialCheckbox::~QtMaterialCheckbox() = default;

QSize QtMaterialCheckbox::sizeHint() const
{
    const_cast<QtMaterialCheckbox*>(this)->resolveSpecIfNeeded();

    QFont labelFont = font();
    if (theme().typography().contains(m_spec.labelTypeRole)) {
        labelFont = theme().typography().style(m_spec.labelTypeRole).font;
    }

    const QFontMetrics metrics(labelFont);
    const int labelWidth = text().isEmpty() ? 0 : metrics.horizontalAdvance(text());
    const int width = m_spec.touchTarget.width() + (text().isEmpty() ? 0 : m_spec.spacing + labelWidth);
    const int height = qMax(m_spec.touchTarget.height(), metrics.height() + 12);
    return QSize(width, height);
}

QSize QtMaterialCheckbox::minimumSizeHint() const
{
    const_cast<QtMaterialCheckbox*>(this)->resolveSpecIfNeeded();
    return QSize(m_spec.touchTarget.width(), m_spec.touchTarget.height());
}

void QtMaterialCheckbox::changeEvent(QEvent* event)
{
    if (event && (event->type() == QEvent::FontChange || event->type() == QEvent::StyleChange)) {
        invalidateResolvedSpec();
        updateGeometry();
    }
    QtMaterialSelectionControl::changeEvent(event);
}

void QtMaterialCheckbox::themeChangedEvent(const Theme& theme)
{
    QtMaterialSelectionControl::themeChangedEvent(theme);
    Q_UNUSED(theme)
    m_specDirty = true;
}

void QtMaterialCheckbox::invalidateResolvedSpec()
{
    m_specDirty = true;
}

void QtMaterialCheckbox::mousePressEvent(QMouseEvent* event)
{
    if (event && event->button() == Qt::LeftButton && m_ripple) {
        m_ripple->addRipple(QtMaterial::mousePosition(event));
    }
    QtMaterialSelectionControl::mousePressEvent(event);
}

void QtMaterialCheckbox::resolveSpecIfNeeded() const
{
    if (!m_specDirty) {
        return;
    }

    SpecFactory factory;
    m_spec = factory.checkboxSpec(theme(), density());
    m_specDirty = false;
}

QRect QtMaterialCheckbox::touchTargetRect() const
{
    resolveSpecIfNeeded();
    return centeredSquare(rect(), m_spec.touchTarget.height()).translated(0, 0);
}

QRect QtMaterialCheckbox::indicatorRectForPaint() const
{
    const QRect target = touchTargetRect();
    return centeredSquare(target, m_spec.indicatorSize);
}

QRect QtMaterialCheckbox::stateLayerRect() const
{
    const QRect target = touchTargetRect();
    return centeredSquare(target, m_spec.stateLayerSize);
}

QRect QtMaterialCheckbox::labelRectForPaint() const
{
    const QRect target = touchTargetRect();
    const int left = target.right() + 1 + m_spec.spacing;
    const int width = qMax(0, rect().right() - left + 1);
    return QRect(left, 0, width, height());
}

QPainterPath QtMaterialCheckbox::indicatorPath() const
{
    const QRect indicator = indicatorRectForPaint();
    QPainterPath path;
    path.addRoundedRect(QRectF(indicator), m_spec.cornerRadius, m_spec.cornerRadius);
    return path;
}

qreal QtMaterialCheckbox::stateLayerOpacity() const
{
    if (!isEnabled()) {
        return 0.0;
    }

    const StateLayer& stateLayer = theme().stateLayer();
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

void QtMaterialCheckbox::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    resolveSpecIfNeeded();

    const QRect targetRect = touchTargetRect();
    const QRect indicatorRect = indicatorRectForPaint();
    const QRect stateRect = stateLayerRect();
    const QRect labelRect = labelRectForPaint();
    const QPainterPath boxPath = indicatorPath();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const qreal overlayOpacity = stateLayerOpacity();
    if (overlayOpacity > 0.0) {
        QtMaterialStateLayerPainter::paintRect(
            &painter,
            QRectF(stateRect),
            m_spec.stateLayerColor,
            overlayOpacity,
            stateRect.height() / 2.0
        );
    }

    if (m_ripple) {
        QPainterPath ripplePath;
        ripplePath.addEllipse(QRectF(stateRect));
        m_ripple->setClipPath(ripplePath);
        m_ripple->paint(&painter, withOpacity(m_spec.stateLayerColor, theme().stateLayer().pressOpacity));
    }

    const bool enabled = isEnabled();
    const bool checked = isChecked();

    const QColor outlineColor = enabled
        ? (checked ? m_spec.selectedContainerColor : m_spec.unselectedOutlineColor)
        : (checked ? m_spec.disabledSelectedContainerColor : m_spec.disabledUnselectedOutlineColor);

    const QColor fillColor = checked
        ? (enabled ? m_spec.selectedContainerColor : m_spec.disabledSelectedContainerColor)
        : Qt::transparent;

    painter.save();
    QPen outlinePen(outlineColor, m_spec.outlineWidth);
    outlinePen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(outlinePen);
    painter.setBrush(fillColor);
    painter.drawPath(boxPath);
    painter.restore();

    if (checked) {
        painter.save();
        QPen checkPen(enabled ? m_spec.selectedIconColor : m_spec.disabledLabelColor, m_spec.checkmarkStrokeWidth);
        checkPen.setCapStyle(Qt::RoundCap);
        checkPen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(checkPen);

        const QPointF p1(indicatorRect.left() + indicatorRect.width() * 0.22, indicatorRect.top() + indicatorRect.height() * 0.55);
        const QPointF p2(indicatorRect.left() + indicatorRect.width() * 0.45, indicatorRect.top() + indicatorRect.height() * 0.78);
        const QPointF p3(indicatorRect.left() + indicatorRect.width() * 0.80, indicatorRect.top() + indicatorRect.height() * 0.28);
        painter.drawLine(p1, p2);
        painter.drawLine(p2, p3);
        painter.restore();
    }

    QFont labelFont = font();
    if (theme().typography().contains(m_spec.labelTypeRole)) {
        labelFont = theme().typography().style(m_spec.labelTypeRole).font;
    }
    painter.setFont(labelFont);
    painter.setPen(enabled ? m_spec.labelColor : m_spec.disabledLabelColor);
    painter.drawText(labelRect, labelAlignment() | Qt::TextSingleLine, painter.fontMetrics().elidedText(text(), Qt::ElideRight, labelRect.width()));

    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintRectFocusRing(
            &painter,
            QRectF(stateRect).adjusted(1.0, 1.0, -1.0, -1.0),
            m_spec.focusRingColor,
            stateRect.height() / 2.0,
            2.0
        );
    }
}

} // namespace QtMaterial
