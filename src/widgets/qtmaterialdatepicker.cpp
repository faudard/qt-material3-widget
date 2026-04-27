#include "qtmaterial/widgets/qtmaterialdatepicker.h"

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

namespace QtMaterial {

QtMaterialDatePicker::QtMaterialDatePicker(QWidget* parent)
    : QFrame(parent), m_spec(defaultDatePickerSpec())
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
    return m_calendar->selectedDate();
}

void QtMaterialDatePicker::setSelectedDate(const QDate& date)
{
    if (!date.isValid() || m_calendar->selectedDate() == date) {
        return;
    }
    m_calendar->setSelectedDate(date);
    updateHeader();
    emit selectedDateChanged(date);
}

QDate QtMaterialDatePicker::minimumDate() const
{
    return m_calendar->minimumDate();
}

void QtMaterialDatePicker::setMinimumDate(const QDate& date)
{
    if (date.isValid()) {
        m_calendar->setMinimumDate(date);
    }
}

QDate QtMaterialDatePicker::maximumDate() const
{
    return m_calendar->maximumDate();
}

void QtMaterialDatePicker::setMaximumDate(const QDate& date)
{
    if (date.isValid()) {
        m_calendar->setMaximumDate(date);
    }
}

DatePickerSpec QtMaterialDatePicker::spec() const
{
    return m_spec;
}

void QtMaterialDatePicker::setSpec(const DatePickerSpec& spec)
{
    m_spec = spec;
    applySpec();
    update();
}

void QtMaterialDatePicker::showPreviousMonth()
{
    m_calendar->showPreviousMonth();
    updateHeader();
}

void QtMaterialDatePicker::showNextMonth()
{
    m_calendar->showNextMonth();
    updateHeader();
}

void QtMaterialDatePicker::showToday()
{
    setSelectedDate(QDate::currentDate());
    m_calendar->showSelectedDate();
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
    painter.setBrush(m_spec.backgroundColor);
    painter.drawRoundedRect(rect().adjusted(0, 0, -1, -1), m_spec.cornerRadius, m_spec.cornerRadius);

    if (hasFocus()) {
        QPen pen(m_spec.focusRingColor, m_spec.focusRingWidth);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect().adjusted(2, 2, -3, -3), m_spec.cornerRadius, m_spec.cornerRadius);
    }
}

void QtMaterialDatePicker::buildUi()
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(m_spec.contentMargins);
    root->setSpacing(8);

    auto* header = new QGridLayout();
    header->setContentsMargins(0, 0, 0, 0);
    header->setColumnStretch(1, 1);

    m_headerLabel = new QLabel(this);
    m_headerLabel->setObjectName(QStringLiteral("QtMaterialDatePickerHeader"));

    m_prevButton = new QToolButton(this);
    m_prevButton->setText(QStringLiteral("‹"));
    m_prevButton->setAccessibleName(tr("Previous month"));
    connect(m_prevButton, &QToolButton::clicked, this, &QtMaterialDatePicker::showPreviousMonth);

    m_nextButton = new QToolButton(this);
    m_nextButton->setText(QStringLiteral("›"));
    m_nextButton->setAccessibleName(tr("Next month"));
    connect(m_nextButton, &QToolButton::clicked, this, &QtMaterialDatePicker::showNextMonth);

    m_todayButton = new QToolButton(this);
    m_todayButton->setText(tr("Today"));
    m_todayButton->setAccessibleName(tr("Show today"));
    connect(m_todayButton, &QToolButton::clicked, this, &QtMaterialDatePicker::showToday);

    header->addWidget(m_prevButton, 0, 0);
    header->addWidget(m_headerLabel, 0, 1);
    header->addWidget(m_todayButton, 0, 2);
    header->addWidget(m_nextButton, 0, 3);
    root->addLayout(header);

    m_calendar = new QCalendarWidget(this);
    m_calendar->setObjectName(QStringLiteral("QtMaterialDatePickerCalendar"));
    m_calendar->setGridVisible(false);
    m_calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    m_calendar->setNavigationBarVisible(false);
    m_calendar->setFocusPolicy(Qt::StrongFocus);
    connect(m_calendar, &QCalendarWidget::selectionChanged, this, [this]() {
        updateHeader();
        emit selectedDateChanged(m_calendar->selectedDate());
    });
    connect(m_calendar, &QCalendarWidget::activated, this, &QtMaterialDatePicker::activated);
    root->addWidget(m_calendar);
}

void QtMaterialDatePicker::updateHeader()
{
    const QDate date = m_calendar->selectedDate();
    m_headerLabel->setText(QLocale().toString(date, QStringLiteral("MMMM yyyy")));
    setAccessibleName(tr("Date picker, selected %1").arg(QLocale().toString(date, QLocale::LongFormat)));
}

void QtMaterialDatePicker::applySpec()
{
    setAutoFillBackground(false);
    if (layout()) {
        layout()->setContentsMargins(m_spec.contentMargins);
    }

    QPalette palette = this->palette();
    palette.setColor(QPalette::WindowText, m_spec.foregroundColor);
    palette.setColor(QPalette::Text, m_spec.foregroundColor);
    palette.setColor(QPalette::ButtonText, m_spec.foregroundColor);
    palette.setColor(QPalette::Base, m_spec.backgroundColor);
    palette.setColor(QPalette::Window, m_spec.backgroundColor);
    setPalette(palette);

    m_headerLabel->setFont(m_spec.headlineFont);
    m_headerLabel->setStyleSheet(QStringLiteral("color:%1;").arg(m_spec.headlineColor.name()));
    m_calendar->setFont(m_spec.dayFont);

    QTextCharFormat selected;
    selected.setBackground(m_spec.selectedDateColor);
    selected.setForeground(m_spec.selectedDateTextColor);
    m_calendar->setDateTextFormat(m_calendar->selectedDate(), selected);

    const QString style = QStringLiteral(
        "QCalendarWidget QWidget { background: %1; color: %2; }"
        "QCalendarWidget QAbstractItemView { selection-background-color: %3; selection-color: %4; outline: 0; }"
        "QToolButton { border: 0; border-radius: 16px; padding: 6px 10px; color: %2; background: transparent; }"
        "QToolButton:hover { background: rgba(%5,%6,%7,%8); }")
        .arg(m_spec.backgroundColor.name(),
             m_spec.foregroundColor.name(),
             m_spec.selectedDateColor.name(),
             m_spec.selectedDateTextColor.name())
        .arg(m_spec.hoverColor.red())
        .arg(m_spec.hoverColor.green())
        .arg(m_spec.hoverColor.blue())
        .arg(m_spec.hoverColor.alpha());
    setStyleSheet(style);
}

} // namespace QtMaterial
