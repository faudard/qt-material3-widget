#include "qtmaterial/widgets/progress/qtmateriallinearprogressindicator.h"

#include <QEvent>
#include <QPainter>
#include <QPalette>
#include <QtGlobal>
#include <cmath>
#include <QVariantAnimation>

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

QtMaterialLinearProgressIndicator::QtMaterialLinearProgressIndicator(QWidget* parent)
    : QWidget(parent)
{
    initAnimation();
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

QtMaterialLinearProgressIndicator::QtMaterialLinearProgressIndicator(const ProgressIndicatorSpec& spec, QWidget* parent)
    : QWidget(parent)
    , m_spec(spec)
{
    initAnimation();
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

QtMaterialLinearProgressIndicator::~QtMaterialLinearProgressIndicator() = default;

qreal QtMaterialLinearProgressIndicator::value() const noexcept
{
    return m_value;
}

void QtMaterialLinearProgressIndicator::setValue(qreal value)
{
    const qreal normalized = clampProgress(value);
    if (qFuzzyCompare(m_value, normalized)) {
        return;
    }
    m_value = normalized;
    emit valueChanged(m_value);
    update();
}

QtMaterialLinearProgressIndicator::Mode QtMaterialLinearProgressIndicator::mode() const noexcept
{
    return m_mode;
}

void QtMaterialLinearProgressIndicator::setMode(Mode mode)
{
    if (m_mode == mode) {
        return;
    }
    m_mode = mode;
    updateAnimationState();
    emit modeChanged(m_mode);
    update();
}

bool QtMaterialLinearProgressIndicator::invertedAppearance() const noexcept
{
    return m_invertedAppearance;
}

void QtMaterialLinearProgressIndicator::setInvertedAppearance(bool inverted)
{
    if (m_invertedAppearance == inverted) {
        return;
    }
    m_invertedAppearance = inverted;
    emit invertedAppearanceChanged(m_invertedAppearance);
    update();
}

ProgressIndicatorSpec QtMaterialLinearProgressIndicator::spec() const
{
    return m_spec;
}

void QtMaterialLinearProgressIndicator::setSpec(const ProgressIndicatorSpec& spec)
{
    m_spec = spec;
    if (m_animation) {
        m_animation->setDuration(qMax(1, m_spec.animationDurationMs));
    }
    emit specChanged();
    updateGeometry();
    update();
}

QSize QtMaterialLinearProgressIndicator::sizeHint() const
{
    return QSize(160, qMax(1, m_spec.linearHeight));
}

QSize QtMaterialLinearProgressIndicator::minimumSizeHint() const
{
    return QSize(48, qMax(1, m_spec.linearHeight));
}

void QtMaterialLinearProgressIndicator::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const int heightValue = qMax(1, m_spec.linearHeight);
    QRectF trackRect(0.0, (height() - heightValue) / 2.0, width(), heightValue);
    trackRect = trackRect.marginsRemoved(m_spec.margins);
    if (!trackRect.isValid()) {
        return;
    }

    const qreal radius = heightValue / 2.0;
    painter.setPen(Qt::NoPen);
    painter.setBrush(resolvedTrackColor());
    painter.drawRoundedRect(trackRect, radius, radius);

    QRectF activeRect = trackRect;
    if (m_mode == Mode::Determinate) {
        activeRect.setWidth(trackRect.width() * m_value);
        if (m_invertedAppearance) {
            activeRect.moveRight(trackRect.right());
        }
    } else {
        const qreal segment = qMax<qreal>(trackRect.width() * 0.28, qMin<qreal>(64.0, trackRect.width()));
        const qreal travel = trackRect.width() + segment;
        qreal left = trackRect.left() - segment + travel * m_phase;
        if (m_invertedAppearance) {
            left = trackRect.right() - travel * m_phase;
        }
        activeRect = QRectF(left, trackRect.top(), segment, trackRect.height()).intersected(trackRect);
    }

    if (activeRect.width() <= 0.0 || activeRect.height() <= 0.0) {
        return;
    }

    painter.setBrush(resolvedActiveColor());
    painter.drawRoundedRect(activeRect, radius, radius);
}

void QtMaterialLinearProgressIndicator::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    updateAnimationState();
}

void QtMaterialLinearProgressIndicator::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    updateAnimationState();
}

void QtMaterialLinearProgressIndicator::changeEvent(QEvent* event)
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

void QtMaterialLinearProgressIndicator::initAnimation()
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

void QtMaterialLinearProgressIndicator::updateAnimationState()
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

QColor QtMaterialLinearProgressIndicator::resolvedActiveColor() const
{
    return m_spec.activeColor.isValid() ? m_spec.activeColor : fallbackActive(this);
}

QColor QtMaterialLinearProgressIndicator::resolvedTrackColor() const
{
    return m_spec.trackColor.isValid() ? m_spec.trackColor : fallbackTrack(this);
}

} // namespace QtMaterial
