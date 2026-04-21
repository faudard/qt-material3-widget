#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"

#include <QMouseEvent>

#include "qtmaterial/core/qtmaterialeventcompat.h"

#include <QPainter>

#include "private/qtmaterialbuttonrenderhelper_p.h"

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

namespace {

qreal targetStateLayerOpacity(const Theme& theme, const QtMaterialInteractionState& state)
{
    if (!state.isEnabled()) {
        return 0.0;
    }

    const auto& layer = theme.stateLayer();

    if (state.isPressed()) {
        return layer.pressOpacity;
    }
    if (state.isFocused()) {
        return layer.focusOpacity;
    }
    if (state.isHovered()) {
        return layer.hoverOpacity;
    }

    return 0.0;
}

} // namespace

QtMaterialTextButton::QtMaterialTextButton(QWidget* parent)
    : QtMaterialAbstractButton(parent)
    , m_ripple(new QtMaterialRippleController(this))
    , m_stateLayerTransition(new QtMaterialTransitionController(this))
{
    setMinimumHeight(40);

    m_stateLayerTransition->setProgress(0.0);

    QObject::connect(
        m_stateLayerTransition,
        &QtMaterialTransitionController::progressChanged,
        this,
        [this](qreal) { update(); });
}

QtMaterialTextButton::~QtMaterialTextButton() = default;

void QtMaterialTextButton::themeChangedEvent(const Theme& theme)
{
    QtMaterialAbstractButton::themeChangedEvent(theme);
    m_specDirty = true;
    syncStateLayerAnimation();
}

void QtMaterialTextButton::invalidateResolvedSpec()
{
    m_specDirty = true;
}

ButtonSpec QtMaterialTextButton::resolveButtonSpec() const
{
    SpecFactory factory;
    return factory.textButtonSpec(theme(), density());
}

void QtMaterialTextButton::ensureSpecResolved() const
{
    if (!m_specDirty) {
        return;
    }

    m_spec = resolveButtonSpec();

    if (m_stateLayerTransition && theme().motion().contains(m_spec.motionToken)) {
        const MotionStyle motion = theme().motion().style(m_spec.motionToken);
        if (motion.durationMs > 0) {
            m_stateLayerTransition->setDuration(motion.durationMs);
        }
        m_stateLayerTransition->setEasingCurve(motion.easing);
    }

    m_specDirty = false;
}

QSize QtMaterialTextButton::sizeHint() const
{
    ensureSpecResolved();

    QFont resolvedFont = font();
    if (theme().typography().contains(m_spec.labelTypeRole)) {
        resolvedFont = theme().typography().style(m_spec.labelTypeRole).font;
    }

    const auto layout = ButtonRenderHelper::layoutContent(
        this,
        m_spec,
        QRect(0, 0, 4000, m_spec.touchTarget.height()),
        resolvedFont,
        text());

    const int contentRight = layout.hasIcon ? layout.textRect.right() : layout.textRect.right();
    const int contentLeft = layout.hasIcon ? qMin(layout.iconRect.left(), layout.textRect.left())
                                           : layout.textRect.left();
    const int contentWidth = qMax(0, contentRight - contentLeft + 1);
    const int width = qMax(88, contentWidth + 2 * m_spec.horizontalPadding);

    return QSize(width, m_spec.touchTarget.height());
}

QSize QtMaterialTextButton::minimumSizeHint() const
{
    ensureSpecResolved();
    return QSize(64, m_spec.touchTarget.height());
}

void QtMaterialTextButton::mousePressEvent(QMouseEvent* event)
{
    if (m_ripple) {
        m_ripple->addRipple(QtMaterial::mousePosition(event));
    }

    QtMaterialAbstractButton::mousePressEvent(event);
}

void QtMaterialTextButton::stateChangedEvent()
{
    QtMaterialAbstractButton::stateChangedEvent();
    syncStateLayerAnimation();
}

void QtMaterialTextButton::syncStateLayerAnimation()
{
    ensureSpecResolved();

    const qreal target = targetStateLayerOpacity(theme(), interactionState());

    if (!m_stateLayerTransition) {
        update();
        return;
    }

    m_stateLayerTransition->startTo(target);
}

qreal QtMaterialTextButton::animatedStateLayerOpacity() const noexcept
{
    if (!m_stateLayerTransition) {
        return targetStateLayerOpacity(theme(), interactionState());
    }

    return m_stateLayerTransition->progress();
}

void QtMaterialTextButton::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF visualRect = ButtonRenderHelper::containerRect(rect(), m_spec).adjusted(1, 1, -1, -1);
    const qreal radius = ButtonRenderHelper::cornerRadius(theme(), m_spec, visualRect);
    const QPainterPath path = ButtonRenderHelper::containerPath(theme(), m_spec, visualRect);

    const qreal layerOpacity = animatedStateLayerOpacity();
    if (layerOpacity > 0.0) {
        QtMaterialStateLayerPainter::paintPath(&painter, path, m_spec.stateLayerColor, layerOpacity);
    }

    if (m_ripple) {
        m_ripple->setClipPath(path);
        m_ripple->paint(&painter, m_spec.stateLayerColor);
    }

    QFont resolvedFont = font();
    if (theme().typography().contains(m_spec.labelTypeRole)) {
        resolvedFont = theme().typography().style(m_spec.labelTypeRole).font;
    }

    const QColor contentColor = isEnabled() ? m_spec.labelColor : m_spec.disabledLabelColor;

    ButtonRenderHelper::paintContent(
        &painter,
        this,
        m_spec,
        visualRect.toAlignedRect(),
        contentColor,
        contentColor,
        resolvedFont,
        text());

    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintPathFocusRing(&painter, path, m_spec.focusRingColor, 2.0);
    }

    Q_UNUSED(radius)
}

} // namespace QtMaterial