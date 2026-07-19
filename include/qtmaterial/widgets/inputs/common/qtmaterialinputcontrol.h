#pragma once

#include <QRect>
#include <QString>

#include "qtmaterial/widgets/inputs/common/qtmaterialformfieldcontrol.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialInputControl : public QtMaterialFormFieldControl {
    Q_OBJECT
public:
    explicit QtMaterialInputControl(QWidget* parent = nullptr);
    ~QtMaterialInputControl() override;

    // Backward-compatible aliases kept for the existing input widgets API.
    QString labelText() const;
    void setLabelText(const QString& text);

    QString supportingText() const;
    void setSupportingText(const QString& text);

    QString errorText() const;
    void setErrorText(const QString& text);

    bool hasErrorState() const noexcept;
    void setHasErrorState(bool value);

protected:
    void formFieldChangedEvent() override;
    virtual void syncAccessibilityState();
    virtual void contentChangedEvent();

    QRect contentRect() const;
    QRect supportingTextRect() const;
};

} // namespace QtMaterial
