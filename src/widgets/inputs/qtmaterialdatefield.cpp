
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
    m_date = date;
    syncEditorFromDate();
    emit dateChanged(m_date);
    contentChangedEvent();
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
    const QString raw = lineEdit()->text().trimmed();
    if (raw.isEmpty()) {
        const bool hadError = m_parseError;
        m_date = QDate();
        m_parseError = false;
        if (hadError != m_parseError) {
            emit parseErrorChanged(m_parseError);
        }
        return;
    }

    const QDate parsed = QDate::fromString(raw, m_displayFormat);
    const bool newError = !parsed.isValid();
    if (!newError && parsed != m_date) {
        m_date = parsed;
        emit dateChanged(m_date);
    }
    if (newError != m_parseError) {
        m_parseError = newError;
        emit parseErrorChanged(m_parseError);
    }
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
    if (accessibleName().isEmpty() && !labelText().isEmpty()) {
        setAccessibleName(labelText());
    }
    if (m_parseError) {
        setAccessibleDescription(QStringLiteral("Invalid date"));
    } else if (m_date.isValid()) {
        setAccessibleDescription(m_date.toString(m_displayFormat));
    } else {
        setAccessibleDescription(QString());
    }
}
