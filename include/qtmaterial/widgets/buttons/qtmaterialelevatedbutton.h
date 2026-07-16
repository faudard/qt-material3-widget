#pragma once
#include <QString>
#include <QIcon>
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
namespace QtMaterial {
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialElevatedButton : public QtMaterialFilledButton
{
    Q_OBJECT
public:
    explicit QtMaterialElevatedButton(QWidget* parent = nullptr);
    explicit QtMaterialElevatedButton(const QString& text, QWidget* parent = nullptr);
    QtMaterialElevatedButton(const QIcon& icon, const QString& text, QWidget* parent = nullptr);
    ~QtMaterialElevatedButton() override;
protected:
ButtonSpec resolveButtonSpec() const override;
};
} // namespace QtMaterial
