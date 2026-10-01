#pragma once

#include <QWidget>
#include <QString>

namespace Ui {
class DashboardSettingsPanel;
}

namespace QtMaterial {
class QtMaterialSwitch;
}

class DashboardSettingsPanel final : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardSettingsPanel(QWidget* parent = nullptr);
    ~DashboardSettingsPanel() override;

signals:
    void closeRequested();
    void compactChanged(bool compact);
    void messageRequested(const QString& text);

private:
    void applyTheme();
    void syncFromTheme();
    Ui::DashboardSettingsPanel* m_ui = nullptr;
    QtMaterial::QtMaterialSwitch* m_modeSwitch = nullptr;
    QtMaterial::QtMaterialSwitch* m_contrastSwitch = nullptr;
    QtMaterial::QtMaterialSwitch* m_rtlSwitch = nullptr;
    QtMaterial::QtMaterialSwitch* m_compactSwitch = nullptr;
};
