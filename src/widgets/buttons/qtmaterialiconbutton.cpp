#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"
#include <memory>

#include <QAbstractButton>

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRect>
#include <QResizeEvent>

#include "qtmaterial/core/qtmaterialeventcompat.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "private/qtmaterialbuttonmotionhelper_p.h"
#include "qtmaterial/specs/qtmaterialactionbuttonspecresolver.h"

namespace QtMaterial {


struct QtMaterialIconButtonPrivate
{
 bool m_specDirty = true;
 bool m_layoutDirty = true;
 IconButtonSpec m_spec;
 QRect m_cachedVisualRect;
 QRect m_cachedIconRect;
 qreal m_cachedCornerRadius = 0.0;
 QPainterPath m_cachedContainerPath;
 QtMaterialRippleController* m_ripple = nullptr;
 QtMaterialTransitionController* m_transition = nullptr;
 bool m_selected = false;
 bool m_requiresAccessibleName = true;
};

namespace {

qreal radiusForSpec(const IconButtonSpec& spec)
{
    if (spec.cornerRadius < 0.0) {
        return spec.containerSize / 2.0;
    }

    return qMin(
        spec.cornerRadius,
        spec.containerSize / 2.0);
}

qreal stateLayerOpacityFor(
    const QtMaterialInteractionState& state,
    const IconButtonSpec& spec,
    bool enabled)
{
    if (!enabled) {
        return 0.0;
    }
    if (state.isPressed()) {
        return spec.pressStateLayerOpacity;
    }
    if (state.isFocused()) {
        return spec.focusStateLayerOpacity;
    }
    if (state.isHovered()) {
        return spec.hoverStateLayerOpacity;
    }
    return 0.0;
}

} // namespace

QtMaterialIconButton::QtMaterialIconButton(QWidget* parent)
 : QtMaterialAbstractButton(parent)
 , d(std::make_unique<QtMaterialIconButtonPrivate>())
{
 d->m_ripple = new QtMaterialRippleController(this);
 d->m_transition = new QtMaterialTransitionController(this);

    setFocusPolicy(Qt::StrongFocus);
    setCheckable(false);
    setMaterialComponent(QStringLiteral("button"));
    setMaterialVariant(QStringLiteral("icon"));
    setMaterialRole(QStringLiteral("action"));
    syncAccessibilityState();

    d->m_transition->setProgress(d->m_selected ? 1.0 : 0.0);

    QObject::connect(
        d->m_transition,
        &QtMaterialTransitionController::progressChanged,
        this,
        [this](qreal) { update(); });

    QObject::connect(this, &QAbstractButton::toggled, this, [this](bool checked) {
        setSelected(checked);
    });

    d->m_specDirty = true;
    d->m_layoutDirty = true;
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
    const int side = std::max(d->m_spec.touchTarget.width(), d->m_spec.touchTarget.height());
    return QSize(side, side);
}

QSize QtMaterialIconButton::minimumSizeHint() const
{
    return sizeHint();
}

bool QtMaterialIconButton::isSelected() const noexcept
{
    return d->m_selected;
}

void QtMaterialIconButton::setSelected(bool selected)
{
    if (d->m_selected == selected) {
        return;
    }

    d->m_selected = selected;
    interactionState().setSelected(selected);
    syncAccessibilityState();

    if (d->m_transition) {
        selected ? d->m_transition->startForward() : d->m_transition->startBackward();
    } else {
        update();
    }
}



bool QtMaterialIconButton::requiresAccessibleName() const noexcept
{
    return d->m_requiresAccessibleName;
}

void QtMaterialIconButton::setRequiresAccessibleName(bool required)
{
    if (d->m_requiresAccessibleName == required) {
        return;
    }

    d->m_requiresAccessibleName = required;
    syncAccessibilityState();
}

QString QtMaterialIconButton::effectiveAccessibleName() const
{
    const QString explicitName = accessibleName().trimmed();
    const QString tooltipName = toolTip().trimmed();

    if (!explicitName.isEmpty()
        && !(explicitName == QStringLiteral("Icon button") && !tooltipName.isEmpty())) {
        return explicitName;
    }

    const QString textName = QAbstractButton::text().trimmed();
    if (!textName.isEmpty()) {
        return textName;
    }

    if (!tooltipName.isEmpty()) {
        return tooltipName;
    }

    if (requiresAccessibleName()) {
        return QString();
    }

    return QStringLiteral("Icon button");
}

bool QtMaterialIconButton::hasUsableAccessibleName() const
{
    return !effectiveAccessibleName().isEmpty();
}

QString QtMaterialIconButton::accessibilitySummary() const
{
    const QString name = effectiveAccessibleName();
    if (!name.isEmpty()) {
        return name;
    }

    return QStringLiteral("Icon button requires an explicit accessible name");
}

void QtMaterialIconButton::syncAccessibilityState()
{
    QtMaterialAbstractButton::syncAccessibilityState();
    syncIconButtonAccessibility();
}

void QtMaterialIconButton::syncIconButtonAccessibility()
{
    const QString name = effectiveAccessibleName();
    const QString currentName = accessibleName().trimmed();
    const QString tooltipName = toolTip().trimmed();

    const bool currentNameIsAutoDefault =
        currentName == QStringLiteral("Icon button");
    const bool currentNameIsAutoTooltip =
        !tooltipName.isEmpty() && currentName == tooltipName;

    if (!name.isEmpty()) {
        if (currentName.isEmpty() || currentNameIsAutoDefault || currentNameIsAutoTooltip) {
            QWidget::setAccessibleName(name);
        }

        if (accessibleDescription().trimmed().isEmpty()) {
            setAccessibleDescription(QStringLiteral("Icon button"));
        }
        return;
    }

    if (accessibleDescription().trimmed().isEmpty()) {
        setAccessibleDescription(QStringLiteral(
            "Icon button requires an explicit accessible name for assistive technologies"));
    }
}

void QtMaterialIconButton::mousePressEvent(QMouseEvent* event)
{
    if (d->m_ripple) {
        d->m_ripple->addRipple(QtMaterial::mousePosition(event));
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

    const qreal progress = d->m_transition ? d->m_transition->progress() : (d->m_selected ? 1.0 : 0.0);

    const QColor containerColor = isEnabled()
                                      ? ButtonMotionHelper::blendColor(d->m_spec.containerColor, d->m_spec.selectedContainerColor, progress)
                                      : d->m_spec.containerColor;

    const QColor iconColor = isEnabled()
                                 ? ButtonMotionHelper::blendColor(d->m_spec.iconColor, d->m_spec.selectedIconColor, progress)
                                 : d->m_spec.disabledIconColor;

    const qreal overlayOpacity =
        stateLayerOpacityFor(interactionState(), d->m_spec, isEnabled());

    painter.fillPath(d->m_cachedContainerPath, containerColor);

    if (overlayOpacity > 0.0) {
        QtMaterialStateLayerPainter::paintPath(
            &painter,
            d->m_cachedContainerPath,
            d->m_spec.stateLayerColor,
            overlayOpacity);
    }

    if (d->m_ripple) {
        d->m_ripple->setClipPath(d->m_cachedContainerPath);
        d->m_ripple->paint(&painter, d->m_spec.stateLayerColor);
    }

    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintPathFocusRing(
            &painter,
            d->m_cachedContainerPath,
            d->m_spec.focusRingColor,
            d->m_spec.focusRingWidth);
    }

    if (!icon().isNull()) {
        const QPixmap pixmap = icon().pixmap(
            QSize(d->m_spec.iconSize, d->m_spec.iconSize),
            isEnabled() ? QIcon::Normal : QIcon::Disabled);

        if (!pixmap.isNull()) {
            QImage image = pixmap.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);

            QPainter ip(&image);
            ip.setCompositionMode(QPainter::CompositionMode_SourceIn);
            ip.fillRect(image.rect(), iconColor);
            ip.end();

            painter.drawImage(d->m_cachedIconRect, image);
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
    case QEvent::ToolTipChange:
        syncAccessibilityState();
        break;
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
    syncAccessibilityState();
    invalidateLayoutCache();
}

IconButtonSpec
QtMaterialIconButton::resolveIconButtonSpec() const
{
    return ActionButtonSpecResolver().iconButtonSpec(
        theme(),
        density());
}

void QtMaterialIconButton::invalidateLayoutCache()
{
    d->m_layoutDirty = true;
    updateGeometry();
    update();
}

void QtMaterialIconButton::invalidateResolvedSpec()
{
    d->m_specDirty = true;
    d->m_layoutDirty = true;
    updateGeometry();
    update();
}

void QtMaterialIconButton::ensureSpecResolved() const
{
    if (!d->m_specDirty) {
        return;
    }

    d->m_spec = resolveIconButtonSpec();

    if (d->m_spec.hasResolvedMotionStyle) {
        d->m_transition->applyMotionStyle(
            d->m_spec.motionStyle);
        if (d->m_spec.motionStyle.durationMs > 0) {
            d->m_ripple->setDuration(
                d->m_spec.motionStyle.durationMs);
        }
    }
    d->m_ripple->setBaseOpacity(
        d->m_spec.pressStateLayerOpacity);

    d->m_specDirty = false;
    d->m_layoutDirty = true;
}

void QtMaterialIconButton::ensureLayoutResolved() const
{
    ensureSpecResolved();

    if (!d->m_layoutDirty) {
        return;
    }

    const QRect outer = rect();
    const QSize visualSize(d->m_spec.containerSize, d->m_spec.containerSize);

    d->m_cachedVisualRect = QRect(
        outer.center().x() - visualSize.width() / 2,
        outer.center().y() - visualSize.height() / 2,
        visualSize.width(),
        visualSize.height());

    d->m_cachedCornerRadius = radiusForSpec(d->m_spec);

    QPainterPath path;
    path.addRoundedRect(QRectF(d->m_cachedVisualRect), d->m_cachedCornerRadius, d->m_cachedCornerRadius);
    d->m_cachedContainerPath = path;

    d->m_cachedIconRect = QRect(
        d->m_cachedVisualRect.center().x() - d->m_spec.iconSize / 2,
        d->m_cachedVisualRect.center().y() - d->m_spec.iconSize / 2,
        d->m_spec.iconSize,
        d->m_spec.iconSize);

    d->m_layoutDirty = false;
}

} // namespace QtMaterial