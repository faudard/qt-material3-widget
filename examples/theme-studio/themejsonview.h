#pragma once

#include <QByteArray>
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
    void setValidationResult(bool valid, const QString& message);

signals:
    void copyRequested();
    void importRequested();
    void exportRequested();
    void validateRequested(const QByteArray& json);
    void applyRequested(const QByteArray& json);

private:
    QPlainTextEdit* m_editor {};
    QPushButton* m_copyButton {};
    QPushButton* m_importButton {};
    QPushButton* m_exportButton {};
    QPushButton* m_validateButton {};
    QPushButton* m_applyButton {};
    class QLabel* m_validationLabel {};
};
