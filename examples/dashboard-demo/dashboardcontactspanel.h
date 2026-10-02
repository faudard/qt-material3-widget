#pragma once

#include <QWidget>
#include <QString>

class QShowEvent;

namespace Ui {
class DashboardContactsPanel;
}

class DashboardContactsPanel final : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardContactsPanel(QWidget* parent = nullptr);
    ~DashboardContactsPanel() override;

protected:
    void showEvent(QShowEvent* event) override;

signals:
    void closeRequested();
    void messageRequested(const QString& text);

private:
    void applyTheme();
    Ui::DashboardContactsPanel* m_ui = nullptr;
};
