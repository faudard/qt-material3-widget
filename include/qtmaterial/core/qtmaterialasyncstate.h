#pragma once

#include <QString>
#include <QStringList>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QTMATERIAL3_CORE_EXPORT QtMaterialAsyncState {
public:
    QtMaterialAsyncState();

    bool isBusy() const noexcept;
    void setBusy(bool busy) noexcept;

    bool isIndeterminate() const noexcept;
    void setIndeterminate(bool indeterminate) noexcept;

    bool hasProgress() const noexcept;
    double progress() const noexcept;
    void setProgress(double progress) noexcept;
    void clearProgress() noexcept;

    QString statusText() const;
    void setStatusText(const QString& text);

    QStringList stateNames() const;
    QString toPropertyString() const;

private:
    bool m_busy = false;
    bool m_indeterminate = true;
    bool m_hasProgress = false;
    double m_progress = -1.0;
    QString m_statusText;
};

} // namespace QtMaterial
