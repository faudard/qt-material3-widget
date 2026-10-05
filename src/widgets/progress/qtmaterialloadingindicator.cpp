#include "qtmaterial/widgets/progress/qtmaterialloadingindicator.h"

#include <QAbstractAnimation>
#include <QHideEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QShowEvent>
#include <QVariantAnimation>

#include <cmath>

#include "../resolution/qtmaterialmissingmaterial3specresolution_p.h"

namespace QtMaterial {

class QtMaterialLoadingIndicatorPrivate
{
public:
    bool active = true;
    int indicatorSize = 48;
    qreal phase = 0.0;
    QVariantAnimation* animation = nullptr;
};

QtMaterialLoadingIndicator::QtMaterialLoadingIndicator(QWidget* parent)
    : QtMaterialWidget(parent)
    , d(std::make_unique<QtMaterialLoadingIndicatorPrivate>())
{
    setMaterialComponent(QStringLiteral("LoadingIndicator"));
    setAccessibleName(tr("Loading"));
    setAccessibleDescription(tr("In progress"));
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    d->animation = new QVariantAnimation(this);
    d->animation->setStartValue(0.0);
    d->animation->setEndValue(1.0);
    d->animation->setDuration(1200);
    d->animation->setLoopCount(-1);
    d->animation->setEasingCurve(QEasingCurve::InOutSine);

    connect(d->animation, &QVariantAnimation::valueChanged,
            this, [this](const QVariant& value) {
        d->phase = value.toReal();
        update();
    });

    updateAnimationState();
}

QtMaterialLoadingIndicator::~QtMaterialLoadingIndicator() = default;

bool QtMaterialLoadingIndicator::isActive() const noexcept
{
    return d->active;
}

void QtMaterialLoadingIndicator::setActive(bool active)
{
    if (d->active == active) {
        return;
    }

    d->active = active;
    setAccessibleDescription(active ? tr("In progress") : tr("Idle"));
    updateAnimationState();
    update();
    Q_EMIT activeChanged(active);
}

int QtMaterialLoadingIndicator::indicatorSize() const noexcept
{
    return d->indicatorSize;
}

void QtMaterialLoadingIndicator::setIndicatorSize(int size)
{
    size = qMax(24, size);
    if (d->indicatorSize == size) {
        return;
    }

    d->indicatorSize = size;
    updateGeometry();
    update();
    Q_EMIT indicatorSizeChanged(size);
}

QSize QtMaterialLoadingIndicator::sizeHint() const
{
    return QSize(d->indicatorSize, d->indicatorSize);
}

QSize QtMaterialLoadingIndicator::minimumSizeHint() const
{
    return QSize(24, 24);
}

void QtMaterialLoadingIndicator::paintEvent(QPaintEvent*)
{
    if (!d->active) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF bounds = QRectF(rect()).adjusted(4.0, 4.0, -4.0, -4.0);
    if (bounds.isEmpty()) {
        return;
    }

    const QPointF center = bounds.center();
    const qreal baseRadius = qMin(bounds.width(), bounds.height()) / 2.0;
    constexpr int points = 8;
    constexpr qreal pi = 3.14159265358979323846;

    QPainterPath path;
    for (int index = 0; index < points; ++index) {
        const qreal angle =
            (2.0 * pi * index / points)
            - (pi / 2.0)
            + d->phase * 2.0 * pi;
        const qreal wave =
            0.5 + 0.5 * std::sin(
                d->phase * 2.0 * pi
                + index * pi / 2.0);
        const qreal spoke =
            index % 2 == 0
                ? 0.78 + 0.18 * wave
                : 0.58 + 0.16 * (1.0 - wave);
        const QPointF point(
            center.x() + std::cos(angle) * baseRadius * spoke,
            center.y() + std::sin(angle) * baseRadius * spoke);

        if (index == 0) {
            path.moveTo(point);
        } else {
            path.lineTo(point);
        }
    }
    path.closeSubpath();

    const auto resolved =
        MissingMaterial3SpecResolution::loadingIndicatorSpec(
            theme(),
            palette());

    painter.setPen(Qt::NoPen);
    painter.setBrush(resolved.indicatorColor);
    painter.drawPath(path);
}

void QtMaterialLoadingIndicator::showEvent(QShowEvent* event)
{
    QtMaterialWidget::showEvent(event);
    updateAnimationState();
}

void QtMaterialLoadingIndicator::hideEvent(QHideEvent* event)
{
    if (d->animation) {
        d->animation->stop();
    }
    QtMaterialWidget::hideEvent(event);
}

void QtMaterialLoadingIndicator::themeChangedEvent(const Theme& theme)
{
    QtMaterialWidget::themeChangedEvent(theme);
    updateAnimationState();
    update();
}

void QtMaterialLoadingIndicator::updateAnimationState()
{
    if (!d->animation) {
        return;
    }

    const auto resolved =
        MissingMaterial3SpecResolution::loadingIndicatorSpec(
            theme(),
            palette());
    const bool reducedMotion = resolved.reducedMotion;

    if (!d->active || !isVisible() || reducedMotion) {
        d->animation->stop();
        if (reducedMotion) {
            d->phase = 0.5;
        }
        return;
    }

    if (d->animation->state() != QAbstractAnimation::Running) {
        d->animation->start();
    }
}

} // namespace QtMaterial
