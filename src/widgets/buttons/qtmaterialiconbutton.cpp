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
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "private/qtmaterialbuttonmotionhelper_p.h"

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
    constexpr const char* autoAccessibleNameProperty = "_qtm3_auto_accessible_name";

    const QString currentName = accessibleName().trimmed();
    const QString previousAutoName = property(autoAccessibleNameProperty).toString().trimmed();
    if (!currentName.isEmpty() && currentName != previousAutoName) {
        return currentName;
    }

    const QString textName = QAbstractButton::text().trimmed();
    if (!textName.isEmpty()) {
        return textName;
    }

    const QString tooltipName = toolTip().trimmed();
    if (!tooltipName.isEmpty()) {
        return tooltipName;
    }

    if (!requiresAccessibleName()) {
        return QStringLiteral("Icon button");
    }

    return QString();
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
    constexpr const char* autoAccessibleNameProperty = "_qtm3_auto_accessible_name";

    const QString name = effectiveAccessibleName();
    const QString currentName = accessibleName().trimmed();
    const QString previousAutoName = property(autoAccessibleNameProperty).toString().trimmed();
    const bool currentNameIsAuto = currentName.isEmpty()
        || (!previousAutoName.isEmpty() && currentName == previousAutoName);

    if (!name.isEmpty()) {
        if (currentNameIsAuto) {
            QWidget::setAccessibleName(name);
            setProperty(autoAccessibleNameProperty, name);
        }
        setAccessibleDescription(QStringLiteral("Icon button"));
        return;
    }

    if (currentNameIsAuto && !previousAutoName.isEmpty()) {
        QWidget::setAccessibleName(QString());
        setProperty(autoAccessibleNameProperty, QString());
    }

    setAccessibleDescription(QStringLiteral(
        "Icon button requires an accessible name, tooltip, or fallback label for assistive technologies"));
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
        ButtonMotionHelper::targetStateLayerOpacity(theme(), interactionState());

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
            d->m_spec.stateLayerColor,
            2.0);
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

IconButtonSpec QtMaterialIconButton::resolveIconButtonSpec() const
{
    SpecFactory factory;
    return factory.iconButtonSpec(theme(), density());
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

    ButtonMotionHelper::configureMotion(
        theme(),
        d->m_spec,
        d->m_transition,
        d->m_ripple);

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