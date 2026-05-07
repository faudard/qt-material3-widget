
#include "qtmaterial/widgets/inputs/qtmaterialdatefield.h"

#include <QLineEdit>
#include <QToolButton>

QtMaterialDateField::QtMaterialDateField(QWidget* parent)
    : QtMaterialOutlinedTextField(parent)
    , m_displayFormat(QStringLiteral("yyyy-MM-dd"))
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
    return m_date;
}

void QtMaterialDateField::setDate(const QDate& date)
{
    if (m_date == date) {
        return;
    }

    const bool wasAcceptable = isDateAcceptable();
    m_date = date;
    syncEditorFromDate();
    setParseError(m_date.isValid() && !isDateInRange(m_date));
    emit dateChanged(m_date);
    notifyDateAcceptabilityIfChanged(wasAcceptable);
    updateTrailingAffordances();
    updateAccessibilityMetadata();
    update();
}


QString QtMaterialDateField::displayFormat() const
{
    return m_displayFormat;
}

void QtMaterialDateField::setDisplayFormat(const QString& format)
{
    if (m_displayFormat == format) {
        return;
    }
    m_displayFormat = format;
    syncEditorFromDate();
    emit displayFormatChanged(m_displayFormat);
    contentChangedEvent();
}

bool QtMaterialDateField::isClearable() const noexcept
{
    return m_clearable;
}

void QtMaterialDateField::setClearable(bool clearable)
{
    if (m_clearable == clearable) {
        return;
    }
    m_clearable = clearable;
    updateTrailingAffordances();
    emit clearableChanged(m_clearable);
}

QString QtMaterialDateField::placeholderTextForDate() const
{
    return m_placeholderTextForDate;
}

void QtMaterialDateField::setPlaceholderTextForDate(const QString& text)
{
    m_placeholderTextForDate = text;
    if (lineEdit() && lineEdit()->text().isEmpty()) {
        lineEdit()->setPlaceholderText(m_placeholderTextForDate);
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
    if (m_date.isValid()) {
        lineEdit()->setText(m_date.toString(m_displayFormat));
    } else {
        lineEdit()->clear();
        lineEdit()->setPlaceholderText(m_placeholderTextForDate);
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

    const QDate parsed = QDate::fromString(text, m_displayFormat);
    if (!parsed.isValid()) {
        setParseError(true);
        updateAccessibilityMetadata();
        return;
    }

    m_date = parsed;
    setParseError(!isDateInRange(parsed));
    emit dateChanged(m_date);
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
    if (m_lastAccessibilitySummary != summary) {
        m_lastAccessibilitySummary = summary;
        emit accessibilitySummaryChanged(summary);
    }
}


QString QtMaterialDateField::placeholderTextForDate() const
{
    return m_placeholderTextForDate;
}

void QtMaterialDateField::setPlaceholderTextForDate(const QString& text)
{
    if (m_placeholderTextForDate == text) {
        return;
    }

    m_placeholderTextForDate = text;
    if (!date().isValid()) {
        setPlaceholderText(text);
    }
    updateAccessibilityMetadata();
}

QDate QtMaterialDateField::minimumDate() const noexcept
{
    return m_minimumDate;
}

void QtMaterialDateField::setMinimumDate(const QDate& date)
{
    if (m_minimumDate == date) {
        return;
    }

    const bool wasAcceptable = isDateAcceptable();
    m_minimumDate = date;
    if (m_minimumDate.isValid() && m_maximumDate.isValid() && m_minimumDate > m_maximumDate) {
        m_maximumDate = m_minimumDate;
    }

    if (m_date.isValid() && !isDateInRange(m_date)) {
        setParseError(true);
    }

    emit dateRangeChanged(m_minimumDate, m_maximumDate);
    notifyDateAcceptabilityIfChanged(wasAcceptable);
    updateAccessibilityMetadata();
    update();
}

QDate QtMaterialDateField::maximumDate() const noexcept
{
    return m_maximumDate;
}

void QtMaterialDateField::setMaximumDate(const QDate& date)
{
    if (m_maximumDate == date) {
        return;
    }

    const bool wasAcceptable = isDateAcceptable();
    m_maximumDate = date;
    if (m_minimumDate.isValid() && m_maximumDate.isValid() && m_maximumDate < m_minimumDate) {
        m_minimumDate = m_maximumDate;
    }

    if (m_date.isValid() && !isDateInRange(m_date)) {
        setParseError(true);
    }

    emit dateRangeChanged(m_minimumDate, m_maximumDate);
    notifyDateAcceptabilityIfChanged(wasAcceptable);
    updateAccessibilityMetadata();
    update();
}

bool QtMaterialDateField::isDateAcceptable() const noexcept
{
    return !m_parseError && (!m_date.isValid() || isDateInRange(m_date));
}

bool QtMaterialDateField::hasParseError() const noexcept
{
    return m_parseError;
}

QString QtMaterialDateField::accessibilitySummary() const
{
    QStringList parts;
    const QString label = labelText();
    if (!label.isEmpty()) {
        parts << label;
    }
    if (m_date.isValid()) {
        parts << m_date.toString(m_displayFormat);
    } else {
        parts << tr("No date selected");
    }
    if (m_minimumDate.isValid()) {
        parts << tr("Minimum %1").arg(m_minimumDate.toString(m_displayFormat));
    }
    if (m_maximumDate.isValid()) {
        parts << tr("Maximum %1").arg(m_maximumDate.toString(m_displayFormat));
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
    if (m_minimumDate.isValid() && date < m_minimumDate) {
        return false;
    }
    if (m_maximumDate.isValid() && date > m_maximumDate) {
        return false;
    }
    return true;
}

QString QtMaterialDateField::effectiveErrorTextForDate() const
{
    if (m_parseError) {
        if (m_date.isValid() && !isDateInRange(m_date)) {
            if (m_minimumDate.isValid() && m_date < m_minimumDate) {
                return tr("Date must be on or after %1").arg(m_minimumDate.toString(m_displayFormat));
            }
            if (m_maximumDate.isValid() && m_date > m_maximumDate) {
                return tr("Date must be on or before %1").arg(m_maximumDate.toString(m_displayFormat));
            }
        }
        return tr("Enter a valid date");
    }
    return QString();
}

void QtMaterialDateField::setParseError(bool hasError)
{
    if (m_parseError == hasError) {
        return;
    }
    const bool wasAcceptable = isDateAcceptable();
    m_parseError = hasError;
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
