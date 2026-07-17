#include "qtmaterial/widgets/data/qtmaterialdivider.h"

#include <QEvent>
#include <QPainter>
#include <QSizePolicy>

#include "qtmaterial/specs/qtmaterialdataspecresolver.h"

namespace QtMaterial {

class QtMaterialDividerPrivate
{
public:
    Qt::Orientation orientation =
        Qt::Horizontal;

    int leadingInset = 0;
    int trailingInset = 0;
    int thickness = 1;

    bool leadingInsetExplicit = false;
    bool trailingInsetExplicit = false;
    bool thicknessExplicit = false;

    QColor color;

    bool decorative = true;
    QString accessibilityLabel;
    QString lastAccessibilitySummary;

    mutable DividerSpec resolvedSpec;
    mutable bool specDirty = true;
};

namespace {

QString orientationText(
    Qt::Orientation orientation)
{
    return orientation == Qt::Horizontal
        ? QStringLiteral("Horizontal separator")
        : QStringLiteral("Vertical separator");
}

} // namespace

QtMaterialDivider::QtMaterialDivider(
    QWidget* parent)
    : QtMaterialDivider(
          Qt::Horizontal,
          parent)
{
}

QtMaterialDivider::QtMaterialDivider(
    Qt::Orientation orientation,
    QWidget* parent)
    : QtMaterialWidget(parent)
    , d_ptr(
          std::make_unique<
              QtMaterialDividerPrivate>())
{
    setMaterialComponent(
        QStringLiteral("Divider"));

    setAttribute(
        Qt::WA_TransparentForMouseEvents);
    setAttribute(
        Qt::WA_NoSystemBackground);
    setAttribute(
        Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
    setFocusPolicy(Qt::NoFocus);

    d_ptr->orientation = orientation;
    updateMinimumExtent();
    syncAccessibility();
}

QtMaterialDivider::~QtMaterialDivider() =
    default;

QtMaterialDivider* QtMaterialDivider::horizontal(
    QWidget* parent)
{
    return new QtMaterialDivider(
        Qt::Horizontal,
        parent);
}

QtMaterialDivider* QtMaterialDivider::vertical(
    QWidget* parent)
{
    return new QtMaterialDivider(
        Qt::Vertical,
        parent);
}

Qt::Orientation
QtMaterialDivider::orientation() const noexcept
{
    return d_ptr->orientation;
}

void QtMaterialDivider::setOrientation(
    Qt::Orientation orientation)
{
    if (d_ptr->orientation == orientation) {
        return;
    }

    d_ptr->orientation = orientation;
    invalidateResolvedSpec();
    updateMinimumExtent();

    Q_EMIT orientationChanged(orientation);

    updateGeometry();
    syncAccessibility();
    update();
}

int QtMaterialDivider::leadingInset() const noexcept
{
    return d_ptr->leadingInset;
}

void QtMaterialDivider::setLeadingInset(
    int value)
{
    const int normalized =
        qMax(0, value);

    const bool valueChanged =
        d_ptr->leadingInset
        != normalized;
    const bool explicitChanged =
        !d_ptr->leadingInsetExplicit;

    if (!valueChanged && !explicitChanged) {
        return;
    }

    d_ptr->leadingInset = normalized;
    d_ptr->leadingInsetExplicit = true;
    invalidateResolvedSpec();

    if (valueChanged) {
        Q_EMIT leadingInsetChanged(normalized);
    }

    syncAccessibility();
    update();
}

int QtMaterialDivider::trailingInset() const noexcept
{
    return d_ptr->trailingInset;
}

void QtMaterialDivider::setTrailingInset(
    int value)
{
    const int normalized =
        qMax(0, value);

    const bool valueChanged =
        d_ptr->trailingInset
        != normalized;
    const bool explicitChanged =
        !d_ptr->trailingInsetExplicit;

    if (!valueChanged && !explicitChanged) {
        return;
    }

    d_ptr->trailingInset = normalized;
    d_ptr->trailingInsetExplicit = true;
    invalidateResolvedSpec();

    if (valueChanged) {
        Q_EMIT trailingInsetChanged(normalized);
    }

    syncAccessibility();
    update();
}

int QtMaterialDivider::thickness() const noexcept
{
    return d_ptr->thickness;
}

void QtMaterialDivider::setThickness(
    int value)
{
    const int normalized =
        qMax(1, value);

    const bool valueChanged =
        d_ptr->thickness
        != normalized;
    const bool explicitChanged =
        !d_ptr->thicknessExplicit;

    if (!valueChanged && !explicitChanged) {
        return;
    }

    d_ptr->thickness = normalized;
    d_ptr->thicknessExplicit = true;
    invalidateResolvedSpec();
    updateMinimumExtent();

    if (valueChanged) {
        Q_EMIT thicknessChanged(normalized);
    }

    updateGeometry();
    syncAccessibility();
    update();
}

QColor QtMaterialDivider::color() const noexcept
{
    return d_ptr->color;
}

void QtMaterialDivider::setColor(
    const QColor& color)
{
    if (d_ptr->color == color) {
        return;
    }

    d_ptr->color = color;
    invalidateResolvedSpec();

    Q_EMIT colorChanged(color);

    update();
}

void QtMaterialDivider::resetColor()
{
    setColor(QColor());
}

bool QtMaterialDivider::isDecorative() const noexcept
{
    return d_ptr->decorative;
}

void QtMaterialDivider::setDecorative(
    bool decorative)
{
    if (d_ptr->decorative == decorative) {
        return;
    }

    d_ptr->decorative = decorative;

    Q_EMIT decorativeChanged(decorative);

    syncAccessibility();
}

QString QtMaterialDivider::accessibilityLabel() const
{
    return d_ptr->accessibilityLabel;
}

void QtMaterialDivider::setAccessibilityLabel(
    const QString& label)
{
    if (
        d_ptr->accessibilityLabel
        == label) {
        return;
    }

    d_ptr->accessibilityLabel = label;

    Q_EMIT accessibilityLabelChanged(label);

    syncAccessibility();
}

QString
QtMaterialDivider::accessibilitySummary() const
{
    if (d_ptr->decorative) {
        return QString();
    }

    const QString explicitLabel =
        d_ptr->accessibilityLabel.trimmed();

    if (!explicitLabel.isEmpty()) {
        return explicitLabel;
    }

    ensureSpecResolved();
    const DividerSpec& spec =
        d_ptr->resolvedSpec;

    QString summary =
        orientationText(
            spec.orientation);

    if (
        spec.leadingInset > 0
        || spec.trailingInset > 0) {
        summary += QStringLiteral(", inset");
    }

    if (spec.thickness > 1) {
        summary +=
            QStringLiteral(", %1 px thick")
                .arg(spec.thickness);
    }

    return summary;
}

QRect QtMaterialDivider::lineRect() const
{
    ensureSpecResolved();

    const DividerSpec& spec =
        d_ptr->resolvedSpec;

    const int lineThickness =
        qMax(1, spec.thickness);

    if (
        spec.orientation
        == Qt::Horizontal) {
        const int leading =
            qMax(0, spec.leadingInset);
        const int trailing =
            qMax(0, spec.trailingInset);

        const int x =
            layoutDirection()
                == Qt::RightToLeft
            ? trailing
            : leading;

        const int widthValue =
            qMax(
                0,
                width()
                    - leading
                    - trailing);

        const int y =
            qMax(
                0,
                (height() - lineThickness) / 2);

        return QRect(
            x,
            y,
            widthValue,
            lineThickness);
    }

    const int top =
        qMax(0, spec.leadingInset);
    const int bottom =
        qMax(0, spec.trailingInset);
    const int heightValue =
        qMax(
            0,
            height()
                - top
                - bottom);
    const int x =
        qMax(
            0,
            (width() - lineThickness) / 2);

    return QRect(
        x,
        top,
        lineThickness,
        heightValue);
}

const DividerSpec&
QtMaterialDivider::resolvedSpec() const
{
    ensureSpecResolved();
    return d_ptr->resolvedSpec;
}

QSize QtMaterialDivider::sizeHint() const
{
    ensureSpecResolved();

    const int thicknessValue =
        d_ptr->resolvedSpec.thickness;

    return (
        d_ptr->resolvedSpec.orientation
        == Qt::Horizontal)
        ? QSize(64, thicknessValue)
        : QSize(thicknessValue, 64);
}

QSize QtMaterialDivider::minimumSizeHint() const
{
    ensureSpecResolved();

    const int thicknessValue =
        d_ptr->resolvedSpec.thickness;

    return (
        d_ptr->resolvedSpec.orientation
        == Qt::Horizontal)
        ? QSize(0, thicknessValue)
        : QSize(thicknessValue, 0);
}

void QtMaterialDivider::paintEvent(
    QPaintEvent*)
{
    ensureSpecResolved();

    const DividerSpec& spec =
        d_ptr->resolvedSpec;
    const QRect geometry =
        lineRect();

    if (
        geometry.isEmpty()
        || !spec.color.isValid()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(
        QPainter::Antialiasing,
        spec.cornerRadius > 0.0);
    painter.setPen(Qt::NoPen);
    painter.setBrush(
        (
            spec.leadingInset > 0
            || spec.trailingInset > 0)
            && spec.insetColor.isValid()
        ? spec.insetColor
        : spec.color);

    if (spec.cornerRadius > 0.0) {
        painter.drawRoundedRect(
            QRectF(geometry),
            spec.cornerRadius,
            spec.cornerRadius);
    } else {
        painter.drawRect(geometry);
    }
}

void QtMaterialDivider::changeEvent(
    QEvent* event)
{
    QtMaterialWidget::changeEvent(event);

    if (
        event
        && event->type()
            == QEvent::LayoutDirectionChange) {
        update();
    }
}

void QtMaterialDivider::themeChangedEvent(
    const QtMaterial::Theme& theme)
{
    QtMaterialWidget::themeChangedEvent(theme);
    invalidateResolvedSpec();
    updateMinimumExtent();
    updateGeometry();
    update();
}

void QtMaterialDivider::ensureSpecResolved() const
{
    if (!d_ptr->specDirty) {
        return;
    }

    const DataSpecResolver resolver;
    DividerSpec spec =
        resolver.dividerSpec(theme());

    spec.orientation =
        d_ptr->orientation;

    if (d_ptr->leadingInsetExplicit) {
        spec.leadingInset =
            d_ptr->leadingInset;
    }
    if (d_ptr->trailingInsetExplicit) {
        spec.trailingInset =
            d_ptr->trailingInset;
    }
    if (d_ptr->thicknessExplicit) {
        spec.thickness =
            d_ptr->thickness;
    }

    if (d_ptr->color.isValid()) {
        spec.color =
            d_ptr->color;
        spec.insetColor =
            d_ptr->color;
    }

    d_ptr->resolvedSpec = spec;
    d_ptr->specDirty = false;
}

void QtMaterialDivider::invalidateResolvedSpec()
{
    d_ptr->specDirty = true;
}

void QtMaterialDivider::updateMinimumExtent()
{
    ensureSpecResolved();

    const int thicknessValue =
        qMax(
            1,
            d_ptr->resolvedSpec.thickness);

    if (
        d_ptr->resolvedSpec.orientation
        == Qt::Horizontal) {
        setSizePolicy(
            QSizePolicy::Expanding,
            QSizePolicy::Fixed);
        setMinimumSize(
            0,
            thicknessValue);
    } else {
        setSizePolicy(
            QSizePolicy::Fixed,
            QSizePolicy::Expanding);
        setMinimumSize(
            thicknessValue,
            0);
    }
}

void QtMaterialDivider::syncAccessibility()
{
    const QString summary =
        accessibilitySummary();

    if (d_ptr->decorative) {
        setAccessibleName(QString());
        setAccessibleDescription(QString());
    } else {
        const QString explicitLabel =
            d_ptr->accessibilityLabel.trimmed();

        setAccessibleName(
            explicitLabel.isEmpty()
                ? orientationText(
                      d_ptr->orientation)
                : explicitLabel);
        setAccessibleDescription(summary);
    }

    if (
        d_ptr->lastAccessibilitySummary
        == summary) {
        return;
    }

    d_ptr->lastAccessibilitySummary =
        summary;

    Q_EMIT accessibilitySummaryChanged(
        summary);
}

} // namespace QtMaterial
