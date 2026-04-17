#pragma once
#include <QEnterEvent>
#include <QFocusEvent>
#include "qtmaterial/core/qtmaterialdensity.h"
#include "qtmaterial/core/qtmaterialinteractionstate.h"
#include "qtmaterial/core/qtmaterialwidget.h"
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QEnterEvent>
#else
#include <QEvent>
#endif
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *event) override;
#else
    void enterEvent(QEvent *event) override;
#endif
    void leaveEvent(QEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void changeEvent(QEvent* event) override;
private:
    QtMaterialInteractionState m_state;
    Density m_density;
};
} // namespace QtMaterial
