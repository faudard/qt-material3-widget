#pragma once

#include <QWidget>
#include <QString>

namespace Ui {
class DashboardAccountPanel;
}

class DashboardAccountPanel final : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardAccountPanel(QWidget* parent = nullptr);
    ~DashboardAccountPanel() override;

signals:
    void closeRequested();
    void navigateRequested(int pageIndex);
    void messageRequested(const QString& text);

private:
    void applyTheme();
    Ui::DashboardAccountPanel* m_ui = nullptr;
};
