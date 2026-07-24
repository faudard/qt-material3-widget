#pragma once
#include <QAbstractButton>

class QEvent;
class QFocusEvent;
class QKeyEvent;
class QMouseEvent;

#include <QIcon>
#include <QString>

#include "qtmaterial/core/qtmaterialeventcompat.h"
#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/foundation/qtmaterialinteractionstate.h"
#include <QString>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/theme/qtmaterialthemecontexthost.h"

namespace QtMaterial {


class QtMaterialThemeContextBinding;

class QTMATERIAL3_CORE_EXPORT QtMaterialAbstractButton
    : public QAbstractButton
    , public ThemeContextHost
{
    Q_OBJECT
    Q_INTERFACES(QtMaterial::ThemeContextHost)

public:
    explicit QtMaterialAbstractButton(QWidget* parent = nullptr);
    ~QtMaterialAbstractButton() override;

    void setThemeContext(ThemeContext* context);
    ThemeContext* themeContext() const noexcept override;
    ThemeContext* effectiveThemeContext() const noexcept override;

    QString materialComponent() const;
    QString materialVariant() const;
    QString materialRole() const;
    QString materialTestId() const;
    QString materialState() const;

    void setMaterialComponent(const QString& value);
    void setMaterialVariant(const QString& value);
    void setMaterialRole(const QString& value);
    void setMaterialTestId(const QString& value);
    void setMaterialState(const QString& value);

    Density density() const noexcept;
    void setDensity(Density density);

    void setText(const QString& text);
    void setIcon(const QIcon& icon);
    void setIconSize(const QSize& size);
    void setCheckable(bool checkable);
    void setDown(bool down);

signals:
    void effectiveThemeContextChanged(
        QtMaterial::ThemeContext* context);
    void materialMetadataChanged();

protected:
    const Theme& theme() const;
    const QtMaterialInteractionState& interactionState() const noexcept;
    QtMaterialInteractionState& interactionState() noexcept;

    virtual void themeChangedEvent(const QtMaterial::Theme& theme);
    virtual void invalidateResolvedSpec();
    virtual void stateChangedEvent();
    virtual void syncAccessibilityState();
    virtual void contentChangedEvent();

    bool event(QEvent* event) override;
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
    void syncFromButtonState() noexcept;
    void syncAndNotifyIfChanged() noexcept;
    void syncAutomationState();

    QtMaterialInteractionState m_state;
    Density m_density;
    QtMaterialThemeContextBinding* m_themeBinding = nullptr;
};

} // namespace QtMaterial