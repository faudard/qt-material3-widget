#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"
#include <memory>

#include <QMouseEvent>
#include <QPainter>

#include "qtmaterial/core/qtmaterialeventcompat.h"
#include "private/qtmaterialselectionrenderhelper_p.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialcheckboxspec.h"
#include "qtmaterial/specs/qtmaterialselectionspecresolver.h"

namespace QtMaterial {


struct QtMaterialCheckboxPrivate
{
 bool m_specDirty = true;
 CheckboxSpec m_spec;
 Qt::CheckState m_checkState = Qt::Unchecked;
 bool m_tristate = false;
 QtMaterialRippleController* m_ripple = nullptr;
 QtMaterialTransitionController* m_transition = nullptr;
};

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
 , d(std::make_unique<QtMaterialCheckboxPrivate>())
{
 d->m_ripple = new QtMaterialRippleController(this);
 d->m_transition = new QtMaterialTransitionController(this);

    setCheckable(true);
    setFocusPolicy(Qt::StrongFocus);
    syncCheckedFromCheckState();

    if (d->m_transition) {
        d->m_transition->setProgress(targetTransitionProgress());
        QObject::connect(d->m_transition,
                         &QtMaterialTransitionController::progressChanged,
                         this,
                         [this](qreal) { update(); });
    }
}

QtMaterialCheckbox::~QtMaterialCheckbox() = default;

bool QtMaterialCheckbox::isTristate() const noexcept
{
    return d->m_tristate;
}

void QtMaterialCheckbox::setTristate(bool tristate)
{
    if (d->m_tristate == tristate) {
        return;
    }

    d->m_tristate = tristate;
    if (!d->m_tristate && d->m_checkState == Qt::PartiallyChecked) {
        setCheckState(Qt::Unchecked);
    }
}

Qt::CheckState QtMaterialCheckbox::checkState() const noexcept
{
    return d->m_checkState;
}

void QtMaterialCheckbox::setCheckState(Qt::CheckState state)
{
    if (!d->m_tristate && state == Qt::PartiallyChecked) {
        state = Qt::Unchecked;
    }

    if (d->m_checkState == state) {
        return;
    }

    d->m_checkState = state;
    syncCheckedFromCheckState();

    if (d->m_transition) {
        const qreal target = targetTransitionProgress();
        d->m_transition->setProgress(target);
    }

    emit checkStateChanged(d->m_checkState);
    update();
}

void QtMaterialCheckbox::syncCheckedFromCheckState()
{
    const bool selected = (d->m_checkState != Qt::Unchecked);

    if (QAbstractButton::isChecked() != selected) {
        QAbstractButton::setChecked(selected);
    }
}

qreal QtMaterialCheckbox::targetTransitionProgress() const noexcept
{
    switch (d->m_checkState) {
    case Qt::PartiallyChecked:
        return 0.5;
    case Qt::Checked:
        return 1.0;
    case Qt::Unchecked:
    default:
        return 0.0;
    }
}

void QtMaterialCheckbox::themeChangedEvent(const Theme& theme)
{
    QtMaterialSelectionControl::themeChangedEvent(theme);
    d->m_specDirty = true;
}

void QtMaterialCheckbox::invalidateResolvedSpec()
{
    d->m_specDirty = true;
}

void QtMaterialCheckbox::resolveSpecIfNeeded() const
{
    if (!d->m_specDirty) {
        return;
    }

    SelectionSpecResolver factory;
    d->m_spec = factory.checkboxSpec(theme(), density());
    const_cast<QtMaterialCheckbox*>(this)->setSpacing(d->m_spec.spacing);

    if (d->m_transition && theme().motion().contains(d->m_spec.motionToken)) {
        const MotionStyle motion = theme().motion().style(d->m_spec.motionToken);
        if (motion.durationMs > 0) {
            d->m_transition->setDuration(motion.durationMs);
        }
        d->m_transition->setEasingCurve(motion.easing);
    }

    d->m_specDirty = false;
}

void QtMaterialCheckbox::mousePressEvent(QMouseEvent* event)
{
    if (d->m_ripple) {
        d->m_ripple->addRipple(QtMaterial::mousePosition(event));
    }
    QtMaterialSelectionControl::mousePressEvent(event);
}

void QtMaterialCheckbox::nextCheckState()
{
    if (!isEnabled()) {
        return;
    }

    if (d->m_tristate) {
        switch (d->m_checkState) {
        case Qt::Unchecked:
            setCheckState(Qt::PartiallyChecked);
            break;
        case Qt::PartiallyChecked:
            setCheckState(Qt::Checked);
            break;
        case Qt::Checked:
        default:
            setCheckState(Qt::Unchecked);
            break;
        }
        return;
    }

    setCheckState(d->m_checkState == Qt::Checked ? Qt::Unchecked : Qt::Checked);
}

void QtMaterialCheckbox::paintEvent(QPaintEvent*)
{
    resolveSpecIfNeeded();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRect box = indicatorRect();
    const QRectF stateLayerRect =
        SelectionRenderHelper::centeredStateLayerRect(box, d->m_spec.stateLayerSize);
    const qreal stateOpacity =
        SelectionRenderHelper::stateLayerOpacity(theme(), interactionState());
    const qreal progress =
        d->m_transition ? d->m_transition->progress() : targetTransitionProgress();

    SelectionRenderHelper::paintCircularStateLayer(
        &painter, stateLayerRect, d->m_spec.stateLayerColor, stateOpacity);

    QPainterPath rippleClip;
    rippleClip.addEllipse(stateLayerRect);
    if (d->m_ripple) {
        d->m_ripple->setClipPath(rippleClip);
        d->m_ripple->paint(&painter, d->m_spec.stateLayerColor);
    }

    const bool enabled = isEnabled();
    const QColor selectedContainer =
        enabled ? d->m_spec.selectedContainerColor : d->m_spec.disabledSelectedContainerColor;
    const QColor unselectedOutline =
        enabled ? d->m_spec.unselectedOutlineColor : d->m_spec.disabledUnselectedOutlineColor;
    const QColor outlineColor =
        blendColor(unselectedOutline, selectedContainer, qMin(progress * 2.0, 1.0));

    QColor fillColor = selectedContainer;
    fillColor.setAlphaF(selectedContainer.alphaF() * qMin(progress * 2.0, 1.0));

    painter.save();
    painter.setPen(QPen(outlineColor, d->m_spec.outlineWidth));
    painter.setBrush(fillColor);
    painter.drawRoundedRect(QRectF(box), d->m_spec.cornerRadius, d->m_spec.cornerRadius);

    if (d->m_checkState == Qt::PartiallyChecked) {
        QColor iconColor = d->m_spec.selectedIconColor;
        iconColor.setAlphaF(iconColor.alphaF() * qMax(progress, 0.5));
        painter.setPen(QPen(iconColor,
                            d->m_spec.checkmarkStrokeWidth,
                            Qt::SolidLine,
                            Qt::RoundCap,
                            Qt::RoundJoin));
        const int y = box.center().y();
        painter.drawLine(QPointF(box.left() + box.width() * 0.22, y),
                         QPointF(box.right() - box.width() * 0.22, y));
    } else if (progress > 0.0) {
        QColor iconColor = d->m_spec.selectedIconColor;
        iconColor.setAlphaF(iconColor.alphaF() * progress);
        painter.setPen(QPen(iconColor,
                            d->m_spec.checkmarkStrokeWidth,
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

    const QFont labelFont =
        SelectionRenderHelper::labelFont(theme(), d->m_spec.labelTypeRole, font());
    const QColor labelColor =
        enabled ? d->m_spec.labelColor : d->m_spec.disabledLabelColor;

    SelectionRenderHelper::paintLabel(
        &painter, labelRect(), labelAlignment(), text(), labelColor, labelFont);

    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintRectFocusRing(
            &painter,
            stateLayerRect,
            d->m_spec.focusRingColor,
            stateLayerRect.height() / 2.0,
            2.0);
    }
}

} // namespace QtMaterial