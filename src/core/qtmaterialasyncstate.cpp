#include "qtmaterial/core/qtmaterialasyncstate.h"

#include <QtGlobal>

namespace QtMaterial {

QtMaterialAsyncState::QtMaterialAsyncState() = default;

bool QtMaterialAsyncState::isBusy() const noexcept { return m_busy; }
void QtMaterialAsyncState::setBusy(bool busy) noexcept { m_busy = busy; }

bool QtMaterialAsyncState::isIndeterminate() const noexcept { return m_indeterminate; }
void QtMaterialAsyncState::setIndeterminate(bool indeterminate) noexcept { m_indeterminate = indeterminate; }

bool QtMaterialAsyncState::hasProgress() const noexcept { return m_hasProgress; }
double QtMaterialAsyncState::progress() const noexcept { return m_progress; }

void QtMaterialAsyncState::setProgress(double progress) noexcept
{
    m_hasProgress = true;
    m_indeterminate = false;
    if (progress < 0.0) progress = 0.0;
    if (progress > 1.0) progress = 1.0;
    m_progress = progress;
}

void QtMaterialAsyncState::clearProgress() noexcept
{
    m_hasProgress = false;
    m_progress = -1.0;
    m_indeterminate = true;
}

QString QtMaterialAsyncState::statusText() const { return m_statusText; }
void QtMaterialAsyncState::setStatusText(const QString& text) { m_statusText = text; }

QStringList QtMaterialAsyncState::stateNames() const
{
    QStringList names;
    if (m_busy) names << QStringLiteral("busy");
    if (m_indeterminate) names << QStringLiteral("indeterminate");
    if (m_hasProgress) names << QStringLiteral("progress");
    if (!m_statusText.isEmpty()) names << QStringLiteral("statusText");
    return names;
}

QString QtMaterialAsyncState::toPropertyString() const
{
    return stateNames().join(QLatin1Char(' '));
}

} // namespace QtMaterial
