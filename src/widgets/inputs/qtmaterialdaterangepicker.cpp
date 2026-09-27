#include "qtmaterial/widgets/inputs/qtmaterialdaterangepicker.h"

#include <QHBoxLayout>

#include "qtmaterial/widgets/qtmaterialdatepicker.h"

namespace QtMaterial {

class QtMaterialDateRangePickerPrivate final
{
public:
    QtMaterialDatePicker* startPicker = nullptr;
    QtMaterialDatePicker* endPicker = nullptr;
    bool syncing = false;
};

QtMaterialDateRangePicker::QtMaterialDateRangePicker(QWidget* parent)
    : QWidget(parent)
    , d_ptr(std::make_unique<QtMaterialDateRangePickerPrivate>())
{
    d_ptr->startPicker = new QtMaterialDatePicker(this);
    d_ptr->endPicker = new QtMaterialDatePicker(this);
    setObjectName(QStringLiteral("qtmaterial_date_range_picker"));
    setAccessibleName(tr("Date range"));

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);
    layout->addWidget(d_ptr->startPicker, 1);
    layout->addWidget(d_ptr->endPicker, 1);

    const QDate today = QDate::currentDate();
    d_ptr->startPicker->setSelectedDate(today);
    d_ptr->endPicker->setSelectedDate(today);

    connect(d_ptr->startPicker, &QtMaterialDatePicker::selectedDateChanged, this, [this](const QDate& date) {
        if (d_ptr->syncing) return;
        d_ptr->syncing = true;
        if (d_ptr->endPicker->selectedDate() < date) {
            d_ptr->endPicker->setSelectedDate(date);
        }
        synchronizeConstraints();
        d_ptr->syncing = false;
        emit startDateChanged(date);
        emit dateRangeChanged(startDate(), endDate());
    });
    connect(d_ptr->endPicker, &QtMaterialDatePicker::selectedDateChanged, this, [this](const QDate& date) {
        if (d_ptr->syncing) return;
        d_ptr->syncing = true;
        if (d_ptr->startPicker->selectedDate() > date) {
            d_ptr->startPicker->setSelectedDate(date);
        }
        synchronizeConstraints();
        d_ptr->syncing = false;
        emit endDateChanged(date);
        emit dateRangeChanged(startDate(), endDate());
    });

    synchronizeConstraints();
}

QtMaterialDateRangePicker::~QtMaterialDateRangePicker() = default;

QDate QtMaterialDateRangePicker::startDate() const { return d_ptr->startPicker->selectedDate(); }
QDate QtMaterialDateRangePicker::endDate() const { return d_ptr->endPicker->selectedDate(); }

void QtMaterialDateRangePicker::setStartDate(const QDate& date)
{
    if (!date.isValid()) return;
    setDateRange(date, qMax(date, endDate()));
}

void QtMaterialDateRangePicker::setEndDate(const QDate& date)
{
    if (!date.isValid()) return;
    setDateRange(qMin(startDate(), date), date);
}

void QtMaterialDateRangePicker::setDateRange(const QDate& start, const QDate& end)
{
    if (!start.isValid() || !end.isValid()) return;

    const QDate normalizedStart = qMin(start, end);
    const QDate normalizedEnd = qMax(start, end);

    d_ptr->syncing = true;

    // Widen the constraints before moving either endpoint. Otherwise the
    // constraints from the previous range can clamp a valid new range.
    d_ptr->startPicker->setMaximumDate(normalizedEnd);
    d_ptr->endPicker->setMinimumDate(normalizedStart);

    d_ptr->startPicker->setSelectedDate(normalizedStart);
    d_ptr->endPicker->setSelectedDate(normalizedEnd);

    synchronizeConstraints();
    d_ptr->syncing = false;

    emit startDateChanged(normalizedStart);
    emit endDateChanged(normalizedEnd);
    emit dateRangeChanged(normalizedStart, normalizedEnd);
}

QtMaterialDatePicker* QtMaterialDateRangePicker::startPicker() const noexcept { return d_ptr->startPicker; }
QtMaterialDatePicker* QtMaterialDateRangePicker::endPicker() const noexcept { return d_ptr->endPicker; }

void QtMaterialDateRangePicker::synchronizeConstraints()
{
    d_ptr->endPicker->setMinimumDate(d_ptr->startPicker->selectedDate());
    d_ptr->startPicker->setMaximumDate(d_ptr->endPicker->selectedDate());
}

} // namespace QtMaterial
