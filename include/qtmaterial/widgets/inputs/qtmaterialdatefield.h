
#pragma once

#include <QDate>
#include <QString>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

class QToolButton;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialDateField : public QtMaterialOutlinedTextField
{
    Q_OBJECT
    Q_PROPERTY(QDate date READ date WRITE setDate NOTIFY dateChanged)
    Q_PROPERTY(QString displayFormat READ displayFormat WRITE setDisplayFormat NOTIFY displayFormatChanged)
    Q_PROPERTY(bool clearable READ isClearable WRITE setClearable NOTIFY clearableChanged)

public:
    explicit QtMaterialDateField(QWidget* parent = nullptr);
    explicit QtMaterialDateField(const QString& labelText, QWidget* parent = nullptr);
    ~QtMaterialDateField() override;

    QDate date() const noexcept;
    void setDate(const QDate& date);

    QString displayFormat() const;
    void setDisplayFormat(const QString& format);

    bool isClearable() const noexcept;
    void setClearable(bool clearable);

    QString placeholderTextForDate() const;
    void setPlaceholderTextForDate(const QString& text);

signals:
    void dateChanged(const QDate& date);
    void displayFormatChanged(const QString& format);
    void clearableChanged(bool clearable);
    void calendarRequested();
    void parseErrorChanged(bool hasError);

protected:
    void contentChangedEvent() override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;

private slots:
    void handleEditorEditingFinished();
    void handleCalendarButtonClicked();
    void handleClearButtonClicked();

private:
    void syncEditorFromDate();
    void syncDateFromEditor();
    void updateTrailingAffordances();
    void updateAccessibilityMetadata();

    QDate m_date;
    QString m_displayFormat;
    QString m_placeholderTextForDate;
    bool m_clearable = false;
    bool m_parseError = false;
    QToolButton* m_calendarButton = nullptr;
    QToolButton* m_clearButton = nullptr;
};
