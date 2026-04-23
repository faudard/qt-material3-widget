#pragma once

#include "qtmaterial/core/qtmaterialselectioncontrol.h"
#include "qtmaterial/specs/qtmaterialcheckboxspec.h"
#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QtMaterialRippleController;
class QtMaterialTransitionController;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialCheckbox : public QtMaterialSelectionControl
{
    Q_OBJECT

public:
    explicit QtMaterialCheckbox(QWidget* parent = nullptr);
    ~QtMaterialCheckbox() override;

    bool isTristate() const noexcept;
    void setTristate(bool tristate);

    Qt::CheckState checkState() const noexcept;
    void setCheckState(Qt::CheckState state);

signals:
    void checkStateChanged(Qt::CheckState state);

protected:
    void paintEvent(QPaintEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;
    void mousePressEvent(QMouseEvent* event) override;
    void nextCheckState() override;

private:
    void resolveSpecIfNeeded() const;
    void syncCheckedFromCheckState();
    qreal targetTransitionProgress() const noexcept;

    mutable bool m_specDirty = true;
    mutable CheckboxSpec m_spec;

    Qt::CheckState m_checkState = Qt::Unchecked;
    bool m_tristate = false;

    QtMaterialRippleController* m_ripple = nullptr;
    QtMaterialTransitionController* m_transition = nullptr;
};

} // namespace QtMaterial