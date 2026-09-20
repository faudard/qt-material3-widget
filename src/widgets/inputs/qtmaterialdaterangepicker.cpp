#include "qtmaterial/widgets/inputs/qtmaterialdaterangepicker.h"

#include <QHBoxLayout>

#include "qtmaterial/widgets/qtmaterialdatepicker.h"

namespace QtMaterial {

QtMaterialDateRangePicker::QtMaterialDateRangePicker(QWidget* parent)
    : QWidget(parent)
    , m_startPicker(new QtMaterialDatePicker(this))
    , m_endPicker(new QtMaterialDatePicker(this))
{
    setObjectName(QStringLiteral("qtmaterial_date_range_picker"));
    setAccessibleName(tr("Date range"));

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);
    layout->addWidget(m_startPicker, 1);
    layout->addWidget(m_endPicker, 1);

    const QDate today = QDate::currentDate();
    m_startPicker->setSelectedDate(today);
    m_endPicker->setSelectedDate(today);

    connect(m_startPicker, &QtMaterialDatePicker::selectedDateChanged, this, [this](const QDate& date) {
        if (m_syncing) return;
        m_syncing = true;
        if (m_endPicker->selectedDate() < date) {
            m_endPicker->setSelectedDate(date);
        }
        synchronizeConstraints();
        m_syncing = false;
        emit startDateChanged(date);
        emit dateRangeChanged(startDate(), endDate());
    });
    connect(m_endPicker, &QtMaterialDatePicker::selectedDateChanged, this, [this](const QDate& date) {
        if (m_syncing) return;
        m_syncing = true;
        if (m_startPicker->selectedDate() > date) {
            m_startPicker->setSelectedDate(date);
        }
        synchronizeConstraints();
        m_syncing = false;
        emit endDateChanged(date);
        emit dateRangeChanged(startDate(), endDate());
    });

    synchronizeConstraints();
}

QtMaterialDateRangePicker::~QtMaterialDateRangePicker() = default;

QDate QtMaterialDateRangePicker::startDate() const { return m_startPicker->selectedDate(); }
QDate QtMaterialDateRangePicker::endDate() const { return m_endPicker->selectedDate(); }

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
    m_syncing = true;
    m_startPicker->setSelectedDate(normalizedStart);
    m_endPicker->setSelectedDate(normalizedEnd);
    synchronizeConstraints();
    m_syncing = false;
    emit startDateChanged(normalizedStart);
    emit endDateChanged(normalizedEnd);
    emit dateRangeChanged(normalizedStart, normalizedEnd);
}

QtMaterialDatePicker* QtMaterialDateRangePicker::startPicker() const noexcept { return m_startPicker; }
QtMaterialDatePicker* QtMaterialDateRangePicker::endPicker() const noexcept { return m_endPicker; }

void QtMaterialDateRangePicker::synchronizeConstraints()
{
    m_endPicker->setMinimumDate(m_startPicker->selectedDate());
    m_startPicker->setMaximumDate(m_endPicker->selectedDate());
}

} // namespace QtMaterial
