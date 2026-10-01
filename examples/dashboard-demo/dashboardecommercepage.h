#pragma once

#include <QWidget>
#include <QString>

namespace Ui {
class DashboardEcommercePage;
}

class DashboardEcommercePage final : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardEcommercePage(QWidget* parent = nullptr);
    ~DashboardEcommercePage() override;

signals:
    void messageRequested(const QString& text);

private:
    void applyTheme();
    Ui::DashboardEcommercePage* m_ui = nullptr;
};
