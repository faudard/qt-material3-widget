#pragma once
#include "qtmaterial/core/qtmaterialabstractbutton.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/qtmaterialglobal.h"
namespace QtMaterial {
class QtMaterialRippleController;
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialTextButton : public QtMaterialAbstractButton
{
    Q_OBJECT
public:
    explicit QtMaterialTextButton(QWidget* parent = nullptr);
    ~QtMaterialTextButton() override;
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void resolveSpecIfNeeded() const;
    mutable bool m_specDirty = true;
    mutable ButtonSpec m_spec;
    QtMaterialRippleController* m_ripple = nullptr;
};
} // namespace QtMaterial
