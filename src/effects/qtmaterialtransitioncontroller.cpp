#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"

#include "qtmaterial/theme/qtmaterialtheme.h"

#include <QAbstractAnimation>
#include <QVariantAnimation>
#include <QtGlobal>

namespace QtMaterial {

namespace {
constexpr qreal kEpsilon = 0.0001;
}

QtMaterialTransitionController::QtMaterialTransitionController(QObject* parent)
    : QObject(parent)
    , m_animation(new QVariantAnimation(this))
{
    m_animation->setDuration(m_durationMs);
    m_animation->setEasingCurve(m_easing);

    connect(m_animation, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
        setProgress(value.toReal());
    });

    connect(m_animation, &QVariantAnimation::finished, this, [this]() {
        const qreal target =
            qBound<qreal>(0.0, m_targetProgress, 1.0);
        if (qAbs(m_progress - target) >= kEpsilon) {
            m_progress = target;
            emit progressChanged(m_progress);
        } else {
            m_progress = target;
        }
        emit finished();
    });
}

QtMaterialTransitionController::~QtMaterialTransitionController() = default;

qreal QtMaterialTransitionController::progress() const noexcept
{
    return m_progress;
}

void QtMaterialTransitionController::setProgress(qreal value)
{
    const qreal clamped = qBound<qreal>(0.0, value, 1.0);
    if (qAbs(m_progress - clamped) < kEpsilon) {
        return;
    }

    m_progress = clamped;
    emit progressChanged(m_progress);
}

void QtMaterialTransitionController::setDuration(int durationMs)
{
    m_durationMs = qMax(0, durationMs);
    if (m_animation) {
        m_animation->setDuration(m_durationMs);
    }
}

int QtMaterialTransitionController::duration() const noexcept
{
    return m_durationMs;
}

void QtMaterialTransitionController::setEasingCurve(const QEasingCurve& curve)
{
    m_easing = curve;
    if (m_animation) {
        m_animation->setEasingCurve(m_easing);
    }
}

QEasingCurve QtMaterialTransitionController::easingCurve() const noexcept
{
    return m_easing;
}

void QtMaterialTransitionController::setReducedMotion(bool reducedMotion)
{
    if (m_reducedMotion == reducedMotion) {
        return;
    }

    m_reducedMotion = reducedMotion;
    if (m_reducedMotion && isRunning()) {
        finish();
    }
}

bool QtMaterialTransitionController::reducedMotion() const noexcept
{
    return m_reducedMotion;
}

bool QtMaterialTransitionController::isRunning() const noexcept
{
    return m_animation
        && m_animation->state() == QAbstractAnimation::Running;
}

void QtMaterialTransitionController::stop()
{
    if (m_animation) {
        m_animation->stop();
    }
}

void QtMaterialTransitionController::finish()
{
    if (m_animation) {
        m_animation->stop();
    }

    const qreal target = qBound<qreal>(0.0, m_targetProgress, 1.0);
    const bool changed = qAbs(m_progress - target) >= kEpsilon;
    m_progress = target;
    if (changed) {
        emit progressChanged(m_progress);
    }
    emit finished();
}

void QtMaterialTransitionController::startForward()
{
    startTo(1.0);
}

void QtMaterialTransitionController::startBackward()
{
    startTo(0.0);
}

void QtMaterialTransitionController::startTo(qreal target)
{
    m_targetProgress = qBound<qreal>(0.0, target, 1.0);

    if (!m_animation) {
        setProgress(m_targetProgress);
        emit finished();
        return;
    }

    if (qAbs(m_progress - m_targetProgress) < kEpsilon) {
        setProgress(m_targetProgress);
        emit finished();
        return;
    }

    if (m_reducedMotion || m_durationMs <= 0) {
        m_animation->stop();
        setProgress(m_targetProgress);
        emit finished();
        return;
    }

    m_animation->stop();
    m_animation->setDuration(m_durationMs);
    m_animation->setEasingCurve(m_easing);
    m_animation->setStartValue(m_progress);
    m_animation->setEndValue(m_targetProgress);
    m_animation->start();
}

void QtMaterialTransitionController::applyMotionStyle(const MotionStyle& style)
{
    setDuration(style.durationMs);
    setEasingCurve(style.easing);
}

void QtMaterialTransitionController::applyMotionToken(
    const Theme& theme,
    MotionToken token)
{
    applyMotionStyle(theme.motion().style(token));
    setReducedMotion(theme.accessibility().reducedMotion);
}

} // namespace QtMaterial
