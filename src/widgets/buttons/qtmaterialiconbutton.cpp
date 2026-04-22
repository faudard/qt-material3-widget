#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>

#include "qtmaterial/core/qtmaterialeventcompat.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "private/qtmaterialbuttonmotionhelper_p.h"

namespace QtMaterial {

namespace {

QColor blendColor(const QColor& a, const QColor& b, qreal t)
{
    QColor out;
    out.setRedF(a.redF() + (b.redF() - a.redF()) * t);
    out.setGreenF(a.greenF() + (b.greenF() - a.greenF()) * t);
    out.setBlueF(a.blueF() + (b.blueF() - a.blueF()) * t);
    out.setAlphaF(a.alphaF() + (b.alphaF() - a.alphaF()) * t);
    return out;
}

qreal radiusForSpec(const IconButtonSpec& spec)
{
    switch (spec.shapeRole) {
    case ShapeRole::Full:
        return spec.containerSize / 2.0;
    case ShapeRole::ExtraLarge:
        return 28.0;
    case ShapeRole::Large:
        return 20.0;
    case ShapeRole::Medium:
        return 16.0;
    case ShapeRole::Small:
        return 12.0;
    case ShapeRole::ExtraSmall:
        return 8.0;
    case ShapeRole::None:
    default:
        return 0.0;
    }
}

qreal stateLayerOpacityFor(const QtMaterialInteractionState& state, const Theme& theme, bool enabled)
{
    if (!enabled) {
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

QtMaterialIconButton::QtMaterialIconButton(QWidget* parent)
    : QtMaterialAbstractButton(parent)
    , m_ripple(new QtMaterialRippleController(this))
    , m_transition(new QtMaterialTransitionController(this))
{
    setFocusPolicy(Qt::StrongFocus);
    setCheckable(false);

    m_transition->setProgress(m_selected ? 1.0 : 0.0);

    QObject::connect(
        m_transition,
        &QtMaterialTransitionController::progressChanged,
        this,
        [this](qreal) { update(); });

    QObject::connect(this, &QAbstractButton::toggled, this, [this](bool checked) {
        setSelected(checked);
    });

    m_specDirty = true;
    m_layoutDirty = true;
}

QtMaterialIconButton::QtMaterialIconButton(const QIcon& icon, QWidget* parent)
    : QtMaterialIconButton(parent)
{
    setIcon(icon);
}

QtMaterialIconButton::~QtMaterialIconButton() = default;

QSize QtMaterialIconButton::sizeHint() const
{
    ensureSpecResolved();
    const int side = std::max(m_spec.touchTarget.width(), m_spec.touchTarget.height());
    return QSize(side, side);
}

QSize QtMaterialIconButton::minimumSizeHint() const
{
    return sizeHint();
}

bool QtMaterialIconButton::isSelected() const noexcept
{
    return m_selected;
}

void QtMaterialIconButton::setSelected(bool selected)
{
    if (m_selected == selected) {
        return;
    }

    m_selected = selected;

    if (m_transition) {
        selected ? m_transition->startForward() : m_transition->startBackward();
    } else {
        update();
    }
}

void QtMaterialIconButton::mousePressEvent(QMouseEvent* event)
{
    if (m_ripple) {
        m_ripple->addRipple(QtMaterial::mousePosition(event));
    }

    QtMaterialAbstractButton::mousePressEvent(event);
}

void QtMaterialIconButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    ensureSpecResolved();
    ensureLayoutResolved();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const qreal progress = m_transition ? m_transition->progress() : (m_selected ? 1.0 : 0.0);

    const QColor containerColor = isEnabled()
                                      ? ButtonMotionHelper::blendColor(m_spec.containerColor, m_spec.selectedContainerColor, progress)
                                      : m_spec.containerColor;

    const QColor iconColor = isEnabled()
                                 ? ButtonMotionHelper::blendColor(m_spec.iconColor, m_spec.selectedIconColor, progress)
                                 : m_spec.disabledIconColor;

    const qreal overlayOpacity =
        ButtonMotionHelper::targetStateLayerOpacity(theme(), interactionState());

    painter.fillPath(m_cachedContainerPath, containerColor);

    if (overlayOpacity > 0.0) {
        QtMaterialStateLayerPainter::paintPath(
            &painter,
            m_cachedContainerPath,
            m_spec.stateLayerColor,
            overlayOpacity);
    }

    if (m_ripple) {
        m_ripple->setClipPath(m_cachedContainerPath);
        m_ripple->paint(&painter, m_spec.stateLayerColor);
    }

    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintPathFocusRing(
            &painter,
            m_cachedContainerPath,
            m_spec.stateLayerColor,
            2.0);
    }

    if (!icon().isNull()) {
        const QPixmap pixmap = icon().pixmap(
            QSize(m_spec.iconSize, m_spec.iconSize),
            isEnabled() ? QIcon::Normal : QIcon::Disabled);

        if (!pixmap.isNull()) {
            QImage image = pixmap.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);

            QPainter ip(&image);
            ip.setCompositionMode(QPainter::CompositionMode_SourceIn);
            ip.fillRect(image.rect(), iconColor);
            ip.end();

            painter.drawImage(m_cachedIconRect, image);
        }
    }
}

void QtMaterialIconButton::resizeEvent(QResizeEvent* event)
{
    invalidateLayoutCache();
    QtMaterialAbstractButton::resizeEvent(event);
}

void QtMaterialIconButton::changeEvent(QEvent* event)
{
    switch (event->type()) {
    case QEvent::EnabledChange:
    case QEvent::FontChange:
    case QEvent::StyleChange:
        invalidateLayoutCache();
        invalidateResolvedSpec();
        break;
    default:
        break;
    }

    QtMaterialAbstractButton::changeEvent(event);
}

void QtMaterialIconButton::themeChangedEvent(const Theme& theme)
{
    Q_UNUSED(theme)
    invalidateResolvedSpec();
}

void QtMaterialIconButton::stateChangedEvent()
{
    update();
}

void QtMaterialIconButton::contentChangedEvent()
{
    invalidateLayoutCache();
}

IconButtonSpec QtMaterialIconButton::resolveIconButtonSpec() const
{
    SpecFactory factory;
    return factory.iconButtonSpec(theme(), density());
}

void QtMaterialIconButton::invalidateLayoutCache()
{
    m_layoutDirty = true;
    updateGeometry();
    update();
}

void QtMaterialIconButton::invalidateResolvedSpec()
{
    m_specDirty = true;
    m_layoutDirty = true;
    updateGeometry();
    update();
}

void QtMaterialIconButton::ensureSpecResolved() const
{
    if (!m_specDirty) {
        return;
    }

    m_spec = resolveIconButtonSpec();

    ButtonMotionHelper::configureMotion(
        theme(),
        m_spec,
        m_transition,
        m_ripple);

    m_specDirty = false;
    m_layoutDirty = true;
}

void QtMaterialIconButton::ensureLayoutResolved() const
{
    ensureSpecResolved();

    if (!m_layoutDirty) {
        return;
    }

    const QRect outer = rect();
    const QSize visualSize(m_spec.containerSize, m_spec.containerSize);

    m_cachedVisualRect = QRect(
        outer.center().x() - visualSize.width() / 2,
        outer.center().y() - visualSize.height() / 2,
        visualSize.width(),
        visualSize.height());

    m_cachedCornerRadius = radiusForSpec(m_spec);

    QPainterPath path;
    path.addRoundedRect(QRectF(m_cachedVisualRect), m_cachedCornerRadius, m_cachedCornerRadius);
    m_cachedContainerPath = path;

    m_cachedIconRect = QRect(
        m_cachedVisualRect.center().x() - m_spec.iconSize / 2,
        m_cachedVisualRect.center().y() - m_spec.iconSize / 2,
        m_spec.iconSize,
        m_spec.iconSize);

    m_layoutDirty = false;
}

QRect QtMaterialIconButton::visualContainerRect() const
{
    ensureLayoutResolved();
    return m_cachedVisualRect;
}

QPainterPath QtMaterialIconButton::containerPath() const
{
    ensureLayoutResolved();
    return m_cachedContainerPath;
}

QRect QtMaterialIconButton::iconRect() const
{
    ensureLayoutResolved();
    return m_cachedIconRect;
}

} // namespace QtMaterial