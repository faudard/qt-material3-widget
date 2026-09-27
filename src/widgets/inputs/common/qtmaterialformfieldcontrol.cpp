#include "qtmaterial/widgets/inputs/common/qtmaterialformfieldcontrol.h"

namespace QtMaterial {

class QtMaterialFormFieldControlPrivate final
{
public:
    QString label;
    QString helperText;
    QString errorText;
    bool required = false;
    bool invalid = false;
    bool readOnly = false;
};

QtMaterialFormFieldControl::QtMaterialFormFieldControl(QWidget* parent)
    : QtMaterialControl(parent)
    , d_ptr(std::make_unique<QtMaterialFormFieldControlPrivate>())
{
}

QtMaterialFormFieldControl::~QtMaterialFormFieldControl() = default;

QString QtMaterialFormFieldControl::label() const { return d_ptr->label; }

void QtMaterialFormFieldControl::setLabel(const QString& label)
{
    if (d_ptr->label == label) return;
    d_ptr->label = label;
    emit labelChanged(d_ptr->label);
    notifyFormFieldChanged();
}

QString QtMaterialFormFieldControl::helperText() const { return d_ptr->helperText; }

void QtMaterialFormFieldControl::setHelperText(const QString& helperText)
{
    if (d_ptr->helperText == helperText) return;
    d_ptr->helperText = helperText;
    emit helperTextChanged(d_ptr->helperText);
    notifyFormFieldChanged();
}

QString QtMaterialFormFieldControl::errorText() const { return d_ptr->errorText; }

void QtMaterialFormFieldControl::setErrorText(const QString& errorText)
{
    if (d_ptr->errorText == errorText) return;
    d_ptr->errorText = errorText;
    emit errorTextChanged(d_ptr->errorText);
    notifyFormFieldChanged();
}

bool QtMaterialFormFieldControl::isRequired() const noexcept { return d_ptr->required; }

void QtMaterialFormFieldControl::setRequired(bool required)
{
    if (d_ptr->required == required) return;
    d_ptr->required = required;
    emit requiredChanged(d_ptr->required);
    notifyFormFieldChanged();
}

bool QtMaterialFormFieldControl::isInvalid() const noexcept { return d_ptr->invalid; }

void QtMaterialFormFieldControl::setInvalid(bool invalid)
{
    if (d_ptr->invalid == invalid) return;
    d_ptr->invalid = invalid;
    interactionState().setError(invalid);
    emit invalidChanged(d_ptr->invalid);
    notifyFormFieldChanged();
}

bool QtMaterialFormFieldControl::isReadOnly() const noexcept { return d_ptr->readOnly; }

void QtMaterialFormFieldControl::setReadOnly(bool readOnly)
{
    if (d_ptr->readOnly == readOnly) return;
    d_ptr->readOnly = readOnly;
    emit readOnlyChanged(d_ptr->readOnly);
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
