#pragma once

#include <QList>
#include <QPointer>

#include "qtmaterial/core/qtmaterialsurface.h"

class QEvent;
class QShowEvent;

namespace QtMaterial {

class QTMATERIAL3_CORE_EXPORT QtMaterialOverlaySurface : public QtMaterialSurface
{
    Q_OBJECT

public:
    explicit QtMaterialOverlaySurface(QWidget* parent = nullptr);
    ~QtMaterialOverlaySurface() override;

    QWidget* hostWidget() const noexcept;
    void setHostWidget(QWidget* host);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void showEvent(QShowEvent* event) override;
    virtual void syncGeometryToHost();

private:
    void rebuildHostGeometryWatchers();
    void clearHostGeometryWatchers();
    bool isHostGeometryWatcher(QObject* object) const noexcept;

    QPointer<QWidget> m_hostWidget;
    QList<QPointer<QWidget>> m_hostGeometryWatchers;
    bool m_restoreVisibilityOnHostShow;
};

} // namespace QtMaterial