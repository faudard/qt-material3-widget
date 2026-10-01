#pragma once

#include <QWidget>
#include <QString>

namespace Ui {
class DashboardContactsPanel;
}

class DashboardContactsPanel final : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardContactsPanel(QWidget* parent = nullptr);
    ~DashboardContactsPanel() override;

signals:
    void closeRequested();
    void messageRequested(const QString& text);

private:
    void applyTheme();
    Ui::DashboardContactsPanel* m_ui = nullptr;
};
