
#include "qtmaterial/widgets/inputs/qtmaterialdatefield.h"

#include <QLineEdit>
#include <QToolButton>
#include <memory>

struct QtMaterialDateFieldPrivate {

    QDate m_date;
    QDate m_minimumDate;
    QDate m_maximumDate;
    mutable QString m_lastAccessibilitySummary;
    QString m_displayFormat = QStringLiteral("yyyy-MM-dd");
    QString m_placeholderTextForDate;
    bool m_clearable = false;
    bool m_parseError = false;
    QToolButton* m_calendarButton = nullptr;
    QToolButton* m_clearButton = nullptr;
};


QtMaterialDateField::QtMaterialDateField(QWidget* parent)
    : QtMaterialOutlinedTextField(parent)
    , d_ptr(std::make_unique<QtMaterialDateFieldPrivate>())
{
    // Future integration direction:
    // - create trailing calendar button
    // - optional clear button
    // - reuse prefix/suffix shell path instead of bespoke layout
    // - parse/format through QLocale or strategy helper
    // - keep date picker optional and externally triggered
}

QtMaterialDateField::QtMaterialDateField(const QString& labelText, QWidget* parent)
    : QtMaterialDateField(parent)
{
    setLabelText(labelText);
}

QtMaterialDateField::~QtMaterialDateField() = default;

QDate QtMaterialDateField::date() const noexcept
{
    return d_ptr->m_date;
}

void QtMaterialDateField::setDate(const QDate& date)
{
    if (d_ptr->m_date == date) {
        return;
    }

    const bool wasAcceptable = isDateAcceptable();
    d_ptr->m_date = date;
    syncEditorFromDate();
    setParseError(d_ptr->m_date.isValid() && !isDateInRange(d_ptr->m_date));
    emit dateChanged(d_ptr->m_date);
    notifyDateAcceptabilityIfChanged(wasAcceptable);
    updateTrailingAffordances();
    updateAccessibilityMetadata();
    update();
}


QString QtMaterialDateField::displayFormat() const
{
    return d_ptr->m_displayFormat;
}

void QtMaterialDateField::setDisplayFormat(const QString& format)
{
    if (d_ptr->m_displayFormat == format) {
        return;
    }
    d_ptr->m_displayFormat = format;
    syncEditorFromDate();
    emit displayFormatChanged(d_ptr->m_displayFormat);
    contentChangedEvent();
}

bool QtMaterialDateField::isClearable() const noexcept
{
    return d_ptr->m_clearable;
}

void QtMaterialDateField::setClearable(bool clearable)
{
    if (d_ptr->m_clearable == clearable) {
        return;
    }
    d_ptr->m_clearable = clearable;
    updateTrailingAffordances();
    emit clearableChanged(d_ptr->m_clearable);
}

QString QtMaterialDateField::placeholderTextForDate() const
{
    return d_ptr->m_placeholderTextForDate;
}

void QtMaterialDateField::setPlaceholderTextForDate(const QString& text)
{
    if (d_ptr->m_placeholderTextForDate == text) {
        return;
    }

    d_ptr->m_placeholderTextForDate = text;
    if (!date().isValid()) {
        setPlaceholderText(text);
    }
    contentChangedEvent();
}


void QtMaterialDateField::contentChangedEvent()
{
    QtMaterialOutlinedTextField::contentChangedEvent();
    updateAccessibilityMetadata();
}

void QtMaterialDateField::themeChangedEvent(const QtMaterial::Theme& theme)
{
    QtMaterialOutlinedTextField::themeChangedEvent(theme);
    updateTrailingAffordances();
}

void QtMaterialDateField::handleEditorEditingFinished()
{
    syncDateFromEditor();
}

void QtMaterialDateField::handleCalendarButtonClicked()
{
    emit calendarRequested();
}

void QtMaterialDateField::handleClearButtonClicked()
{
    setDate(QDate());
    if (lineEdit()) {
        lineEdit()->clear();
    }
    emit parseErrorChanged(false);
}

void QtMaterialDateField::syncEditorFromDate()
{
    if (!lineEdit()) {
        return;
    }
    if (d_ptr->m_date.isValid()) {
        lineEdit()->setText(d_ptr->m_date.toString(d_ptr->m_displayFormat));
    } else {
        lineEdit()->clear();
        lineEdit()->setPlaceholderText(d_ptr->m_placeholderTextForDate);
    }
}

void QtMaterialDateField::syncDateFromEditor()
{
    if (!lineEdit()) {
        return;
    }

    const QString text = lineEdit()->text().trimmed();
    if (text.isEmpty()) {
        setDate(QDate());
        setParseError(false);
        return;
    }

    const QDate parsed = QDate::fromString(text, d_ptr->m_displayFormat);
    if (!parsed.isValid()) {
        setParseError(true);
        updateAccessibilityMetadata();
        return;
    }

    d_ptr->m_date = parsed;
    setParseError(!isDateInRange(parsed));
    emit dateChanged(d_ptr->m_date);
    updateTrailingAffordances();
    updateAccessibilityMetadata();
}


void QtMaterialDateField::updateTrailingAffordances()
{
    // Integration direction:
    // - route trailing icon/button through shared prefix/suffix shell
    // - sync icon tint from resolved text-field spec
    // - keep the date shell visually aligned with outlined/filled variants
}

void QtMaterialDateField::updateAccessibilityMetadata()
{
    const QString summary = accessibilitySummary();
    setAccessibleName(labelText().isEmpty() ? tr("Date") : labelText());
    setAccessibleDescription(summary);
    if (d_ptr->m_lastAccessibilitySummary != summary) {
        d_ptr->m_lastAccessibilitySummary = summary;
        emit accessibilitySummaryChanged(summary);
    }
}

QDate QtMaterialDateField::minimumDate() const noexcept
{
    return d_ptr->m_minimumDate;
}

void QtMaterialDateField::setMinimumDate(const QDate& date)
{
    if (d_ptr->m_minimumDate == date) {
        return;
    }

    const bool wasAcceptable = isDateAcceptable();
    d_ptr->m_minimumDate = date;
    if (d_ptr->m_minimumDate.isValid() && d_ptr->m_maximumDate.isValid() && d_ptr->m_minimumDate > d_ptr->m_maximumDate) {
        d_ptr->m_maximumDate = d_ptr->m_minimumDate;
    }

    if (d_ptr->m_date.isValid() && !isDateInRange(d_ptr->m_date)) {
        setParseError(true);
    }

    emit dateRangeChanged(d_ptr->m_minimumDate, d_ptr->m_maximumDate);
    notifyDateAcceptabilityIfChanged(wasAcceptable);
    updateAccessibilityMetadata();
    update();
}

QDate QtMaterialDateField::maximumDate() const noexcept
{
    return d_ptr->m_maximumDate;
}

void QtMaterialDateField::setMaximumDate(const QDate& date)
{
    if (d_ptr->m_maximumDate == date) {
        return;
    }

    const bool wasAcceptable = isDateAcceptable();
    d_ptr->m_maximumDate = date;
    if (d_ptr->m_minimumDate.isValid() && d_ptr->m_maximumDate.isValid() && d_ptr->m_maximumDate < d_ptr->m_minimumDate) {
        d_ptr->m_minimumDate = d_ptr->m_maximumDate;
    }

    if (d_ptr->m_date.isValid() && !isDateInRange(d_ptr->m_date)) {
        setParseError(true);
    }

    emit dateRangeChanged(d_ptr->m_minimumDate, d_ptr->m_maximumDate);
    notifyDateAcceptabilityIfChanged(wasAcceptable);
    updateAccessibilityMetadata();
    update();
}

bool QtMaterialDateField::isDateAcceptable() const noexcept
{
    return !d_ptr->m_parseError && (!d_ptr->m_date.isValid() || isDateInRange(d_ptr->m_date));
}

bool QtMaterialDateField::hasParseError() const noexcept
{
    return d_ptr->m_parseError;
}

QString QtMaterialDateField::accessibilitySummary() const
{
    QStringList parts;
    const QString label = labelText();
    if (!label.isEmpty()) {
        parts << label;
    }
    if (d_ptr->m_date.isValid()) {
        parts << d_ptr->m_date.toString(d_ptr->m_displayFormat);
    } else {
        parts << tr("No date selected");
    }
    if (d_ptr->m_minimumDate.isValid()) {
        parts << tr("Minimum %1").arg(d_ptr->m_minimumDate.toString(d_ptr->m_displayFormat));
    }
    if (d_ptr->m_maximumDate.isValid()) {
        parts << tr("Maximum %1").arg(d_ptr->m_maximumDate.toString(d_ptr->m_displayFormat));
    }
    const QString error = effectiveErrorTextForDate();
    if (!error.isEmpty()) {
        parts << error;
    }
    return parts.join(QStringLiteral(", "));
}


bool QtMaterialDateField::isDateInRange(const QDate& date) const noexcept
{
    if (!date.isValid()) {
        return true;
    }
    if (d_ptr->m_minimumDate.isValid() && date < d_ptr->m_minimumDate) {
        return false;
    }
    if (d_ptr->m_maximumDate.isValid() && date > d_ptr->m_maximumDate) {
        return false;
    }
    return true;
}

QString QtMaterialDateField::effectiveErrorTextForDate() const
{
    if (d_ptr->m_parseError) {
        if (d_ptr->m_date.isValid() && !isDateInRange(d_ptr->m_date)) {
            if (d_ptr->m_minimumDate.isValid() && d_ptr->m_date < d_ptr->m_minimumDate) {
                return tr("Date must be on or after %1").arg(d_ptr->m_minimumDate.toString(d_ptr->m_displayFormat));
            }
            if (d_ptr->m_maximumDate.isValid() && d_ptr->m_date > d_ptr->m_maximumDate) {
                return tr("Date must be on or before %1").arg(d_ptr->m_maximumDate.toString(d_ptr->m_displayFormat));
            }
        }
        return tr("Enter a valid date");
    }
    return QString();
}

void QtMaterialDateField::setParseError(bool hasError)
{
    if (d_ptr->m_parseError == hasError) {
        return;
    }
    const bool wasAcceptable = isDateAcceptable();
    d_ptr->m_parseError = hasError;
    setHasErrorState(hasError);
    setErrorText(effectiveErrorTextForDate());
    emit parseErrorChanged(hasError);
    notifyDateAcceptabilityIfChanged(wasAcceptable);
    updateAccessibilityMetadata();
}

void QtMaterialDateField::notifyDateAcceptabilityIfChanged(bool previousAcceptable)
{
    const bool nowAcceptable = isDateAcceptable();
    if (previousAcceptable != nowAcceptable) {
        emit dateAcceptableChanged(nowAcceptable);
    }
}
