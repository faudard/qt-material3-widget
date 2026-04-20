#pragma once

#include <QString>

#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialFilledTonalButton : public QtMaterialFilledButton
{
    Q_OBJECT
public:
    explicit QtMaterialFilledTonalButton(QWidget* parent = nullptr);
    explicit QtMaterialFilledTonalButton(const QString& text, QWidget* parent = nullptr);
    ~QtMaterialFilledTonalButton() override;

protected:
    ButtonSpec resolveButtonSpec() const override;
};

} // namespace QtMaterial
