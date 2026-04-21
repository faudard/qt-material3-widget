#pragma once

#include <QString>
#include "qtmaterial/core/qtmaterialcontrol.h"

namespace QtMaterial {

class QTMATERIAL3_CORE_EXPORT QtMaterialInputControl : public QtMaterialControl
{
    Q_OBJECT
public:
    explicit QtMaterialInputControl(QWidget* parent = nullptr);
    ~QtMaterialInputControl() override;

    QString labelText() const;
    void setLabelText(const QString& text);

    QString supportingText() const;
    void setSupportingText(const QString& text);

    QString errorText() const;
    void setErrorText(const QString& text);

    bool hasErrorState() const noexcept;
    void setHasErrorState(bool value);

protected:
    virtual void syncAccessibilityState();
    virtual void contentChangedEvent();
    QRect contentRect() const;
    QRect supportingTextRect() const;

private:
    QString m_labelText;
    QString m_supportingText;
    QString m_errorText;
    bool m_hasErrorState = false;
};

} // namespace QtMaterial
