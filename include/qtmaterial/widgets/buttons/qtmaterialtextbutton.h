#pragma once

#include "qtmaterial/core/qtmaterialabstractbutton.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QtMaterialRippleController;
class QtMaterialTransitionController;

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
    QSize minimumSizeHint() const override;
    void stateChangedEvent() override;

    virtual ButtonSpec resolveButtonSpec() const;

protected:
    void ensureSpecResolved() const;
    qreal animatedStateLayerOpacity() const noexcept;
    void syncStateLayerAnimation();

    mutable bool m_specDirty = true;
    mutable ButtonSpec m_spec;
    QtMaterialRippleController* m_ripple = nullptr;
    QtMaterialTransitionController* m_stateLayerTransition = nullptr;
};

} // namespace QtMaterial