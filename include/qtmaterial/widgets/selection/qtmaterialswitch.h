#pragma once
#include "qtmaterial/core/qtmaterialselectioncontrol.h"
#include "qtmaterial/specs/qtmaterialswitchspec.h"
#include "qtmaterial/qtmaterialglobal.h"
namespace QtMaterial {
class QtMaterialTransitionController;
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSwitch : public QtMaterialSelectionControl
{
    Q_OBJECT
public:
    explicit QtMaterialSwitch(QWidget* parent = nullptr);
    ~QtMaterialSwitch() override;
protected:
    void paintEvent(QPaintEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;
    QSize sizeHint() const override;
private:
    void resolveSpecIfNeeded() const;
    mutable bool m_specDirty = true;
    mutable SwitchSpec m_spec;
    QtMaterialTransitionController* m_transition = nullptr;
};
} // namespace QtMaterial
