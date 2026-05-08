
#pragma once

#include <memory>
#include <QDate>
#include <QString>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"


class QtMaterialDateFieldPrivate;
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialDateField : public QtMaterial::QtMaterialOutlinedTextField
{
    Q_OBJECT
    Q_PROPERTY(QDate date READ date WRITE setDate NOTIFY dateChanged)
    Q_PROPERTY(QString displayFormat READ displayFormat WRITE setDisplayFormat NOTIFY displayFormatChanged)
    Q_PROPERTY(bool clearable READ isClearable WRITE setClearable NOTIFY clearableChanged)
  Q_PROPERTY(QDate minimumDate READ minimumDate WRITE setMinimumDate NOTIFY dateRangeChanged)
  Q_PROPERTY(QDate maximumDate READ maximumDate WRITE setMaximumDate NOTIFY dateRangeChanged)
  Q_PROPERTY(bool dateAcceptable READ isDateAcceptable NOTIFY dateAcceptableChanged)
  Q_PROPERTY(QString accessibilitySummary READ accessibilitySummary NOTIFY accessibilitySummaryChanged)

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

  QDate minimumDate() const noexcept;

  void setMinimumDate(const QDate& date);

  QDate maximumDate() const noexcept;

  void setMaximumDate(const QDate& date);

  bool isDateAcceptable() const noexcept;

  bool hasParseError() const noexcept;

  QString accessibilitySummary() const;

signals:
    void dateChanged(const QDate& date);
    void displayFormatChanged(const QString& format);
    void clearableChanged(bool clearable);
    void calendarRequested();
    void parseErrorChanged(bool hasError);

  void dateRangeChanged(const QDate& minimumDate, const QDate& maximumDate);

  void dateAcceptableChanged(bool acceptable);

  void accessibilitySummaryChanged(const QString& summary);

protected:
    void contentChangedEvent() override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;

private slots:
    void handleEditorEditingFinished();
    void handleCalendarButtonClicked();
    void handleClearButtonClicked();

private:
    friend class QtMaterialDateFieldPrivate;
    std::unique_ptr<QtMaterialDateFieldPrivate> d_ptr;

};
