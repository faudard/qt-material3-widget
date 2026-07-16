#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/core/qtmaterialeventcompat.h"
#include "qtmaterial/foundation/qtmaterialinteractionstate.h"
#include "qtmaterial/core/qtmaterialwidget.h"

class QEvent;
class QFocusEvent;

namespace QtMaterial {

class QTMATERIAL3_CORE_EXPORT QtMaterialControl : public QtMaterialWidget
{
    Q_OBJECT

public:
    explicit QtMaterialControl(QWidget* parent = nullptr);
    ~QtMaterialControl() override;

    bool isHovered() const noexcept;
    bool isPressed() const noexcept;
    bool isFocusedControl() const noexcept;

    Density density() const noexcept;
    void setDensity(Density density);

protected:
    const QtMaterialInteractionState& interactionState() const noexcept;
    QtMaterialInteractionState& interactionState() noexcept;

    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    virtual void invalidateResolvedSpec();
    virtual void stateChangedEvent();

    void enterEvent(EnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void changeEvent(QEvent* event) override;

private:
    void syncAutomationState();

    QtMaterialInteractionState m_state;
    Density m_density;
};

} // namespace QtMaterial
