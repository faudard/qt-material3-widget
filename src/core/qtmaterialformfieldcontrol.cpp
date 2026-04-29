#include "qtmaterial/core/qtmaterialformfieldcontrol.h"

namespace QtMaterial {

QtMaterialFormFieldControl::QtMaterialFormFieldControl(QWidget* parent)
    : QtMaterialControl(parent)
{
}

QtMaterialFormFieldControl::~QtMaterialFormFieldControl() = default;

QString QtMaterialFormFieldControl::label() const { return m_label; }

void QtMaterialFormFieldControl::setLabel(const QString& label)
{
    if (m_label == label) return;
    m_label = label;
    emit labelChanged(m_label);
    notifyFormFieldChanged();
}

QString QtMaterialFormFieldControl::helperText() const { return m_helperText; }

void QtMaterialFormFieldControl::setHelperText(const QString& helperText)
{
    if (m_helperText == helperText) return;
    m_helperText = helperText;
    emit helperTextChanged(m_helperText);
    notifyFormFieldChanged();
}

QString QtMaterialFormFieldControl::errorText() const { return m_errorText; }

void QtMaterialFormFieldControl::setErrorText(const QString& errorText)
{
    if (m_errorText == errorText) return;
    m_errorText = errorText;
    emit errorTextChanged(m_errorText);
    notifyFormFieldChanged();
}

bool QtMaterialFormFieldControl::isRequired() const noexcept { return m_required; }

void QtMaterialFormFieldControl::setRequired(bool required)
{
    if (m_required == required) return;
    m_required = required;
    emit requiredChanged(m_required);
    notifyFormFieldChanged();
}

bool QtMaterialFormFieldControl::isInvalid() const noexcept { return m_invalid; }

void QtMaterialFormFieldControl::setInvalid(bool invalid)
{
    if (m_invalid == invalid) return;
    m_invalid = invalid;
    interactionState().setError(invalid);
    emit invalidChanged(m_invalid);
    notifyFormFieldChanged();
}

bool QtMaterialFormFieldControl::isReadOnly() const noexcept { return m_readOnly; }

void QtMaterialFormFieldControl::setReadOnly(bool readOnly)
{
    if (m_readOnly == readOnly) return;
    m_readOnly = readOnly;
    emit readOnlyChanged(m_readOnly);
    notifyFormFieldChanged();
}

void QtMaterialFormFieldControl::formFieldChangedEvent()
{
    invalidateResolvedSpec();
    updateGeometry();
    update();
}

void QtMaterialFormFieldControl::notifyFormFieldChanged()
{
    formFieldChangedEvent();
}

} // namespace QtMaterial
