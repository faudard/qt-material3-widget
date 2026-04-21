#include "qtmaterial/widgets/selection/qtmaterialradiobutton.h"

// #include <QChangeEvent>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>

#include "private/qtmaterialselectionrenderhelper_p.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/core/qtmaterialeventcompat.h"

namespace QtMaterial {

namespace {
QRectF centeredRect(const QRectF& outer, qreal size)
{
    return QRectF(outer.center().x() - size / 2.0,
                  outer.center().y() - size / 2.0,
                  size,
                  size);
}

qreal lerp(qreal a, qreal b, qreal t)
{
    return a + (b - a) * t;
}

QColor blendColor(const QColor& a, const QColor& b, qreal t)
{
    QColor out;
    out.setRedF(lerp(a.redF(), b.redF(), t));
    out.setGreenF(lerp(a.greenF(), b.greenF(), t));
    out.setBlueF(lerp(a.blueF(), b.blueF(), t));
    out.setAlphaF(lerp(a.alphaF(), b.alphaF(), t));
    return out;
}
}

QtMaterialRadioButton::QtMaterialRadioButton(QWidget* parent)
    : QtMaterialSelectionControl(parent)
    , m_ripple(new QtMaterialRippleController(this))
    , m_transition(new QtMaterialTransitionController(this))
{
    setCheckable(true);
    setAutoExclusive(false);
    setFocusPolicy(Qt::StrongFocus);

    m_transition->setProgress(isChecked() ? 1.0 : 0.0);

    QObject::connect(m_transition, &QtMaterialTransitionController::progressChanged, this, [this](qreal) {
        update();
    });

    QObject::connect(this, &QAbstractButton::toggled, this, [this](bool checked) {
        resolveSpecIfNeeded();
        if (!m_transition) {
            return;
        }
        checked ? m_transition->startForward() : m_transition->startBackward();
    });
}

QtMaterialRadioButton::QtMaterialRadioButton(const QString& text, QWidget* parent)
    : QtMaterialRadioButton(parent)
{
    setText(text);
}

QtMaterialRadioButton::~QtMaterialRadioButton() = default;

void QtMaterialRadioButton::themeChangedEvent(const Theme& theme)
{
    QtMaterialSelectionControl::themeChangedEvent(theme);
    m_specDirty = true;
    m_layoutDirty = true;
    updateGeometry();
    update();
}

void QtMaterialRadioButton::invalidateResolvedSpec()
{
    m_specDirty = true;
    m_layoutDirty = true;
}

void QtMaterialRadioButton::stateChangedEvent()
{
    QtMaterialSelectionControl::stateChangedEvent();
    update();
}

void QtMaterialRadioButton::resolveSpecIfNeeded() const
{
    if (!m_specDirty) {
        return;
    }

    SpecFactory factory;
    m_spec = factory.radioButtonSpec(theme(), density());
    const_cast<QtMaterialRadioButton*>(this)->setSpacing(m_spec.spacing);
    if (m_transition && theme().motion().contains(m_spec.motionToken)) {
        const MotionStyle motion = theme().motion().style(m_spec.motionToken);
        if (motion.durationMs > 0) {
            m_transition->setDuration(motion.durationMs);
        }
        m_transition->setEasingCurve(motion.easing);
    }
    m_specDirty = false;
    m_layoutDirty = true;
}

void QtMaterialRadioButton::invalidateLayoutCache()
{
    m_layoutDirty = true;
}

void QtMaterialRadioButton::resolveLayoutIfNeeded() const
{
    const_cast<QtMaterialRadioButton*>(this)->resolveSpecIfNeeded();
    if (!m_layoutDirty) {
        return;
    }

    const QRect bounds = rect();
    const QRect indicatorBounds(0,
                                (bounds.height() - m_spec.indicatorSize) / 2,
                                m_spec.indicatorSize,
                                m_spec.indicatorSize);

    m_cachedIndicatorRect = QRectF(indicatorBounds);
    m_cachedDotRect = centeredRect(m_cachedIndicatorRect, m_spec.dotSize);
    m_cachedStateLayerRect = SelectionRenderHelper::centeredStateLayerRect(indicatorBounds, m_spec.touchTarget.height());
    m_cachedFocusRingRect = m_cachedIndicatorRect.adjusted(-3.0, -3.0, 3.0, 3.0);
    m_cachedLabelRect = QRect(indicatorBounds.right() + 1 + spacing(), 0,
                              qMax(0, width() - indicatorBounds.width() - spacing() - 1), height());

    m_cachedIndicatorPath = QPainterPath();
    m_cachedIndicatorPath.addEllipse(m_cachedIndicatorRect);

    m_cachedRippleClipPath = QPainterPath();
    m_cachedRippleClipPath.addEllipse(m_cachedStateLayerRect);

    const QFont labelFont = SelectionRenderHelper::labelFont(theme(), m_spec.labelTypeRole, font());
    const QFontMetrics metrics(labelFont);
    m_cachedElidedText = metrics.elidedText(text(), Qt::ElideRight, m_cachedLabelRect.width());

    m_layoutDirty = false;
}

QSize QtMaterialRadioButton::sizeHint() const
{
    const_cast<QtMaterialRadioButton*>(this)->resolveSpecIfNeeded();

    const QFont labelFont = SelectionRenderHelper::labelFont(theme(), m_spec.labelTypeRole, font());
    const QFontMetrics metrics(labelFont);
    const int labelWidth = text().isEmpty() ? 0 : (metrics.horizontalAdvance(text()) + spacing());
    const int width = qMax(m_spec.touchTarget.width(), m_spec.indicatorSize) + labelWidth;
    const int height = qMax(m_spec.touchTarget.height(), qMax(m_spec.indicatorSize, metrics.height()));
    return QSize(width, height);
}

QSize QtMaterialRadioButton::minimumSizeHint() const
{
    const_cast<QtMaterialRadioButton*>(this)->resolveSpecIfNeeded();
    return QSize(m_spec.touchTarget.width(), qMax(m_spec.touchTarget.height(), m_spec.indicatorSize));
}

void QtMaterialRadioButton::resizeEvent(QResizeEvent* event)
{
    invalidateLayoutCache();
    QtMaterialSelectionControl::resizeEvent(event);
}

void QtMaterialRadioButton::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::FontChange || event->type() == QEvent::StyleChange || event->type() == QEvent::EnabledChange) {
        invalidateLayoutCache();
    }
    QtMaterialSelectionControl::changeEvent(event);
}

void QtMaterialRadioButton::mousePressEvent(QMouseEvent* event)
{
    if (m_ripple) {
        m_ripple->addRipple(QtMaterial::mousePosition(event));
    }
    QtMaterialSelectionControl::mousePressEvent(event);
}

void QtMaterialRadioButton::paintEvent(QPaintEvent*)
{
    resolveLayoutIfNeeded();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const bool enabled = isEnabled();
    const qreal progress = m_transition ? m_transition->progress() : (isChecked() ? 1.0 : 0.0);
    const qreal stateOpacity = SelectionRenderHelper::stateLayerOpacity(theme(), interactionState());

    SelectionRenderHelper::paintCircularStateLayer(
        &painter,
        m_cachedStateLayerRect,
        m_spec.stateLayerColor,
        stateOpacity);

    painter.save();

    const QColor ringColor = enabled
                                 ? blendColor(m_spec.unselectedOutlineColor, m_spec.selectedColor, progress)
                                 : m_spec.disabledColor;

    QPen pen(ringColor, m_spec.outlineWidth);
    pen.setCosmetic(true);

    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(m_cachedIndicatorPath);

    if (progress > 0.0) {
        QRectF dotRect = centeredRect(m_cachedIndicatorRect, m_spec.dotSize * progress);

        QColor dotColor = enabled ? m_spec.selectedColor : m_spec.disabledColor;
        dotColor.setAlphaF(dotColor.alphaF() * progress);

        painter.setPen(Qt::NoPen);
        painter.setBrush(dotColor);
        painter.drawEllipse(dotRect);
    }

    painter.restore();

    if (m_ripple) {
        m_ripple->setClipPath(m_cachedRippleClipPath);
        m_ripple->paint(&painter, m_spec.stateLayerColor);
    }

    if (!m_cachedElidedText.isEmpty()) {
        const QFont labelFont = SelectionRenderHelper::labelFont(theme(), m_spec.labelTypeRole, font());
        const QColor labelColor = enabled ? m_spec.labelColor : m_spec.disabledColor;

        SelectionRenderHelper::paintLabel(
            &painter,
            m_cachedLabelRect,
            labelAlignment(),
            m_cachedElidedText,
            labelColor,
            labelFont);
    }

    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintRectFocusRing(
            &painter,
            m_cachedFocusRingRect,
            m_spec.focusRingColor,
            m_cachedFocusRingRect.width() / 2.0,
            2.0);
    }
}

} // namespace QtMaterial
