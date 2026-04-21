#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"

#include <QVariantAnimation>
#include <QtGlobal>

namespace QtMaterial {

namespace {
constexpr int kFallbackDurationMs = 180;
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

    connect(m_animation, &QVariantAnimation::finished, this, &QtMaterialTransitionController::finished);
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
        m_animation->setDuration(m_durationMs > 0 ? m_durationMs : kFallbackDurationMs);
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
    if (!m_animation) {
        setProgress(target);
        emit finished();
        return;
    }

    if (qAbs(m_progress - target) < kEpsilon) {
        setProgress(target);
        emit finished();
        return;
    }

    m_animation->stop();
    m_animation->setDuration(m_durationMs > 0 ? m_durationMs : kFallbackDurationMs);
    m_animation->setEasingCurve(m_easing);
    m_animation->setStartValue(m_progress);
    m_animation->setEndValue(target);
    m_animation->start();
}

} // namespace QtMaterial