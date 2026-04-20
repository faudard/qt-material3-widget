#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"

#include <QMouseEvent>
#include <QPainter>

#include "private/qtmaterialbuttonrenderhelper_p.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

QtMaterialTextButton::QtMaterialTextButton(QWidget* parent)
    : QtMaterialAbstractButton(parent)
    , m_ripple(new QtMaterialRippleController(this))
{
    setMinimumHeight(40);
}

QtMaterialTextButton::~QtMaterialTextButton() = default;

void QtMaterialTextButton::themeChangedEvent(const Theme& theme)
{
    QtMaterialAbstractButton::themeChangedEvent(theme);
    m_specDirty = true;
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
    m_specDirty = false;
}

QSize QtMaterialTextButton::sizeHint() const
{
    ensureSpecResolved();
    QFont resolvedFont = font();
    if (theme().typography().contains(m_spec.labelTypeRole)) {
        resolvedFont = theme().typography().style(m_spec.labelTypeRole).font;
    }
    const auto layout = ButtonRenderHelper::layoutContent(this, m_spec, QRect(0, 0, 4000, m_spec.touchTarget.height()), resolvedFont, text());
    const int contentRight = layout.hasIcon ? layout.textRect.right() : layout.textRect.right();
    const int contentLeft = layout.hasIcon ? qMin(layout.iconRect.left(), layout.textRect.left()) : layout.textRect.left();
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
        m_ripple->addRipple(event->position());
    }
    QtMaterialAbstractButton::mousePressEvent(event);
}

void QtMaterialTextButton::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF visualRect = ButtonRenderHelper::containerRect(rect(), m_spec).adjusted(1, 1, -1, -1);
    const qreal radius = ButtonRenderHelper::cornerRadius(theme(), m_spec, visualRect);
    const QPainterPath path = ButtonRenderHelper::containerPath(theme(), m_spec, visualRect);

    const qreal layerOpacity = ButtonRenderHelper::stateLayerOpacity(theme(), interactionState());
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
    ButtonRenderHelper::paintContent(&painter, this, m_spec, visualRect.toAlignedRect(), isEnabled() ? m_spec.labelColor : m_spec.disabledLabelColor, resolvedFont, text());

    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintPathFocusRing(&painter, path, m_spec.stateLayerColor, 2.0);
    }

    Q_UNUSED(radius)
}

} // namespace QtMaterial
