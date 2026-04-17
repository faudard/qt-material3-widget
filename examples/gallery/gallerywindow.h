#pragma once
#include <QMainWindow>
class QTabWidget;
class GalleryWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit GalleryWindow(QWidget* parent = nullptr);
    ~GalleryWindow() override;
private:
    QTabWidget* m_tabs = nullptr;
};
