#pragma once

#include "qtmaterial/core/qtmaterialselectioncontrol.h"
#include <memory>
#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QtMaterialCheckboxPrivate;

class QtMaterialRippleController;
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
 std::unique_ptr<QtMaterialCheckboxPrivate> d;
};

} // namespace QtMaterial