#include "qtmaterial/widgets/selection/qtmaterialswitch.h"

#include <QChangeEvent>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>

#include "private/qtmaterialselectionrenderhelper_p.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

QtMaterialSwitch::QtMaterialSwitch(QWidget* parent)
    : QtMaterialSelectionControl(parent)
    , m_transition(new QtMaterialTransitionController(this))
    , m_ripple(new QtMaterialRippleController(this))
{
    setCheckable(true);
    setFocusPolicy(Qt::StrongFocus);

    QObject::connect(m_transition, &QtMaterialTransitionController::progressChanged, this, [this](qreal) {
        m_layoutDirty = true;
        update();
    });
    QObject::connect(this, &QAbstractButton::toggled, this, [this](bool) {
        syncTransitionState(true);
    });

    syncTransitionState(false);
}

QtMaterialSwitch::~QtMaterialSwitch() = default;

void QtMaterialSwitch::themeChangedEvent(const Theme& theme)
{
    QtMaterialSelectionControl::themeChangedEvent(theme);
    m_specDirty = true;
    m_layoutDirty = true;
    updateGeometry();
    update();
}

void QtMaterialSwitch::invalidateResolvedSpec()
{
    m_specDirty = true;
    m_layoutDirty = true;
}

void QtMaterialSwitch::stateChangedEvent()
{
    QtMaterialSelectionControl::stateChangedEvent();
    update();
}

void QtMaterialSwitch::resolveSpecIfNeeded() const
{
    if (!m_specDirty) {
        return;
    }

    SpecFactory factory;
    m_spec = factory.switchSpec(theme(), density());
    const_cast<QtMaterialSwitch*>(this)->setSpacing(m_spec.spacing);
    m_specDirty = false;
    m_layoutDirty = true;
}

void QtMaterialSwitch::invalidateLayoutCache()
{
    m_layoutDirty = true;
}

void QtMaterialSwitch::resolveLayoutIfNeeded() const
{
    const_cast<QtMaterialSwitch*>(this)->resolveSpecIfNeeded();
    if (!m_layoutDirty) {
        return;
    }

    const qreal progress = m_transition ? m_transition->progress() : (isChecked() ? 1.0 : 0.0);

    m_cachedTrackRect = SelectionRenderHelper::switchTrackRect(indicatorRect(), m_spec);
    m_cachedStateLayerRect = SelectionRenderHelper::centeredStateLayerRect(indicatorRect(), m_spec.stateLayerSize);
    m_cachedTrackPath = SelectionRenderHelper::roundedTrackPath(m_cachedTrackRect);
    m_cachedHandleRect = SelectionRenderHelper::switchHandleRect(m_cachedTrackRect, m_spec, progress);
    m_cachedFocusRingRect = m_cachedTrackRect.adjusted(-3.0, -3.0, 3.0, 3.0);
    m_cachedLabelRect = labelRect();

    const QFont labelFont = SelectionRenderHelper::labelFont(theme(), m_spec.labelTypeRole, font());
    const QFontMetrics metrics(labelFont);
    m_cachedElidedText = metrics.elidedText(text(), Qt::ElideRight, m_cachedLabelRect.width());

    m_layoutDirty = false;
}

void QtMaterialSwitch::syncTransitionState(bool animated)
{
    if (!m_transition) {
        return;
    }

    if (animated) {
        if (isChecked()) {
            m_transition->startForward();
        } else {
            m_transition->startBackward();
        }
    } else {
        m_transition->setProgress(isChecked() ? 1.0 : 0.0);
    }
}

QSize QtMaterialSwitch::sizeHint() const
{
    const_cast<QtMaterialSwitch*>(this)->resolveSpecIfNeeded();

    const QFont labelFont = SelectionRenderHelper::labelFont(theme(), m_spec.labelTypeRole, font());
    const QFontMetrics metrics(labelFont);
    const int labelWidth = text().isEmpty() ? 0 : (metrics.horizontalAdvance(text()) + spacing());
    const int width = qMax(m_spec.touchTarget.width(), m_spec.trackWidth) + labelWidth;
    const int height = qMax(m_spec.touchTarget.height(), qMax(m_spec.trackHeight, metrics.height()));
    return QSize(width, height);
}

QSize QtMaterialSwitch::minimumSizeHint() const
{
    const_cast<QtMaterialSwitch*>(this)->resolveSpecIfNeeded();
    return QSize(m_spec.touchTarget.width(), qMax(m_spec.touchTarget.height(), m_spec.trackHeight));
}

void QtMaterialSwitch::resizeEvent(QResizeEvent* event)
{
    invalidateLayoutCache();
    QtMaterialSelectionControl::resizeEvent(event);
}

void QtMaterialSwitch::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::FontChange || event->type() == QEvent::StyleChange || event->type() == QEvent::EnabledChange) {
        invalidateLayoutCache();
    }
    QtMaterialSelectionControl::changeEvent(event);
}

void QtMaterialSwitch::mousePressEvent(QMouseEvent* event)
{
    if (m_ripple) {
        m_ripple->addRipple(event->position());
    }
    QtMaterialSelectionControl::mousePressEvent(event);
}

void QtMaterialSwitch::paintEvent(QPaintEvent*)
{
    resolveLayoutIfNeeded();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const bool enabled = isEnabled();
    const qreal progress = m_transition ? m_transition->progress() : (isChecked() ? 1.0 : 0.0);
    const qreal stateOpacity = SelectionRenderHelper::stateLayerOpacity(theme(), interactionState());

    const QColor trackColor = enabled
        ? (isChecked() ? m_spec.selectedTrackColor : m_spec.unselectedTrackColor)
        : (isChecked() ? m_spec.disabledSelectedTrackColor : m_spec.disabledUnselectedTrackColor);

    const QColor handleColor = enabled
        ? (isChecked() ? m_spec.selectedHandleColor : m_spec.unselectedHandleColor)
        : (isChecked() ? m_spec.disabledSelectedHandleColor : m_spec.disabledUnselectedHandleColor);

    SelectionRenderHelper::paintCircularStateLayer(&painter, m_cachedStateLayerRect, m_spec.stateLayerColor, stateOpacity);

    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(trackColor);
    painter.drawPath(m_cachedTrackPath);
    painter.setBrush(handleColor);
    painter.drawEllipse(m_cachedHandleRect);
    painter.restore();

    if (m_ripple) {
        QPainterPath clip;
        clip.addRoundedRect(m_cachedTrackRect, m_cachedTrackRect.height() / 2.0, m_cachedTrackRect.height() / 2.0);
        m_ripple->setClipPath(clip);
        m_ripple->paint(&painter, m_spec.stateLayerColor);
    }

    if (!m_cachedElidedText.isEmpty()) {
        const QFont labelFont = SelectionRenderHelper::labelFont(theme(), m_spec.labelTypeRole, font());
        const QColor labelColor = enabled ? m_spec.labelColor : m_spec.disabledLabelColor;
        SelectionRenderHelper::paintLabel(&painter, m_cachedLabelRect, labelAlignment(), m_cachedElidedText, labelColor, labelFont);
    }

    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintRectFocusRing(&painter,
                                                     m_cachedFocusRingRect,
                                                     m_spec.focusRingColor,
                                                     m_cachedFocusRingRect.height() / 2.0,
                                                     2.0);
    }
}

} // namespace QtMaterial
