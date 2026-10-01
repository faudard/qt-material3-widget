#pragma once

#include <QWidget>
#include <QString>

class QShowEvent;

namespace Ui {
class DashboardAppsPage;
}

class DashboardAppsPage final : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardAppsPage(QWidget* parent = nullptr);
    ~DashboardAppsPage() override;

protected:
    void showEvent(QShowEvent* event) override;

signals:
    void messageRequested(const QString& text);

private:
    void applyTheme();
    Ui::DashboardAppsPage* m_ui = nullptr;
};
