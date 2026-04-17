#pragma once

#include <QWidget>

class QComboBox;
class QLabel;
class QListWidget;
class QPushButton;

class ThemePreviewWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ThemePreviewWindow(QWidget* parent = nullptr);

private slots:
    void rebuildTheme();

private:
    void populateRoles();

    QComboBox* m_modeCombo;
    QPushButton* m_applyButton;
    QListWidget* m_rolesList;
    QLabel* m_titleLabel;
};
