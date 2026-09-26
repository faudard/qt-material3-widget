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

protected:
    void formFieldChangedEvent() override;
    virtual void syncAccessibilityState();
    virtual void contentChangedEvent();

    QRect contentRect() const;
    QRect supportingTextRect() const;
};

} // namespace QtMaterial
