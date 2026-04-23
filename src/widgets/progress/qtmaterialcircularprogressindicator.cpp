#include "qtmaterial/widgets/progress/qtmaterialcircularprogressindicator.h"

#include <QEvent>
#include <QPainter>
#include <QPalette>
#include <QVariantAnimation>
#include <QtGlobal>
#include <cmath>

namespace QtMaterial {
namespace {
qreal clampProgress(qreal value)
{
    if (std::isnan(value)) {
        return 0.0;
    }
    return qBound<qreal>(0.0, value, 1.0);
}

QColor fallbackActive(const QWidget* widget)
{
    return widget->palette().color(QPalette::Highlight);
}

QColor fallbackTrack(const QWidget* widget)
{
    QColor color = widget->palette().color(QPalette::Mid);
    color.setAlpha(qMax(32, color.alpha() / 2));
    return color;
}
} // namespace

QtMaterialCircularProgressIndicator::QtMaterialCircularProgressIndicator(QWidget* parent)
    : QWidget(parent)
{
    initAnimation();
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

QtMaterialCircularProgressIndicator::QtMaterialCircularProgressIndicator(const ProgressIndicatorSpec& spec, QWidget* parent)
    : QWidget(parent)
    , m_spec(spec)
{
    initAnimation();
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

QtMaterialCircularProgressIndicator::~QtMaterialCircularProgressIndicator() = default;

qreal QtMaterialCircularProgressIndicator::value() const noexcept
{
    return m_value;
}

void QtMaterialCircularProgressIndicator::setValue(qreal value)
{
    const qreal normalized = clampProgress(value);
    if (qFuzzyCompare(m_value, normalized)) {
        return;
    }
    m_value = normalized;
    emit valueChanged(m_value);
    update();
}

QtMaterialCircularProgressIndicator::Mode QtMaterialCircularProgressIndicator::mode() const noexcept
{
    return m_mode;
}

void QtMaterialCircularProgressIndicator::setMode(Mode mode)
{
    if (m_mode == mode) {
        return;
    }
    m_mode = mode;
    updateAnimationState();
    emit modeChanged(m_mode);
    update();
}

ProgressIndicatorSpec QtMaterialCircularProgressIndicator::spec() const
{
    return m_spec;
}

void QtMaterialCircularProgressIndicator::setSpec(const ProgressIndicatorSpec& spec)
{
    m_spec = spec;
    if (m_animation) {
        m_animation->setDuration(qMax(1, m_spec.animationDurationMs));
    }
    emit specChanged();
    updateGeometry();
    update();
}

QSize QtMaterialCircularProgressIndicator::sizeHint() const
{
    return m_spec.circularSize.expandedTo(QSize(24, 24));
}

QSize QtMaterialCircularProgressIndicator::minimumSizeHint() const
{
    return QSize(24, 24);
}

void QtMaterialCircularProgressIndicator::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const int stroke = qMax(1, m_spec.circularStrokeWidth);
    const qreal inset = stroke / 2.0 + qMax(0, m_spec.trackGap);
    QRectF rect = QRectF(0, 0, width(), height()).adjusted(inset, inset, -inset, -inset);
    if (!rect.isValid()) {
        return;
    }

    QPen trackPen(resolvedTrackColor(), stroke, Qt::SolidLine, Qt::RoundCap);
    QPen activePen(resolvedActiveColor(), stroke, Qt::SolidLine, Qt::RoundCap);

    painter.setPen(trackPen);
    painter.drawArc(rect, 0, 360 * 16);

    painter.setPen(activePen);
    int startAngle = 90 * 16;
    int spanAngle = 0;
    if (m_mode == Mode::Determinate) {
        spanAngle = static_cast<int>(-360.0 * 16.0 * m_value);
    } else {
        startAngle = static_cast<int>((90.0 - 360.0 * m_phase) * 16.0);
        spanAngle = static_cast<int>(-96.0 * 16.0);
    }

    if (spanAngle != 0) {
        painter.drawArc(rect, startAngle, spanAngle);
    }
}

void QtMaterialCircularProgressIndicator::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    updateAnimationState();
}

void QtMaterialCircularProgressIndicator::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    updateAnimationState();
}

void QtMaterialCircularProgressIndicator::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    switch (event->type()) {
    case QEvent::PaletteChange:
    case QEvent::StyleChange:
        update();
        break;
    default:
        break;
    }
}

void QtMaterialCircularProgressIndicator::initAnimation()
{
    auto* animation = new QVariantAnimation(this);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setDuration(qMax(1, m_spec.animationDurationMs));
    animation->setLoopCount(-1);
    connect(animation, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
        m_phase = value.toReal();
        update();
    });
    m_animation = animation;
    updateAnimationState();
}

void QtMaterialCircularProgressIndicator::updateAnimationState()
{
    if (!m_animation) {
        return;
    }
    if (m_mode == Mode::Indeterminate && isVisible()) {
        if (m_animation->state() != QAbstractAnimation::Running) {
            m_animation->start();
        }
    } else {
        m_animation->stop();
        m_phase = 0.0;
    }
}

QColor QtMaterialCircularProgressIndicator::resolvedActiveColor() const
{
    return m_spec.activeColor.isValid() ? m_spec.activeColor : fallbackActive(this);
}

QColor QtMaterialCircularProgressIndicator::resolvedTrackColor() const
{
    return m_spec.trackColor.isValid() ? m_spec.trackColor : fallbackTrack(this);
}

} // namespace QtMaterial
