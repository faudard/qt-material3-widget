#pragma once
#include <QAbstractButton>
class QEvent;
class QFocusEvent;
class QKeyEvent;
class QMouseEvent;
#include <QIcon>
#include <QString>
#include "qtmaterial/core/qtmaterialeventcompat.h"
#include "qtmaterial/core/qtmaterialdensity.h"
#include "qtmaterial/core/qtmaterialinteractionstate.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
namespace QtMaterial {
class QTMATERIAL3_CORE_EXPORT QtMaterialAbstractButton : public QAbstractButton
{
    Q_OBJECT
public:
    explicit QtMaterialAbstractButton(QWidget* parent = nullptr);
    ~QtMaterialAbstractButton() override;
    Density density() const noexcept;
    void setDensity(Density density);
    void setText(const QString& text);
    void setIcon(const QIcon& icon);
protected:
    const Theme& theme() const;
    const QtMaterialInteractionState& interactionState() const noexcept;
    QtMaterialInteractionState& interactionState() noexcept;
    virtual void themeChangedEvent(const QtMaterial::Theme& theme);
    virtual void invalidateResolvedSpec();
    virtual void stateChangedEvent();
    virtual void syncAccessibilityState();
    virtual void contentChangedEvent();
    void enterEvent(EnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void changeEvent(QEvent* event) override;
    void checkStateSet() override;
    void nextCheckState() override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
private slots:
    void handleThemeChanged(const QtMaterial::Theme& theme);
private:
    QtMaterialInteractionState m_state;
    Density m_density;
    void syncFromButtonState() noexcept;
    void syncAndNotifyIfChanged() noexcept;
};
} // namespace QtMaterial
