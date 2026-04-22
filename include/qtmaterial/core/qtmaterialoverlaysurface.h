#pragma once

#include <QPointer>

#include "qtmaterial/core/qtmaterialsurface.h"

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
    virtual void syncGeometryToHost();

private:
    QPointer<QWidget> m_hostWidget;
    bool m_restoreVisibilityOnHostShow;
};

} // namespace QtMaterial