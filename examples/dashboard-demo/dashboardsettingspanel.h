#pragma once

#include <QWidget>
#include <QString>

namespace Ui {
class DashboardSettingsPanel;
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
    class QtMaterialSwitch* m_modeSwitch = nullptr;
    class QtMaterialSwitch* m_contrastSwitch = nullptr;
    class QtMaterialSwitch* m_rtlSwitch = nullptr;
    class QtMaterialSwitch* m_compactSwitch = nullptr;
};
