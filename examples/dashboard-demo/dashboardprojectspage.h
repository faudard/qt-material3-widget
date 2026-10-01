#pragma once

#include <QWidget>
#include <QString>

namespace Ui {
class DashboardProjectsPage;
}

class DashboardProjectsPage final : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardProjectsPage(QWidget* parent = nullptr);
    ~DashboardProjectsPage() override;

signals:
    void messageRequested(const QString& text);

private:
    void applyTheme();
    Ui::DashboardProjectsPage* m_ui = nullptr;
};
