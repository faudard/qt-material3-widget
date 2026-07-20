#include "qtmaterial/widgets/qtmaterialdatepicker.h"
#include "qtmaterial/core/qtmaterialthemecontextbinding.h"
#include "qtmaterial/specs/qtmaterialdataspecresolver.h"
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
#include <QPen>

namespace QtMaterial {

struct QtMaterialDatePickerPrivate {
    mutable DatePickerSpec m_spec = defaultDatePickerSpec();
    DatePickerSpec m_explicitSpec;
    bool m_explicitSpecSet = false;
    mutable bool m_specDirty = true;
    QtMaterialThemeContextBinding* m_themeBinding = nullptr;
    QDate m_formattedSelectedDate;
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
    d_ptr->m_themeBinding =
        new QtMaterialThemeContextBinding(this, this);
    connect(
        d_ptr->m_themeBinding,
        &QtMaterialThemeContextBinding::effectiveThemeContextChanged,
        this,
        &QtMaterialDatePicker::effectiveThemeContextChanged);
    connect(
        d_ptr->m_themeBinding,
        &QtMaterialThemeContextBinding::themeChanged,
        this,
        [this](const Theme&) {
            if (d_ptr->m_explicitSpecSet) {
                return;
            }
            d_ptr->m_specDirty = true;
            ensureSpecResolved();
            applyResolvedSpec();
        });

    setObjectName(QStringLiteral("QtMaterialDatePicker"));
    setFocusPolicy(Qt::StrongFocus);
    setFrameShape(QFrame::NoFrame);
    buildUi();
    ensureSpecResolved();
    applyResolvedSpec();
    updateHeader();
}

QtMaterialDatePicker::~QtMaterialDatePicker() = default;

void QtMaterialDatePicker::setThemeContext(
    ThemeContext* context)
{
    if (themeContext() == context) {
        return;
    }

    d_ptr->m_themeBinding->setThemeContext(context);
    Q_EMIT themeContextChanged(context);

    if (!d_ptr->m_explicitSpecSet) {
        d_ptr->m_specDirty = true;
        ensureSpecResolved();
        applyResolvedSpec();
    }
}

ThemeContext*
QtMaterialDatePicker::themeContext() const noexcept
{
    return d_ptr->m_themeBinding->themeContext();
}

ThemeContext*
QtMaterialDatePicker::effectiveThemeContext() const noexcept
{
    return d_ptr->m_themeBinding->effectiveThemeContext();
}

QDate QtMaterialDatePicker::selectedDate() const
{
    return d_ptr->m_calendar->selectedDate();
}

void QtMaterialDatePicker::setSelectedDate(
    const QDate& date)
{
    if (!date.isValid()
        || d_ptr->m_calendar->selectedDate() == date) {
        return;
    }

    // QCalendarWidget synchronously emits selectionChanged().
    // The connected handler owns the public selectedDateChanged signal.
    d_ptr->m_calendar->setSelectedDate(date);
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
    return resolvedSpec();
}

const DatePickerSpec&
QtMaterialDatePicker::resolvedSpec() const
{
    ensureSpecResolved();
    return d_ptr->m_spec;
}

void QtMaterialDatePicker::setSpec(
    const DatePickerSpec& spec)
{
    d_ptr->m_explicitSpec = spec;
    d_ptr->m_explicitSpecSet = true;
    d_ptr->m_specDirty = true;
    ensureSpecResolved();
    applyResolvedSpec();
}

bool QtMaterialDatePicker::hasExplicitSpec() const noexcept
{
    return d_ptr->m_explicitSpecSet;
}

void QtMaterialDatePicker::resetSpec()
{
    if (!d_ptr->m_explicitSpecSet) {
        return;
    }

    d_ptr->m_explicitSpecSet = false;
    d_ptr->m_specDirty = true;
    ensureSpecResolved();
    applyResolvedSpec();
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

void QtMaterialDatePicker::changeEvent(
    QEvent* event)
{
    QFrame::changeEvent(event);
    if (!event) {
        return;
    }

    if (event->type() == QEvent::LanguageChange) {
        updateTranslations();
        updateHeader();
        return;
    }

    if (event->type() == QEvent::EnabledChange) {
        applyResolvedSpec();
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
    ensureSpecResolved();
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
    connect(
        d_ptr->m_calendar,
        &QCalendarWidget::selectionChanged,
        this,
        [this]() {
            refreshDateFormats();
            updateHeader();
            Q_EMIT selectedDateChanged(
                d_ptr->m_calendar->selectedDate());
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

void QtMaterialDatePicker::ensureSpecResolved() const
{
    if (!d_ptr->m_specDirty) {
        return;
    }

    if (d_ptr->m_explicitSpecSet) {
        d_ptr->m_spec = d_ptr->m_explicitSpec;
        d_ptr->m_specDirty = false;
        return;
    }

    d_ptr->m_spec =
        DataSpecResolver().datePickerSpec(
            d_ptr->m_themeBinding->theme(),
            Density::Default);
    d_ptr->m_specDirty = false;
}

void QtMaterialDatePicker::refreshDateFormats()
{
    if (!d_ptr->m_calendar) {
        return;
    }

    if (d_ptr->m_formattedSelectedDate.isValid()) {
        d_ptr->m_calendar->setDateTextFormat(
            d_ptr->m_formattedSelectedDate,
            QTextCharFormat());
    }

    QTextCharFormat weekday;
    weekday.setForeground(d_ptr->m_spec.weekdayTextColor);
    weekday.setFont(d_ptr->m_spec.weekdayFont);
    for (int day = Qt::Monday; day <= Qt::Sunday; ++day) {
        d_ptr->m_calendar->setWeekdayTextFormat(
            static_cast<Qt::DayOfWeek>(day),
            weekday);
    }

    const QDate today = QDate::currentDate();
    QTextCharFormat todayFormat;
    todayFormat.setForeground(d_ptr->m_spec.todayOutlineColor);
    todayFormat.setTextOutline(
        QPen(d_ptr->m_spec.todayOutlineColor, 1.0));
    d_ptr->m_calendar->setDateTextFormat(today, todayFormat);

    const QDate selectedDate =
        d_ptr->m_calendar->selectedDate();
    QTextCharFormat selected;
    selected.setBackground(d_ptr->m_spec.selectedDateColor);
    selected.setForeground(d_ptr->m_spec.selectedDateTextColor);
    selected.setFont(d_ptr->m_spec.dayFont);
    d_ptr->m_calendar->setDateTextFormat(
        selectedDate,
        selected);
    d_ptr->m_formattedSelectedDate = selectedDate;
}

void QtMaterialDatePicker::updateTranslations()
{
    d_ptr->m_prevButton->setAccessibleName(
        tr("Previous month"));
    d_ptr->m_nextButton->setAccessibleName(
        tr("Next month"));
    d_ptr->m_todayButton->setText(tr("Today"));
    d_ptr->m_todayButton->setAccessibleName(
        tr("Show today"));
}

void QtMaterialDatePicker::applyResolvedSpec()
{
    ensureSpecResolved();
    setAutoFillBackground(false);

    if (layout()) {
        layout()->setContentsMargins(d_ptr->m_spec.contentMargins);
        layout()->setSpacing(d_ptr->m_spec.contentSpacing);
    }

    QPalette resolvedPalette = palette();
    resolvedPalette.setColor(
        QPalette::WindowText,
        d_ptr->m_spec.foregroundColor);
    resolvedPalette.setColor(
        QPalette::Text,
        d_ptr->m_spec.foregroundColor);
    resolvedPalette.setColor(
        QPalette::ButtonText,
        d_ptr->m_spec.navigationButtonTextColor);
    resolvedPalette.setColor(
        QPalette::Base,
        d_ptr->m_spec.backgroundColor);
    resolvedPalette.setColor(
        QPalette::Window,
        d_ptr->m_spec.backgroundColor);
    resolvedPalette.setColor(
        QPalette::Disabled,
        QPalette::Text,
        d_ptr->m_spec.disabledTextColor);
    resolvedPalette.setColor(
        QPalette::Disabled,
        QPalette::ButtonText,
        d_ptr->m_spec.disabledTextColor);
    setPalette(resolvedPalette);
    d_ptr->m_calendar->setPalette(resolvedPalette);

    d_ptr->m_headerLabel->setFont(d_ptr->m_spec.headlineFont);
    d_ptr->m_headerLabel->setMinimumHeight(
        d_ptr->m_spec.headerHeight);
    d_ptr->m_headerLabel->setStyleSheet(
        QStringLiteral("color:%1;")
            .arg(d_ptr->m_spec.headlineColor.name()));

    d_ptr->m_calendar->setFont(d_ptr->m_spec.dayFont);
    d_ptr->m_calendar->setMinimumSize(
        d_ptr->m_spec.cellSize * 7,
        d_ptr->m_spec.cellSize * 7);

    refreshDateFormats();

    const QString style = QStringLiteral(
        "QCalendarWidget QWidget {"
        " background: %1;"
        " color: %2;"
        "}"
        "QCalendarWidget QAbstractItemView {"
        " selection-background-color: %3;"
        " selection-color: %4;"
        " outline: 0;"
        "}"
        "QCalendarWidget QAbstractItemView:disabled {"
        " color: %5;"
        "}"
        "QToolButton {"
        " border: 0;"
        " border-radius: %6px;"
        " padding: %7px %8px;"
        " color: %9;"
        " background: transparent;"
        "}"
        "QToolButton:hover {"
        " background: rgba(%10,%11,%12,%13);"
        "}"
        "QToolButton:disabled {"
        " color: %5;"
        "}")
        .arg(
            d_ptr->m_spec.backgroundColor.name(),
            d_ptr->m_spec.foregroundColor.name(),
            d_ptr->m_spec.selectedDateColor.name(),
            d_ptr->m_spec.selectedDateTextColor.name(),
            d_ptr->m_spec.disabledTextColor.name())
        .arg(d_ptr->m_spec.navigationButtonRadius)
        .arg(d_ptr->m_spec.navigationButtonPaddingVertical)
        .arg(d_ptr->m_spec.navigationButtonPaddingHorizontal)
        .arg(d_ptr->m_spec.navigationButtonTextColor.name())
        .arg(d_ptr->m_spec.hoverColor.red())
        .arg(d_ptr->m_spec.hoverColor.green())
        .arg(d_ptr->m_spec.hoverColor.blue())
        .arg(d_ptr->m_spec.hoverColor.alpha());

    setStyleSheet(style);
    updateGeometry();
    update();
}

} // namespace QtMaterial
