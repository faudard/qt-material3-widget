#include "qtmaterial/widgets/inputs/qtmaterialtextfieldvalidationgroup.h"

#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

#include <QLineEdit>

namespace QtMaterial {

QtMaterialTextFieldValidationGroup::QtMaterialTextFieldValidationGroup(QObject* parent)
    : QObject(parent)
{
}

QtMaterialTextFieldValidationGroup::~QtMaterialTextFieldValidationGroup() = default;

void QtMaterialTextFieldValidationGroup::addField(QtMaterialOutlinedTextField* field)
{
    if (!field || indexOf(field) >= 0) {
        return;
    }

    m_fields.append(QPointer<QtMaterialOutlinedTextField>(field));

    connect(field, &QObject::destroyed, this, [this, field]() {
        m_fieldLabels.remove(field);
        m_fieldErrorMessages.remove(field);
        removeNullFields();
        refreshAcceptable();
        refreshValidationSummary();
    });

    connect(field, &QtMaterialOutlinedTextField::acceptableInputChanged,
            this, [this](bool) {
                refreshAcceptable();
                refreshValidationSummary();
            });

    refreshAcceptable();
    refreshValidationSummary();
    emit fieldAdded(field);
}

void QtMaterialTextFieldValidationGroup::removeField(QtMaterialOutlinedTextField* field)
{
    const int idx = indexOf(field);
    if (idx < 0) {
        return;
    }

    if (field) {
        field->disconnect(this);
    }

    m_fieldLabels.remove(field);
    m_fieldErrorMessages.remove(field);
    m_fields.removeAt(idx);
    refreshAcceptable();
    refreshValidationSummary();
    emit fieldRemoved(field);
}

void QtMaterialTextFieldValidationGroup::clear()
{
    const auto currentFields = fields();
    for (QtMaterialOutlinedTextField* field : currentFields) {
        removeField(field);
    }

    m_fieldLabels.clear();
    m_fieldErrorMessages.clear();
    removeNullFields();
    refreshAcceptable();
    refreshValidationSummary();
}

QList<QtMaterialOutlinedTextField*> QtMaterialTextFieldValidationGroup::fields() const
{
    QList<QtMaterialOutlinedTextField*> result;
    result.reserve(m_fields.size());

    for (const auto& field : m_fields) {
        if (field) {
            result.append(field.data());
        }
    }

    return result;
}

int QtMaterialTextFieldValidationGroup::count() const noexcept
{
    int result = 0;
    for (const auto& field : m_fields) {
        if (field) {
            ++result;
        }
    }
    return result;
}

bool QtMaterialTextFieldValidationGroup::isEmpty() const noexcept
{
    return count() == 0;
}

bool QtMaterialTextFieldValidationGroup::isAcceptable() const noexcept
{
    return m_acceptable;
}

bool QtMaterialTextFieldValidationGroup::validateAll()
{
    removeNullFields();

    bool acceptable = true;
    for (const auto& field : m_fields) {
        if (field && !field->validateInput()) {
            acceptable = false;
        }
    }

    if (m_acceptable != acceptable) {
        m_acceptable = acceptable;
        emit acceptableChanged(m_acceptable);
    }

    refreshValidationSummary();
    emit validationRequested(acceptable);
    return acceptable;
}

void QtMaterialTextFieldValidationGroup::resetValidationFeedback()
{
    removeNullFields();

    for (const auto& field : m_fields) {
        if (field) {
            field->resetValidationFeedback();
        }
    }

    refreshAcceptable();
    refreshValidationSummary();
}

QtMaterialOutlinedTextField* QtMaterialTextFieldValidationGroup::firstInvalidField() const
{
    for (const auto& field : m_fields) {
        if (field && !field->isAcceptableInput()) {
            return field.data();
        }
    }

    return nullptr;
}

QList<QtMaterialOutlinedTextField*> QtMaterialTextFieldValidationGroup::invalidFields() const
{
    QList<QtMaterialOutlinedTextField*> result;

    for (const auto& field : m_fields) {
        if (field && !field->isAcceptableInput()) {
            result.append(field.data());
        }
    }

    return result;
}

bool QtMaterialTextFieldValidationGroup::focusFirstInvalidField()
{
    QtMaterialOutlinedTextField* field = firstInvalidField();
    if (!field) {
        return false;
    }

    if (QLineEdit* editor = field->lineEdit()) {
        editor->setFocus(Qt::OtherFocusReason);
    } else {
        field->setFocus(Qt::OtherFocusReason);
    }

    return true;
}

void QtMaterialTextFieldValidationGroup::setFieldLabel(QtMaterialOutlinedTextField* field, const QString& label)
{
    if (!field || indexOf(field) < 0) {
        return;
    }

    const QString trimmed = label.trimmed();
    if (trimmed.isEmpty()) {
        m_fieldLabels.remove(field);
    } else {
        m_fieldLabels.insert(field, trimmed);
    }

    refreshValidationSummary();
}

QString QtMaterialTextFieldValidationGroup::fieldLabel(QtMaterialOutlinedTextField* field) const
{
    if (!field) {
        return QString();
    }

    return m_fieldLabels.value(field);
}

void QtMaterialTextFieldValidationGroup::setFieldErrorMessage(QtMaterialOutlinedTextField* field, const QString& message)
{
    if (!field || indexOf(field) < 0) {
        return;
    }

    const QString trimmed = message.trimmed();
    if (trimmed.isEmpty()) {
        m_fieldErrorMessages.remove(field);
    } else {
        m_fieldErrorMessages.insert(field, trimmed);
    }

    refreshValidationSummary();
}

QString QtMaterialTextFieldValidationGroup::fieldErrorMessage(QtMaterialOutlinedTextField* field) const
{
    if (!field) {
        return QString();
    }

    return m_fieldErrorMessages.value(field);
}

QStringList QtMaterialTextFieldValidationGroup::invalidFieldLabels() const
{
    QStringList result;

    for (QtMaterialOutlinedTextField* field : invalidFields()) {
        result.append(effectiveFieldLabel(field));
    }

    return result;
}

QStringList QtMaterialTextFieldValidationGroup::validationSummary() const
{
    return m_validationSummary;
}

QString QtMaterialTextFieldValidationGroup::validationSummaryText() const
{
    return validationSummaryText(QStringLiteral("\n"));
}

QString QtMaterialTextFieldValidationGroup::validationSummaryText(const QString& separator) const
{
    return m_validationSummary.join(separator);
}

int QtMaterialTextFieldValidationGroup::indexOf(QtMaterialOutlinedTextField* field) const noexcept
{
    if (!field) {
        return -1;
    }

    for (int i = 0; i < m_fields.size(); ++i) {
        if (m_fields.at(i) == field) {
            return i;
        }
    }

    return -1;
}

void QtMaterialTextFieldValidationGroup::removeNullFields()
{
    for (int i = m_fields.size() - 1; i >= 0; --i) {
        if (!m_fields.at(i)) {
            m_fields.removeAt(i);
        }
    }
}

bool QtMaterialTextFieldValidationGroup::computeAcceptable() const noexcept
{
    for (const auto& field : m_fields) {
        if (field && !field->isAcceptableInput()) {
            return false;
        }
    }

    return true;
}

void QtMaterialTextFieldValidationGroup::refreshAcceptable()
{
    removeNullFields();

    const bool acceptable = computeAcceptable();
    if (m_acceptable == acceptable) {
        return;
    }

    m_acceptable = acceptable;
    emit acceptableChanged(m_acceptable);
}

QString QtMaterialTextFieldValidationGroup::effectiveFieldLabel(QtMaterialOutlinedTextField* field) const
{
    if (!field) {
        return QString();
    }

    const QString explicitLabel = m_fieldLabels.value(field).trimmed();
    if (!explicitLabel.isEmpty()) {
        return explicitLabel;
    }

    const QString accessibleName = field->accessibleName().trimmed();
    if (!accessibleName.isEmpty()) {
        return accessibleName;
    }

    const QString objectName = field->objectName().trimmed();
    if (!objectName.isEmpty()) {
        return objectName;
    }

    const int idx = indexOf(field);
    return idx >= 0
        ? QStringLiteral("Field %1").arg(idx + 1)
        : QStringLiteral("Field");
}

QString QtMaterialTextFieldValidationGroup::effectiveFieldErrorMessage(QtMaterialOutlinedTextField* field) const
{
    if (!field) {
        return QString();
    }

    const QString explicitMessage = m_fieldErrorMessages.value(field).trimmed();
    if (!explicitMessage.isEmpty()) {
        return explicitMessage;
    }

    const QString accessibleDescription = field->accessibleDescription().trimmed();
    if (!accessibleDescription.isEmpty()) {
        return accessibleDescription;
    }

    return QStringLiteral("Invalid value");
}

QStringList QtMaterialTextFieldValidationGroup::computeValidationSummary() const
{
    QStringList result;

    for (QtMaterialOutlinedTextField* field : invalidFields()) {
        const QString label = effectiveFieldLabel(field);
        const QString message = effectiveFieldErrorMessage(field);

        if (message.isEmpty()) {
            result.append(label);
        } else {
            result.append(QStringLiteral("%1: %2").arg(label, message));
        }
    }

    return result;
}

void QtMaterialTextFieldValidationGroup::refreshValidationSummary()
{
    const QStringList next = computeValidationSummary();
    if (m_validationSummary == next) {
        return;
    }

    m_validationSummary = next;
    emit validationSummaryChanged(m_validationSummary);
}

} // namespace QtMaterial
