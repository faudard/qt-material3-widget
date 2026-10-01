#pragma once

#include <QWidget>
#include <QString>

namespace Ui {
class DashboardInvoicePage;
}

class DashboardInvoicePage final : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardInvoicePage(QWidget* parent = nullptr);
    ~DashboardInvoicePage() override;

signals:
    void messageRequested(const QString& text);

private:
    void applyTheme();
    Ui::DashboardInvoicePage* m_ui = nullptr;
};
