#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"

#include <QMouseEvent>

#include "qtmaterial/core/qtmaterialeventcompat.h"

#include <QPainter>
#include <QPainterPath>

#include "private/qtmaterialselectionrenderhelper_p.h"

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

namespace {

qreal lerp(qreal a, qreal b, qreal t)
{
    return a + (b - a) * t;
}

QPointF lerpPoint(const QPointF& a, const QPointF& b, qreal t)
{
    return QPointF(lerp(a.x(), b.x(), t), lerp(a.y(), b.y(), t));
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

} // namespace

QtMaterialCheckbox::QtMaterialCheckbox(QWidget* parent)
    : QtMaterialSelectionControl(parent)
    , m_ripple(new QtMaterialRippleController(this))
    , m_transition(new QtMaterialTransitionController(this))
{
    setCheckable(true);
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

QtMaterialCheckbox::~QtMaterialCheckbox() = default;

void QtMaterialCheckbox::themeChangedEvent(const Theme& theme)
{
    QtMaterialSelectionControl::themeChangedEvent(theme);
    m_specDirty = true;
}

void QtMaterialCheckbox::invalidateResolvedSpec()
{
    m_specDirty = true;
}

void QtMaterialCheckbox::resolveSpecIfNeeded() const
{
    if (!m_specDirty) {
        return;
    }

    SpecFactory factory;
    m_spec = factory.checkboxSpec(theme(), density());

    const_cast<QtMaterialCheckbox*>(this)->setSpacing(m_spec.spacing);

    if (m_transition && theme().motion().contains(m_spec.motionToken)) {
        const MotionStyle motion = theme().motion().style(m_spec.motionToken);
        if (motion.durationMs > 0) {
            m_transition->setDuration(motion.durationMs);
        }
        m_transition->setEasingCurve(motion.easing);
    }

    m_specDirty = false;
}

void QtMaterialCheckbox::mousePressEvent(QMouseEvent* event)
{
    if (m_ripple) {
        m_ripple->addRipple(QtMaterial::mousePosition(event));
    }

    QtMaterialSelectionControl::mousePressEvent(event);
}

void QtMaterialCheckbox::paintEvent(QPaintEvent*)
{
    resolveSpecIfNeeded();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRect box = indicatorRect();
    const QRectF stateLayerRect = SelectionRenderHelper::centeredStateLayerRect(box, m_spec.stateLayerSize);
    const qreal stateOpacity = SelectionRenderHelper::stateLayerOpacity(theme(), interactionState());
    const qreal progress = m_transition ? m_transition->progress() : (isChecked() ? 1.0 : 0.0);

    SelectionRenderHelper::paintCircularStateLayer(&painter, stateLayerRect, m_spec.stateLayerColor, stateOpacity);

    QPainterPath rippleClip;
    rippleClip.addEllipse(stateLayerRect);

    if (m_ripple) {
        m_ripple->setClipPath(rippleClip);
        m_ripple->paint(&painter, m_spec.stateLayerColor);
    }

    const bool enabled = isEnabled();
    const QColor selectedContainer =
        enabled ? m_spec.selectedContainerColor : m_spec.disabledSelectedContainerColor;
    const QColor unselectedOutline =
        enabled ? m_spec.unselectedOutlineColor : m_spec.disabledUnselectedOutlineColor;

    const QColor outlineColor = blendColor(unselectedOutline, selectedContainer, progress);

    QColor fillColor = selectedContainer;
    fillColor.setAlphaF(selectedContainer.alphaF() * progress);

    painter.save();
    painter.setPen(QPen(outlineColor, m_spec.outlineWidth));
    painter.setBrush(fillColor);
    painter.drawRoundedRect(QRectF(box), m_spec.cornerRadius, m_spec.cornerRadius);

    if (progress > 0.0) {
        QColor iconColor = m_spec.selectedIconColor;
        iconColor.setAlphaF(iconColor.alphaF() * progress);

        painter.setPen(QPen(
            iconColor,
            m_spec.checkmarkStrokeWidth,
            Qt::SolidLine,
            Qt::RoundCap,
            Qt::RoundJoin));

        const QPointF p1(box.left() + box.width() * 0.22, box.top() + box.height() * 0.55);
        const QPointF p2(box.left() + box.width() * 0.45, box.bottom() - box.height() * 0.22);
        const QPointF p3(box.right() - box.width() * 0.18, box.top() + box.height() * 0.22);

        if (progress < 0.5) {
            const qreal t = progress / 0.5;
            painter.drawLine(p1, lerpPoint(p1, p2, t));
        } else {
            painter.drawLine(p1, p2);
            const qreal t = (progress - 0.5) / 0.5;
            painter.drawLine(p2, lerpPoint(p2, p3, t));
        }
    }

    painter.restore();

    const QFont labelFont = SelectionRenderHelper::labelFont(theme(), m_spec.labelTypeRole, font());
    const QColor labelColor = enabled ? m_spec.labelColor : m_spec.disabledLabelColor;

    SelectionRenderHelper::paintLabel(&painter, labelRect(), labelAlignment(), text(), labelColor, labelFont);

    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintRectFocusRing(
            &painter,
            stateLayerRect,
            m_spec.focusRingColor,
            stateLayerRect.height() / 2.0,
            2.0);
    }
}

} // namespace QtMaterial