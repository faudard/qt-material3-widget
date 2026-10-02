#pragma once

#include <QWidget>
#include <QString>

class QShowEvent;

namespace Ui {
class DashboardNotificationsPanel;
}

class DashboardNotificationsPanel final : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardNotificationsPanel(QWidget* parent = nullptr);
    ~DashboardNotificationsPanel() override;

protected:
    void showEvent(QShowEvent* event) override;

signals:
    void closeRequested();
    void settingsRequested();
    void messageRequested(const QString& text);

private:
    void applyTheme();
    Ui::DashboardNotificationsPanel* m_ui = nullptr;
};
