#include "qtmaterial/widgets/qtmaterialdatepicker.h"
#include <memory>

#include <QCalendarWidget>
#include <QDate>
#include <QEvent>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QPalette>
#include <QTextCharFormat>
#include <QToolButton>
#include <QVBoxLayout>
#include <qlocale.h>

namespace QtMaterial {

struct QtMaterialDatePickerPrivate {
    DatePickerSpec m_spec = defaultDatePickerSpec();
    QCalendarWidget* m_calendar = nullptr;
    QLabel* m_headerLabel = nullptr;
    QToolButton* m_prevButton = nullptr;
    QToolButton* m_nextButton = nullptr;
    QToolButton* m_todayButton = nullptr;
};

QtMaterialDatePicker::QtMaterialDatePicker(QWidget* parent)
    : QFrame(parent)
    , d_ptr(std::make_unique<QtMaterialDatePickerPrivate>())
{
    setObjectName(QStringLiteral("QtMaterialDatePicker"));
    setFocusPolicy(Qt::StrongFocus);
    setFrameShape(QFrame::NoFrame);
    buildUi();
    applySpec();
    updateHeader();
}

QtMaterialDatePicker::~QtMaterialDatePicker() = default;

QDate QtMaterialDatePicker::selectedDate() const
{
    return d_ptr->m_calendar->selectedDate();
}

void QtMaterialDatePicker::setSelectedDate(const QDate& date)
{
    if (!date.isValid() || d_ptr->m_calendar->selectedDate() == date) {
        return;
    }
    d_ptr->m_calendar->setSelectedDate(date);
    updateHeader();
    emit selectedDateChanged(date);
}

QDate QtMaterialDatePicker::minimumDate() const
{
    return d_ptr->m_calendar->minimumDate();
}

void QtMaterialDatePicker::setMinimumDate(const QDate& date)
{
    if (date.isValid()) {
        d_ptr->m_calendar->setMinimumDate(date);
    }
}

QDate QtMaterialDatePicker::maximumDate() const
{
    return d_ptr->m_calendar->maximumDate();
}

void QtMaterialDatePicker::setMaximumDate(const QDate& date)
{
    if (date.isValid()) {
        d_ptr->m_calendar->setMaximumDate(date);
    }
}

DatePickerSpec QtMaterialDatePicker::spec() const
{
    return d_ptr->m_spec;
}

void QtMaterialDatePicker::setSpec(const DatePickerSpec& spec)
{
    d_ptr->m_spec = spec;
    applySpec();
    update();
}

void QtMaterialDatePicker::showPreviousMonth()
{
    d_ptr->m_calendar->showPreviousMonth();
    updateHeader();
}

void QtMaterialDatePicker::showNextMonth()
{
    d_ptr->m_calendar->showNextMonth();
    updateHeader();
}

void QtMaterialDatePicker::showToday()
{
    setSelectedDate(QDate::currentDate());
    d_ptr->m_calendar->showSelectedDate();
}

void QtMaterialDatePicker::changeEvent(QEvent* event)
{
    QFrame::changeEvent(event);
    if (event->type() == QEvent::EnabledChange || event->type() == QEvent::PaletteChange || event->type() == QEvent::FontChange) {
        applySpec();
    }
}

void QtMaterialDatePicker::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_PageUp:
        showPreviousMonth();
        event->accept();
        return;
    case Qt::Key_PageDown:
        showNextMonth();
        event->accept();
        return;
    case Qt::Key_Home:
        showToday();
        event->accept();
        return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        emit activated(selectedDate());
        event->accept();
        return;
    default:
        break;
    }
    QFrame::keyPressEvent(event);
}

void QtMaterialDatePicker::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(d_ptr->m_spec.backgroundColor);
    painter.drawRoundedRect(rect().adjusted(0, 0, -1, -1), d_ptr->m_spec.cornerRadius, d_ptr->m_spec.cornerRadius);

    if (hasFocus()) {
        QPen pen(d_ptr->m_spec.focusRingColor, d_ptr->m_spec.focusRingWidth);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect().adjusted(2, 2, -3, -3), d_ptr->m_spec.cornerRadius, d_ptr->m_spec.cornerRadius);
    }
}

void QtMaterialDatePicker::buildUi()
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(d_ptr->m_spec.contentMargins);
    root->setSpacing(8);

    auto* header = new QGridLayout();
    header->setContentsMargins(0, 0, 0, 0);
    header->setColumnStretch(1, 1);

    d_ptr->m_headerLabel = new QLabel(this);
    d_ptr->m_headerLabel->setObjectName(QStringLiteral("QtMaterialDatePickerHeader"));

    d_ptr->m_prevButton = new QToolButton(this);
    d_ptr->m_prevButton->setText(QStringLiteral("‹"));
    d_ptr->m_prevButton->setAccessibleName(tr("Previous month"));
    connect(d_ptr->m_prevButton, &QToolButton::clicked, this, &QtMaterialDatePicker::showPreviousMonth);

    d_ptr->m_nextButton = new QToolButton(this);
    d_ptr->m_nextButton->setText(QStringLiteral("›"));
    d_ptr->m_nextButton->setAccessibleName(tr("Next month"));
    connect(d_ptr->m_nextButton, &QToolButton::clicked, this, &QtMaterialDatePicker::showNextMonth);

    d_ptr->m_todayButton = new QToolButton(this);
    d_ptr->m_todayButton->setText(tr("Today"));
    d_ptr->m_todayButton->setAccessibleName(tr("Show today"));
    connect(d_ptr->m_todayButton, &QToolButton::clicked, this, &QtMaterialDatePicker::showToday);

    header->addWidget(d_ptr->m_prevButton, 0, 0);
    header->addWidget(d_ptr->m_headerLabel, 0, 1);
    header->addWidget(d_ptr->m_todayButton, 0, 2);
    header->addWidget(d_ptr->m_nextButton, 0, 3);
    root->addLayout(header);

    d_ptr->m_calendar = new QCalendarWidget(this);
    d_ptr->m_calendar->setObjectName(QStringLiteral("QtMaterialDatePickerCalendar"));
    d_ptr->m_calendar->setGridVisible(false);
    d_ptr->m_calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    d_ptr->m_calendar->setNavigationBarVisible(false);
    d_ptr->m_calendar->setFocusPolicy(Qt::StrongFocus);
    connect(d_ptr->m_calendar, &QCalendarWidget::selectionChanged, this, [this]() {
        updateHeader();
        emit selectedDateChanged(d_ptr->m_calendar->selectedDate());
    });
    connect(d_ptr->m_calendar, &QCalendarWidget::activated, this, &QtMaterialDatePicker::activated);
    root->addWidget(d_ptr->m_calendar);
}

void QtMaterialDatePicker::updateHeader()
{
    const QDate date = d_ptr->m_calendar->selectedDate();
    d_ptr->m_headerLabel->setText(QLocale().toString(date, QStringLiteral("MMMM yyyy")));
    setAccessibleName(tr("Date picker, selected %1").arg(QLocale().toString(date, QLocale::LongFormat)));
}

void QtMaterialDatePicker::applySpec()
{
    setAutoFillBackground(false);
    if (layout()) {
        layout()->setContentsMargins(d_ptr->m_spec.contentMargins);
    }

    QPalette palette = this->palette();
    palette.setColor(QPalette::WindowText, d_ptr->m_spec.foregroundColor);
    palette.setColor(QPalette::Text, d_ptr->m_spec.foregroundColor);
    palette.setColor(QPalette::ButtonText, d_ptr->m_spec.foregroundColor);
    palette.setColor(QPalette::Base, d_ptr->m_spec.backgroundColor);
    palette.setColor(QPalette::Window, d_ptr->m_spec.backgroundColor);
    setPalette(palette);

    d_ptr->m_headerLabel->setFont(d_ptr->m_spec.headlineFont);
    d_ptr->m_headerLabel->setStyleSheet(QStringLiteral("color:%1;").arg(d_ptr->m_spec.headlineColor.name()));
    d_ptr->m_calendar->setFont(d_ptr->m_spec.dayFont);

    QTextCharFormat selected;
    selected.setBackground(d_ptr->m_spec.selectedDateColor);
    selected.setForeground(d_ptr->m_spec.selectedDateTextColor);
    d_ptr->m_calendar->setDateTextFormat(d_ptr->m_calendar->selectedDate(), selected);

    const QString style = QStringLiteral(
        "QCalendarWidget QWidget { background: %1; color: %2; }"
        "QCalendarWidget QAbstractItemView { selection-background-color: %3; selection-color: %4; outline: 0; }"
        "QToolButton { border: 0; border-radius: 16px; padding: 6px 10px; color: %2; background: transparent; }"
        "QToolButton:hover { background: rgba(%5,%6,%7,%8); }")
        .arg(d_ptr->m_spec.backgroundColor.name(),
             d_ptr->m_spec.foregroundColor.name(),
             d_ptr->m_spec.selectedDateColor.name(),
             d_ptr->m_spec.selectedDateTextColor.name())
        .arg(d_ptr->m_spec.hoverColor.red())
        .arg(d_ptr->m_spec.hoverColor.green())
        .arg(d_ptr->m_spec.hoverColor.blue())
        .arg(d_ptr->m_spec.hoverColor.alpha());
    setStyleSheet(style);
}

} // namespace QtMaterial
