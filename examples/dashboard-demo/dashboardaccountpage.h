#pragma once

#include <QWidget>

namespace Ui {
class DashboardAccountPage;
}

class DashboardAccountPage final : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardAccountPage(QWidget* parent = nullptr);
    ~DashboardAccountPage() override;

signals:
    void editProfileRequested();
    void messageRequested(const QString& text);

private:
    Ui::DashboardAccountPage* m_ui = nullptr;
};
