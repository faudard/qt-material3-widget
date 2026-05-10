
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
    void syncEditorFromDate(QtMaterialDateField& q);
    void syncDateFromEditor(QtMaterialDateField& q);
    void updateTrailingAffordances(QtMaterialDateField& q);
    void updateAccessibilityMetadata(QtMaterialDateField& q);
    bool isDateInRange(const QDate& date) const noexcept;
    QString effectiveErrorTextForDate(const QtMaterialDateField& q) const;
    void setParseError(QtMaterialDateField& q, bool hasError);
    void notifyDateAcceptabilityIfChanged(QtMaterialDateField& q, bool previousAcceptable);

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

void QtMaterialDateFieldPrivate::syncEditorFromDate(QtMaterialDateField& q)
{
    if (!q.lineEdit()) {
        return;
    }

    if (m_date.isValid()) {
        q.lineEdit()->setText(m_date.toString(m_displayFormat));
    } else {
        q.lineEdit()->clear();
        q.lineEdit()->setPlaceholderText(m_placeholderTextForDate);
    }
}

void QtMaterialDateFieldPrivate::syncDateFromEditor(QtMaterialDateField& q)
{
    if (!q.lineEdit()) {
        return;
    }

    const QString text = q.lineEdit()->text().trimmed();
    if (text.isEmpty()) {
        q.setDate(QDate());
        setParseError(q, false);
        return;
    }

    const QDate parsed = QDate::fromString(text, m_displayFormat);
    if (!parsed.isValid()) {
        setParseError(q, true);
        updateAccessibilityMetadata(q);
        return;
    }

    m_date = parsed;
    setParseError(q, !isDateInRange(parsed));
    emit q.dateChanged(m_date);
    updateTrailingAffordances(q);
    updateAccessibilityMetadata(q);
}

void QtMaterialDateFieldPrivate::updateTrailingAffordances(QtMaterialDateField& q)
{
    Q_UNUSED(q);
    // Integration direction:
    // - route trailing icon/button through shared prefix/suffix shell
    // - sync icon tint from resolved text-field spec
    // - keep the date shell visually aligned with outlined/filled variants
}

void QtMaterialDateFieldPrivate::updateAccessibilityMetadata(QtMaterialDateField& q)
{
    const QString summary = q.accessibilitySummary();
    q.setAccessibleName(q.labelText().isEmpty() ? q.tr("Date") : q.labelText());
    q.setAccessibleDescription(summary);
    if (m_lastAccessibilitySummary != summary) {
        m_lastAccessibilitySummary = summary;
        emit q.accessibilitySummaryChanged(summary);
    }
}

bool QtMaterialDateFieldPrivate::isDateInRange(const QDate& date) const noexcept
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

QString QtMaterialDateFieldPrivate::effectiveErrorTextForDate(const QtMaterialDateField& q) const
{
    if (m_parseError) {
        if (m_date.isValid() && !isDateInRange(m_date)) {
            if (m_minimumDate.isValid() && m_date < m_minimumDate) {
                return q.tr("Date must be on or after %1").arg(m_minimumDate.toString(m_displayFormat));
            }
            if (m_maximumDate.isValid() && m_date > m_maximumDate) {
                return q.tr("Date must be on or before %1").arg(m_maximumDate.toString(m_displayFormat));
            }
        }
        return q.tr("Enter a valid date");
    }
    return QString();
}

void QtMaterialDateFieldPrivate::setParseError(QtMaterialDateField& q, bool hasError)
{
    if (m_parseError == hasError) {
        return;
    }

    const bool wasAcceptable = q.isDateAcceptable();
    m_parseError = hasError;
    q.setHasErrorState(hasError);
    q.setErrorText(effectiveErrorTextForDate(q));
    emit q.parseErrorChanged(hasError);
    notifyDateAcceptabilityIfChanged(q, wasAcceptable);
    updateAccessibilityMetadata(q);
}

void QtMaterialDateFieldPrivate::notifyDateAcceptabilityIfChanged(QtMaterialDateField& q,
                                                                  bool previousAcceptable)
{
    Q_UNUSED(previousAcceptable);

    q.notifyDateAcceptableIfChanged();
}


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
    d_ptr->syncEditorFromDate(*this);
    d_ptr->setParseError(*this, d_ptr->m_date.isValid() && !d_ptr->isDateInRange(d_ptr->m_date));
    emit dateChanged(d_ptr->m_date);
    d_ptr->notifyDateAcceptabilityIfChanged(*this, wasAcceptable);
    d_ptr->updateTrailingAffordances(*this);
    d_ptr->updateAccessibilityMetadata(*this);
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
    d_ptr->syncEditorFromDate(*this);
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
    d_ptr->updateTrailingAffordances(*this);
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
    d_ptr->updateAccessibilityMetadata(*this);
}

void QtMaterialDateField::themeChangedEvent(const QtMaterial::Theme& theme)
{
    QtMaterialOutlinedTextField::themeChangedEvent(theme);
    d_ptr->updateTrailingAffordances(*this);
}

void QtMaterialDateField::notifyDateAcceptableIfChanged()
{
    const bool acceptable = isDateAcceptable();
    if (acceptable == m_lastDateAcceptable) {
        return;
    }

    m_lastDateAcceptable = acceptable;
    emit dateAcceptableChanged(acceptable);
}

void QtMaterialDateField::handleEditorEditingFinished()
{
    d_ptr->syncDateFromEditor(*this);
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

    if (d_ptr->m_date.isValid() && !d_ptr->isDateInRange(d_ptr->m_date)) {
        d_ptr->setParseError(*this, true);
    }

    emit dateRangeChanged(d_ptr->m_minimumDate, d_ptr->m_maximumDate);
    d_ptr->notifyDateAcceptabilityIfChanged(*this, wasAcceptable);
    d_ptr->updateAccessibilityMetadata(*this);
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

    if (d_ptr->m_date.isValid() && !d_ptr->isDateInRange(d_ptr->m_date)) {
        d_ptr->setParseError(*this, true);
    }

    emit dateRangeChanged(d_ptr->m_minimumDate, d_ptr->m_maximumDate);
    d_ptr->notifyDateAcceptabilityIfChanged(*this, wasAcceptable);
    d_ptr->updateAccessibilityMetadata(*this);
    update();
}

bool QtMaterialDateField::isDateAcceptable() const noexcept
{
    return !d_ptr->m_parseError && (!d_ptr->m_date.isValid() || d_ptr->isDateInRange(d_ptr->m_date));
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
    const QString error = d_ptr->effectiveErrorTextForDate(*this);
    if (!error.isEmpty()) {
        parts << error;
    }
    return parts.join(QStringLiteral(", "));
}










