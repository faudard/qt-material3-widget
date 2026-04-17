#pragma once
#include "qtmaterial/core/qtmaterialselectioncontrol.h"
#include "qtmaterial/specs/qtmaterialcheckboxspec.h"
#include "qtmaterial/qtmaterialglobal.h"
namespace QtMaterial {
class QtMaterialRippleController;
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialCheckbox : public QtMaterialSelectionControl
{
    Q_OBJECT
public:
    explicit QtMaterialCheckbox(QWidget* parent = nullptr);
    ~QtMaterialCheckbox() override;
protected:
    void paintEvent(QPaintEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;
    void mousePressEvent(QMouseEvent* event) override;
private:
    void resolveSpecIfNeeded() const;
    mutable bool m_specDirty = true;
    mutable CheckboxSpec m_spec;
    QtMaterialRippleController* m_ripple = nullptr;
};
} // namespace QtMaterial
