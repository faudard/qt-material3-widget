#pragma once

#include <QDate>
#include <QWidget>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QtMaterialDatePicker;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialDateRangePicker : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QDate startDate READ startDate WRITE setStartDate NOTIFY startDateChanged)
    Q_PROPERTY(QDate endDate READ endDate WRITE setEndDate NOTIFY endDateChanged)

public:
    explicit QtMaterialDateRangePicker(QWidget* parent = nullptr);
    ~QtMaterialDateRangePicker() override;

    QDate startDate() const;
    QDate endDate() const;

    void setStartDate(const QDate& date);
    void setEndDate(const QDate& date);
    void setDateRange(const QDate& start, const QDate& end);

    QtMaterialDatePicker* startPicker() const noexcept;
    QtMaterialDatePicker* endPicker() const noexcept;

signals:
    void startDateChanged(const QDate& date);
    void endDateChanged(const QDate& date);
    void dateRangeChanged(const QDate& start, const QDate& end);

private:
    void synchronizeConstraints();

    QtMaterialDatePicker* m_startPicker = nullptr;
    QtMaterialDatePicker* m_endPicker = nullptr;
    bool m_syncing = false;
};

} // namespace QtMaterial
