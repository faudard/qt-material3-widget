#pragma once
#include <QWidget>
class QLabel;
class QPushButton;
class ThemePreviewWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ThemePreviewWindow(QWidget* parent = nullptr);
private slots:
    void toggleMode();
private:
    void refreshPreview();
    QLabel* m_label;
    QPushButton* m_button;
    bool m_dark;
};
