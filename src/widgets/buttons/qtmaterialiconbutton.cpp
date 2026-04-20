#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"

// #include <QChangeEvent>
#include <QEnterEvent>
#include <QEvent>
#include <QFontMetrics>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QStyleOptionButton>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"

namespace QtMaterial {
namespace {

static QColor effectiveContainerColor(const IconButtonSpec& spec, bool enabled, bool selected)
{
    // if (!enabled) {
    //     return selected ? spec.disabledSelectedContainerColor : spec.disabledContainerColor;
    // }
    // return selected ? spec.selectedContainerColor : spec.containerColor;
}

static QColor effectiveIconColor(const IconButtonSpec& spec, bool enabled, bool selected)
{
    if (!enabled) {
        return spec.disabledIconColor;
    }
    return selected ? spec.selectedIconColor : spec.iconColor;
}

static qreal radiusForSpec(const IconButtonSpec& spec)
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

static qreal stateLayerOpacity(const QtMaterialIconButton* button, const Theme& theme)
{
    // const auto& state = button->interactionState();
    const auto& layer = theme.stateLayer();

    if (!button->isEnabled()) {
        return 0.0;
    }
    // if (state.isPressed()) {
    //     return layer.pressOpacity;
    // }
    // if (state.isFocused()) {
    //     return layer.focusOpacity;
    // }
    // if (state.isHovered()) {
    //     return layer.hoverOpacity;
    // }
    return 0.0;
}

} // namespace

QtMaterialIconButton::QtMaterialIconButton(QWidget* parent)
    : QtMaterialAbstractButton(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setCheckable(false);
    // setAutoDefault(false);
    // setDefault(false);
    invalidateResolvedSpec();
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
    update();
}

void QtMaterialIconButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    ensureSpecResolved();
    ensureLayoutResolved();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QColor containerColor = effectiveContainerColor(m_spec, isEnabled(), m_selected);
    const QColor iconColor = effectiveIconColor(m_spec, isEnabled(), m_selected);

    painter.fillPath(m_cachedContainerPath, containerColor);

    const qreal overlayOpacity = stateLayerOpacity(this, theme());
    if (overlayOpacity > 0.0) {
        QtMaterialStateLayerPainter::paintPath(
            &painter,
            m_cachedContainerPath,
            m_spec.stateLayerColor,
            overlayOpacity
        );
    }

    // if (rippleController()) {
    //     rippleController()->setClipPath(m_cachedContainerPath);
    //     rippleController()->paint(&painter, m_spec.stateLayerColor);
    // }

    if (hasFocus()) {
        QtMaterialFocusIndicator::paintPathFocusRing(
            &painter,
            m_cachedContainerPath,
            // m_spec.focusRingColor,
            // m_spec.focusRingWidth
        );
    }

    if (!icon().isNull()) {
        const QPixmap pixmap = icon().pixmap(QSize(m_spec.iconSize, m_spec.iconSize), isEnabled() ? QIcon::Normal : QIcon::Disabled);
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
    m_specDirty = false;
    m_layoutDirty = true;
}

void QtMaterialIconButton::ensureLayoutResolved() const
{
    ensureSpecResolved();
    if (!m_layoutDirty) {
        return;
    }

    const QSize widgetSize = size();
    const int targetSide = std::max(std::min(widgetSize.width(), widgetSize.height()), m_spec.containerSize);
    const QRect outer = rect();
    const QSize visualSize(m_spec.containerSize, m_spec.containerSize);

    m_cachedVisualRect = QRect(
        outer.center().x() - visualSize.width() / 2,
        outer.center().y() - visualSize.height() / 2,
        visualSize.width(),
        visualSize.height()
    );

    m_cachedCornerRadius = radiusForSpec(m_spec);

    QPainterPath path;
    path.addRoundedRect(QRectF(m_cachedVisualRect), m_cachedCornerRadius, m_cachedCornerRadius);
    m_cachedContainerPath = path;

    m_cachedIconRect = QRect(
        m_cachedVisualRect.center().x() - m_spec.iconSize / 2,
        m_cachedVisualRect.center().y() - m_spec.iconSize / 2,
        m_spec.iconSize,
        m_spec.iconSize
    );

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
