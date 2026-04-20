#pragma once

#include "qtmaterial/core/qtmaterialoverlaysurface.h"
#include "qtmaterial/specs/qtmaterialbottomsheetspec.h"

namespace QtMaterial {

class QtMaterialScrimWidget;
class QtMaterialTransitionController;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialBottomSheet : public QtMaterialOverlaySurface
{
    Q_OBJECT
public:
    explicit QtMaterialBottomSheet(QWidget* parent = nullptr);
    ~QtMaterialBottomSheet() override;

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
    mutable BottomSheetSpec m_spec;
    QtMaterialScrimWidget* m_scrim = nullptr;
    QtMaterialTransitionController* m_transition = nullptr;
};

} // namespace QtMaterial
