#include "qtmaterial/widgets/selection/qtmaterialswitch.h"

#include <QMouseEvent>
#include "qtmaterial/core/qtmaterialeventcompat.h"
#include <QPainter>

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
    QObject::connect(this, &QAbstractButton::toggled, this, [this](bool checked) {
        if (m_transition) {
            checked ? m_transition->startForward() : m_transition->startBackward();
        }
        update();
    });
    QObject::connect(m_transition, &QtMaterialTransitionController::progressChanged, this, [this](qreal) {
        update();
    });
}
QtMaterialSwitch::~QtMaterialSwitch() = default;
void QtMaterialSwitch::themeChangedEvent(const Theme& theme) { QtMaterialSelectionControl::themeChangedEvent(theme); m_specDirty = true; }
void QtMaterialSwitch::invalidateResolvedSpec() { m_specDirty = true; }
void QtMaterialSwitch::resolveSpecIfNeeded() const
{
    if (!m_specDirty) return;
    SpecFactory factory;
    m_spec = factory.switchSpec(theme(), density());
    const_cast<QtMaterialSwitch*>(this)->setSpacing(m_spec.spacing);
    m_specDirty = false;
}
QSize QtMaterialSwitch::sizeHint() const
{
    const_cast<QtMaterialSwitch*>(this)->resolveSpecIfNeeded();
    const QFontMetrics fm(font());
    const int textWidth = text().isEmpty() ? 0 : (m_spec.spacing + fm.horizontalAdvance(text()));
    return QSize(m_spec.touchTarget.width() + textWidth, qMax(m_spec.touchTarget.height(), fm.height() + 16));
}
void QtMaterialSwitch::mousePressEvent(QMouseEvent* event)
{
    if (m_ripple) {
        m_ripple->addRipple(QtMaterial::mousePosition(event));
    }
    QtMaterialSelectionControl::mousePressEvent(event);
}
void QtMaterialSwitch::paintEvent(QPaintEvent*)
{
    resolveSpecIfNeeded();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRect trackHost(0, 0, m_spec.touchTarget.width(), height());
    const QRectF track = SelectionRenderHelper::switchTrackRect(trackHost, m_spec);
    const qreal progress = m_transition ? m_transition->progress() : (isChecked() ? 1.0 : 0.0);

    QColor trackColor;
    QColor handleColor;
    if (isEnabled()) {
        trackColor = isChecked() ? m_spec.selectedTrackColor : m_spec.unselectedTrackColor;
        handleColor = isChecked() ? m_spec.selectedHandleColor : m_spec.unselectedHandleColor;
    } else {
        trackColor = isChecked() ? m_spec.disabledSelectedTrackColor : m_spec.disabledUnselectedTrackColor;
        handleColor = isChecked() ? m_spec.disabledSelectedHandleColor : m_spec.disabledUnselectedHandleColor;
    }

    const QRectF stateLayerRect = SelectionRenderHelper::centeredStateLayerRect(track.toAlignedRect(), m_spec.stateLayerSize);
    const qreal opacity = SelectionRenderHelper::stateLayerOpacity(theme(), interactionState());
    SelectionRenderHelper::paintCircularStateLayer(&painter, stateLayerRect, m_spec.stateLayerColor, opacity);

    if (m_ripple) {
        QPainterPath trackPath = SelectionRenderHelper::roundedTrackPath(track);
        m_ripple->setClipPath(trackPath);
        m_ripple->paint(&painter, m_spec.stateLayerColor);
    }

    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(trackColor);
    painter.drawPath(SelectionRenderHelper::roundedTrackPath(track));
    const QRectF handle = SelectionRenderHelper::switchHandleRect(track, m_spec, progress);
    painter.setBrush(handleColor);
    painter.drawEllipse(handle);
    painter.restore();

    if (!text().isEmpty()) {
        const QFont labelFont = SelectionRenderHelper::labelFont(theme(), m_spec.labelTypeRole, font());
        const QColor labelColor = isEnabled() ? m_spec.labelColor : m_spec.disabledLabelColor;
        SelectionRenderHelper::paintLabel(&painter, QRect(m_spec.touchTarget.width() + m_spec.spacing, 0, width() - m_spec.touchTarget.width() - m_spec.spacing, height()), labelAlignment(), text(), labelColor, labelFont);
    }

    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintRectFocusRing(&painter, stateLayerRect, m_spec.focusRingColor, stateLayerRect.height() / 2.0, 2.0);
    }
}
} // namespace QtMaterial
