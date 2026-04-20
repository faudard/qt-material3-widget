#pragma once

#include <QPointer>

#include "qtmaterial/core/qtmaterialoverlaysurface.h"
#include "qtmaterial/specs/qtmaterialnavigationdrawerspec.h"

namespace QtMaterial {

class QtMaterialScrimWidget;
class QtMaterialTransitionController;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialNavigationDrawer : public QtMaterialOverlaySurface
{
    Q_OBJECT
public:
    explicit QtMaterialNavigationDrawer(QWidget* parent = nullptr);
    ~QtMaterialNavigationDrawer() override;

    void open();
    void close();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;
    void syncGeometryToHost() override;

private:
    void ensureSpecResolved() const;

    mutable bool m_specDirty = true;
    mutable NavigationDrawerSpec m_spec;
    QtMaterialScrimWidget* m_scrim = nullptr;
    QtMaterialTransitionController* m_transition = nullptr;
};

} // namespace QtMaterial
