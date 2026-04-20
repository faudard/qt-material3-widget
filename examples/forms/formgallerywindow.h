#pragma once

#include <QMainWindow>

class QListWidget;
class QStackedWidget;

class FormGalleryWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit FormGalleryWindow(QWidget *parent = nullptr);
    ~FormGalleryWindow() override;

private:
    void buildUi();
    void wireUi();

    QListWidget *m_nav = nullptr;
    QStackedWidget *m_stack = nullptr;
};
