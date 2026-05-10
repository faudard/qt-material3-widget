#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"

#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

QtMaterialFilledTonalButton::QtMaterialFilledTonalButton(QWidget* parent)
    : QtMaterialFilledButton(parent)
{
    setMaterialComponent(QStringLiteral("button"));
    setMaterialVariant(QStringLiteral("filled-tonal"));
    setMaterialRole(QStringLiteral("action"));
}

QtMaterialFilledTonalButton::QtMaterialFilledTonalButton(const QString& text, QWidget* parent)
    : QtMaterialFilledButton(parent)
{
    setText(text);
}

QtMaterialFilledTonalButton::QtMaterialFilledTonalButton(const QIcon& icon, const QString& text, QWidget* parent)
    : QtMaterialFilledTonalButton(text, parent)
{
    setIcon(icon);
}


QtMaterialFilledTonalButton::~QtMaterialFilledTonalButton() = default;

ButtonSpec QtMaterialFilledTonalButton::resolveButtonSpec() const
{
    SpecFactory factory;
    return factory.filledTonalButtonSpec(theme(), density());
}

} // namespace QtMaterial
