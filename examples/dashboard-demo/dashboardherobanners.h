#pragma once

#include <QWidget>

#include <functional>

class QBoxLayout;

class DashboardHeroBanners final : public QWidget
{
public:
    explicit DashboardHeroBanners(QWidget* parent = nullptr);

    void setGoNowHandler(const std::function<void()>& handler);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void updateLayoutMode();

    QBoxLayout* m_layout = nullptr;
    QWidget* m_welcomeCard = nullptr;
    QWidget* m_carouselCard = nullptr;
    std::function<void()> m_goNowHandler;
};
