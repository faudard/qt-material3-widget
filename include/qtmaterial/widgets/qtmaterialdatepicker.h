#pragma once

#include <QDate>
#include <QFrame>

#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"

class QCalendarWidget;
class QLabel;
class QToolButton;

namespace QtMaterial {

class QtMaterialDatePicker : public QFrame {
    Q_OBJECT
    Q_PROPERTY(QDate selectedDate READ selectedDate WRITE setSelectedDate NOTIFY selectedDateChanged)
    Q_PROPERTY(QDate minimumDate READ minimumDate WRITE setMinimumDate)
    Q_PROPERTY(QDate maximumDate READ maximumDate WRITE setMaximumDate)

public:
    explicit QtMaterialDatePicker(QWidget* parent = nullptr);
    ~QtMaterialDatePicker() override;

    QDate selectedDate() const;
    void setSelectedDate(const QDate& date);

    QDate minimumDate() const;
    void setMinimumDate(const QDate& date);

    QDate maximumDate() const;
    void setMaximumDate(const QDate& date);

    DatePickerSpec spec() const;
    void setSpec(const DatePickerSpec& spec);

public slots:
    void showPreviousMonth();
    void showNextMonth();
    void showToday();

signals:
    void selectedDateChanged(const QDate& date);
    void activated(const QDate& date);

protected:
    void changeEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void buildUi();
    void updateHeader();
    void applySpec();

    DatePickerSpec m_spec;
    QCalendarWidget* m_calendar = nullptr;
    QLabel* m_headerLabel = nullptr;
    QToolButton* m_prevButton = nullptr;
    QToolButton* m_nextButton = nullptr;
    QToolButton* m_todayButton = nullptr;
};

} // namespace QtMaterial
