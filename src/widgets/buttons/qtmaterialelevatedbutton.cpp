#include "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h"

#include <QtGlobal>

#include "qtmaterial/specs/qtmaterialbuttonspecresolver.h"

namespace QtMaterial {


QtMaterialElevatedButton::QtMaterialElevatedButton(QWidget* parent)
    : QtMaterialFilledButton(parent)
{
    setMaterialComponent(QStringLiteral("button"));
    setMaterialVariant(QStringLiteral("elevated"));
    setMaterialRole(QStringLiteral("action"));
}

QtMaterialElevatedButton::QtMaterialElevatedButton(const QString& text, QWidget* parent)
    : QtMaterialElevatedButton(parent)
{
    setText(text);
}

QtMaterialElevatedButton::QtMaterialElevatedButton(const QIcon& icon, const QString& text, QWidget* parent)
    : QtMaterialElevatedButton(text, parent)
{
    setIcon(icon);
}

QtMaterialElevatedButton::~QtMaterialElevatedButton() = default;

ButtonSpec QtMaterialElevatedButton::resolveButtonSpec() const
{
    ButtonSpecResolver factory;
    return factory.elevatedButtonSpec(theme(), density());
}


} // namespace QtMaterial
