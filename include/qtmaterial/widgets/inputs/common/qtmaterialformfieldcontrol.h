#pragma once

#include <QString>

#include "qtmaterial/core/qtmaterialcontrol.h"
#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialFormFieldControl : public QtMaterialControl {
    Q_OBJECT
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(QString helperText READ helperText WRITE setHelperText NOTIFY helperTextChanged)
    Q_PROPERTY(QString errorText READ errorText WRITE setErrorText NOTIFY errorTextChanged)
    Q_PROPERTY(bool required READ isRequired WRITE setRequired NOTIFY requiredChanged)
    Q_PROPERTY(bool invalid READ isInvalid WRITE setInvalid NOTIFY invalidChanged)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)

public:
    explicit QtMaterialFormFieldControl(QWidget* parent = nullptr);
    ~QtMaterialFormFieldControl() override;

    QString label() const;
    void setLabel(const QString& label);

    QString helperText() const;
    void setHelperText(const QString& helperText);

    QString errorText() const;
    void setErrorText(const QString& errorText);

    bool isRequired() const noexcept;
    void setRequired(bool required);

    bool isInvalid() const noexcept;
    void setInvalid(bool invalid);

    bool isReadOnly() const noexcept;
    void setReadOnly(bool readOnly);

signals:
    void labelChanged(const QString& label);
    void helperTextChanged(const QString& helperText);
    void errorTextChanged(const QString& errorText);
    void requiredChanged(bool required);
    void invalidChanged(bool invalid);
    void readOnlyChanged(bool readOnly);

protected:
    virtual void formFieldChangedEvent();

private:
    void notifyFormFieldChanged();

    QString m_label;
    QString m_helperText;
    QString m_errorText;
    bool m_required = false;
    bool m_invalid = false;
    bool m_readOnly = false;
};

} // namespace QtMaterial
