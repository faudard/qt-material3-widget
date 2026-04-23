#pragma once

#include <QWidget>

namespace QtMaterial {
class Theme;
}

class QPlainTextEdit;
class QPushButton;

class ThemeJsonView : public QWidget
{
    Q_OBJECT

public:
    explicit ThemeJsonView(QWidget* parent = nullptr);

public slots:
    void applyTheme(const QtMaterial::Theme& theme);

signals:
    void copyRequested();
    void importRequested();
    void exportRequested();

private:
    QPlainTextEdit* m_editor {};
    QPushButton* m_copyButton {};
    QPushButton* m_importButton {};
    QPushButton* m_exportButton {};
};
