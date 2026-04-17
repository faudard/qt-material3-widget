#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
namespace QtMaterial {
QtMaterialTransitionController::QtMaterialTransitionController(QObject* parent) : QObject(parent) {}
QtMaterialTransitionController::~QtMaterialTransitionController() = default;
qreal QtMaterialTransitionController::progress() const noexcept { return m_progress; }
void QtMaterialTransitionController::setProgress(qreal value)
{
    if (qFuzzyCompare(m_progress, value)) return;
    m_progress = value;
    emit progressChanged(m_progress);
}
void QtMaterialTransitionController::setDuration(int durationMs) { m_durationMs = durationMs; }
int QtMaterialTransitionController::duration() const noexcept { return m_durationMs; }
void QtMaterialTransitionController::startForward() { setProgress(1.0); emit finished(); }
void QtMaterialTransitionController::startBackward() { setProgress(0.0); emit finished(); }
} // namespace QtMaterial
